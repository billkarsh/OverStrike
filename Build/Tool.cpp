
#include "Tool.h"
#include "CGBL.h"
#include "Util.h"
#include "Subset.h"

#include <QFileInfo>


/* ---------------------------------------------------------------- */
/* Tool ----------------------------------------------------------- */
/* ---------------------------------------------------------------- */

Tool::Tool()
{
}


Tool::~Tool()
{
}


void Tool::entrypoint()
{
    if( !good_files() )
        return;

    secs_to_samps();

    if( !channel_counts() )
        return;

    size_buffer();

    run();
}

/* ---------------------------------------------------------------- */
/* Private -------------------------------------------------------- */
/* ---------------------------------------------------------------- */

bool Tool::good_files()
{
    if( !GBL.file.endsWith( ".bin" ) ) {
        Log() << QString("File not *.bin [%1].").arg( GBL.file );
        return false;
    }

    QFileInfo   fi( GBL.file );
    if( !fi.exists() ) {
        Log() << QString("File doesn't exist [%1].").arg( GBL.file );
        return false;
    }

    QString smeta = GBL.file;
    smeta.replace( ".bin", ".meta" );

    fi.setFile( smeta );
    if( !fi.exists() ) {
        Log() << QString("Metafile doesn't exist [%1].").arg( smeta );
        return false;
    }

    if( !kvp.fromMetaFile( smeta ) ) {
        Log() << QString("Metafile corrupt [%1].").arg( smeta );
        return false;
    }

    if( kvp["typeThis"].toString() != "imec" ) {
        Log() << QString("Not an imec probe file [%1].").arg( smeta );
        return false;
    }

    return true;
}


void Tool::secs_to_samps()
{
    double  srate = kvp["imSampRate"].toDouble();

    if( !GBL.samps.size() ) {
        for( int i = 0; i < 2; ++i )
            GBL.samps.push_back( GBL.secs[i] * srate );
    }
}


bool Tool::channel_counts()
{
    nG      = kvp["nSavedChans"].toInt();
    nN      = 0;
    smpEOF  = kvp["fileSizeBytes"].toLongLong() / (nG*sizeof(qint16));

    GBL.samps[1] = qBound( 0LL, GBL.samps[1], smpEOF );
    GBL.samps[0] = qBound( 0LL, GBL.samps[0], GBL.samps[1] );

    if( GBL.samps[0] >= GBL.samps[1] )
        return false;

    if( GBL.usrZ.size() ) {

        // idx_SY used to test if acq channel index is neural

        QStringList sl;
        sl = kvp["acqApLfSy"].toString().split(
                QRegExp("^\\s+|\\s*,\\s*"),
                QString::SkipEmptyParts );
        int idx_SY = sl[0].toInt() + sl[1].toInt();

        // ig2ic used to get acq channel index

        QVector<uint>   ig2ic;
        QString         chnstr = kvp["snsSaveChanSubset"].toString();
        if( Subset::isAllChansStr( chnstr ) )
            Subset::defaultVec( ig2ic, nG );
        else
            Subset::rngStr2Vec( ig2ic, chnstr );

        // validate user list of extant neural channels

        foreach( uint C, GBL.usrZ ) {
            if( C < idx_SY ) {
                int ig = ig2ic.indexOf( C );
                if( ig >= 0 )
                    vZ.push_back( ig );
            }
        }
        qSort( vZ );
    }
    else {

        // Calculate nN

        QStringList sl;
        sl = kvp["snsApLfSy"].toString().split(
                QRegExp("^\\s+|\\s*,\\s*"),
                QString::SkipEmptyParts );
        nN = sl[0].toInt() + sl[1].toInt();
    }

    return true;
}


void Tool::size_buffer()
{
    bufSmps = 1024;
    buf.resize( bufSmps * nG );
}


void Tool::run()
{
    if( GBL.linefil )
        doLines();
    else
        doZeros();
}


void Tool::doLines()
{
    vec_i16 Ya, Yb;
    Ya.resize( nG );
    Yb.resize( nG );

    QFile   f( GBL.file );
    qint64  smpRem      = GBL.samps[1] - GBL.samps[0],
            smpCur      = GBL.samps[0],
            Xab         = smpRem + 1;
    char    *B          = (char*)&buf[0];
    int     smpBytes    = nG * sizeof(qint16),
            i0          = 0;

    f.open( QIODevice::ReadWrite );

// Ya

    if( smpCur > 0 )
        f.seek( smpBytes * (smpCur - 1) );
    else
        --Xab;
    f.read( (char*)&Ya[0], smpBytes );

// Yb

    if( GBL.samps[1] < smpEOF )
        f.seek( smpBytes * GBL.samps[1] );
    else {
        f.seek( smpBytes * (GBL.samps[1] - 1) );
        --Xab;
    }
    f.read( (char*)&Yb[0], smpBytes );

// buffer-sized chunks

    f.seek( smpBytes * smpCur );

    do {
        int smpThis = qMin( smpRem, qint64(bufSmps) );

        f.read( B, smpBytes * smpThis );

        line_fill_buf( &Ya[0], &Yb[0], Xab, i0, i0 + smpThis );

        f.seek( smpBytes * (smpCur + i0) );
        f.write( B, smpBytes * smpThis );

        i0      += smpThis;
        smpRem -= smpThis;

    } while( smpRem > 0 );
}


void Tool::line_fill_buf(
    const qint16    *Ya,
    const qint16    *Yb,
    qint64          Xab,
    int             i0,
    int             iLim )
{
    qint16  *d  = &buf[0];

    if( vZ.size() ) {
        uint    *pz = &vZ[0];
        int      nz = vZ.size();
        for( int i = i0; i < iLim; ++i, d += nG ) {
            double  dx = double(i) / Xab;
            for( int iz = 0; iz < nz; ++iz ) {
                int ig = pz[iz];
                d[ig] = Ya[ig] + dx * (Yb[ig] - Ya[ig]);
            }
        }
    }
    else {
        for( int i = i0; i < iLim; ++i, d += nG ) {
            double  dx = double(i) / Xab;
            for( int ig = 0; ig < nN; ++ig )
                d[ig] = Ya[ig] + dx * (Yb[ig] - Ya[ig]);
        }
    }
}


void Tool::doZeros()
{
    QFile   f( GBL.file );
    qint64  smpRem      = GBL.samps[1] - GBL.samps[0],
            smpCur      = GBL.samps[0];
    char    *B          = (char*)&buf[0];
    int     smpBytes    = nG * sizeof(qint16),
            smpThis;

    f.open( QIODevice::ReadWrite );

    if( smpCur > 0 )
        f.seek( smpBytes * smpCur );

    if( vZ.size() ) {
        uint    *pz = &vZ[0];
        int      nz = vZ.size();
        for(;;) {
            smpThis = qMin( smpRem, qint64(bufSmps) );
            f.read( B, smpBytes * smpThis );

            qint16  *b = (qint16*)B;
            for( int it = 0; it < smpThis; ++it, b += nG ) {
                for( int iz = 0; iz < nz; ++iz )
                    b[pz[iz]] = 0;
            }

            f.seek( smpBytes * smpCur );
            f.write( B, smpBytes * smpThis );
            if( (smpRem -= smpThis) <= 0 )
                break;
            smpCur += smpThis;
        }
    }
    else if( nN < nG ) {
        for(;;) {
            smpThis = qMin( smpRem, qint64(bufSmps) );
            f.read( B, smpBytes * smpThis );

            for( int it = 0; it < smpThis; ++it )
                memset( B + smpBytes * it, 0, sizeof(qint16)*nN );

            f.seek( smpBytes * smpCur );
            f.write( B, smpBytes * smpThis );
            if( (smpRem -= smpThis) <= 0 )
                break;
            smpCur += smpThis;
        }
    }
    else {
        smpThis = qMin( smpRem, qint64(bufSmps) );
        memset( B, 0, smpBytes * smpThis );
        for(;;) {
            f.write( B, smpBytes * smpThis );
            if( (smpRem -= smpThis) <= 0 )
                break;
            smpThis = qMin( qint64(bufSmps), smpRem );
        }
    }
}



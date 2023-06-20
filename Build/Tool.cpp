
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

    lf = GBL.file.contains( ".lf." );
    return true;
}


void Tool::secs_to_samps()
{
    srate = kvp["imSampRate"].toDouble();

    if( !GBL.samps.size() ) {
        for( int i = 0; i < 2; ++i )
            GBL.samps.push_back( GBL.secs[i] * srate );
    }
}


bool Tool::channel_counts()
{
    QStringList sl;
    sl = kvp["acqApLfSy"].toString().split(
            QRegExp("^\\s+|\\s*,\\s*"),
            QString::SkipEmptyParts );
    for( int i = 0; i < 3; ++i )
        acq[i] = sl[i].toInt();

    nG      = kvp["nSavedChans"].toInt();
    SY      = acq[0] + acq[1];
    smpEOF  = kvp["fileSizeBytes"].toLongLong() / (nG*sizeof(qint16));

    GBL.samps[1] = qBound( 0LL, GBL.samps[1], smpEOF );
    GBL.samps[0] = qBound( 0LL, GBL.samps[0], GBL.samps[1] );

    if( GBL.samps[0] >= GBL.samps[1] )
        return false;

    Subset::rngStr2Vec( ig2ic, kvp["snsSaveChanSubset"].toString() );
    isSY = ig2ic[nG-1] >= SY;

    foreach( uint C, GBL.usrZ ) {
        if( C < SY ) {
            int ig = ig2ic.indexOf( C );
            if( ig >= 0 )
                vZ.push_back( ig );
        }
    }
    qSort( vZ );

    return true;
}


void Tool::size_buffer()
{
    bufSmps = 1024;
    buf.resize( bufSmps * nG );
}


void Tool::run()
{
    QFile   f( GBL.file );
    qint64  remSmps     = GBL.samps[1] - GBL.samps[0],
            curSmp      = GBL.samps[0];
    char    *B          = (char*)&buf[0];
    int     smpBytes    = nG * sizeof(qint16),
            doSmps;

    f.open( QIODevice::ReadWrite );

    if( curSmp > 0 )
        f.seek( curSmp * smpBytes );

    if( vZ.size() ) {
        uint    *pz = &vZ[0];
        int      nz = vZ.size();
        for(;;) {
            doSmps = qMin( qint64(bufSmps), remSmps );
            f.read( B, doSmps * smpBytes );

            qint16  *b = (qint16*)B;
            for( int it = 0; it < doSmps; ++it, b += nG ) {
                for( int iz = 0; iz < nz; ++iz )
                    b[pz[iz]] = 0;
            }

            f.seek( curSmp * smpBytes );
            f.write( B, doSmps * smpBytes );
            if( (remSmps -= doSmps) <= 0 )
                break;
            curSmp += doSmps;
        }
    }
    else if( isSY ) {
        for(;;) {
            doSmps = qMin( qint64(bufSmps), remSmps );
            f.read( B, doSmps * smpBytes );

            for( int it = 0; it < doSmps; ++it )
                memset( B + it * smpBytes, 0, smpBytes - sizeof(qint16) );

            f.seek( curSmp * smpBytes );
            f.write( B, doSmps * smpBytes );
            if( (remSmps -= doSmps) <= 0 )
                break;
            curSmp += doSmps;
        }
    }
    else {
        doSmps = qMin( qint64(bufSmps), remSmps );
        memset( B, 0, doSmps * smpBytes );
        for(;;) {
            f.write( B, doSmps * smpBytes );
            if( (remSmps -= doSmps) <= 0 )
                break;
            doSmps = qMin( qint64(bufSmps), remSmps );
        }
    }
}



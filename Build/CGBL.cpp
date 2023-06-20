
#include "CGBL.h"
#include "Cmdline.h"
#include "Util.h"
#include "Subset.h"


/* --------------------------------------------------------------- */
/* Globals ------------------------------------------------------- */
/* --------------------------------------------------------------- */

CGBL    GBL;

/* --------------------------------------------------------------- */
/* PrintUsage  --------------------------------------------------- */
/* --------------------------------------------------------------- */

static void PrintUsage()
{
    Log();
    Log() << "*** ERROR: MISSING CRITICAL PARAMETERS ***";
    Log() << "------------------------";
    Log() << "Purpose:";
    Log() << "+ Overwrite noise with zeros in imec probe data.";
    Log() << "Run messages are appended to OverStrike.log in the current working directory.\n";
    Log() << "Usage:";
    Log() << ">OverStrike -file=path/file.bin <-samps=a,b | -secs=a,b> [ options ]\n";
    Log() << "Options:";
    Log() << "-chans=0,3:5    ;only zero these channels (acquisition indices)";
    Log() << "------------------------";
}

/* ---------------------------------------------------------------- */
/* CGBL ----------------------------------------------------------- */
/* ---------------------------------------------------------------- */

/* --------------------------------------------------------------- */
/* Public -------------------------------------------------------- */
/* --------------------------------------------------------------- */

bool CGBL::SetCmdLine( int argc, char* argv[] )
{
// Parse args

    const char  *sarg = 0;

    for( int i = 1; i < argc; ++i ) {

        if( GetArgStr( sarg, "-file=", argv[i] ) )
            file = trim_adjust_slashes( sarg );
        else if( GetArgList( samps, "-samp=", argv[i] ) ) {
            if( samps.size() != 2 )
                goto bad_param;
        }
        else if( GetArgList( secs, "-secs=", argv[i] ) ) {
            if( secs.size() != 2 )
                goto bad_param;
        }
        else if( GetArgStr( sarg, "-chans=", argv[i] ) ) {
            if( !chans_parse( sarg ) ) {
                Log() << "Bad format in -chans list.";
                return false;
            }
        }
        else {
bad_param:
            Log() <<
            QString("Did not understand option '%1'.").arg( argv[i] );
            return false;
        }
    }

// Check args

    if( file.isEmpty() ) {
        PrintUsage();
        return false;
    }

    if( samps.size() ) {
        if( samps[0] >= samps[1] ) {
            Log() << QString("Error: samps A >= B.").arg( samps[0] ).arg( samps[1] );
            return false;
        }
    }
    else if( secs.size() ) {
        if( secs[0] >= secs[1] ) {
            Log() << QString("Error: secs A >= B.").arg( secs[0] ).arg( secs[1] );
            return false;
        }
    }
    else {
        PrintUsage();
        return false;
    }

    return true;
}

/* --------------------------------------------------------------- */
/* Private ------------------------------------------------------- */
/* --------------------------------------------------------------- */

QString CGBL::trim_adjust_slashes( const QString &dir )
{
    QString s = dir.trimmed();

    s.replace( "\\", "/" );
    return s.remove( QRegExp("/+$") );
}


bool CGBL::chans_parse( const QString s )
{
    if( s == "*" || !s.compare( "all", Qt::CaseInsensitive ) )
        return true;
    return Subset::rngStr2Vec( usrZ, s );
}



#ifndef CGBL_H
#define CGBL_H

#include <QVector>

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

class CGBL
{
public:
    std::vector<double> secs;
    std::vector<qint64> samps;
    QString             file;
    QVector<uint>       usrZ;
    bool                linefil;

public:
    CGBL() : linefil(false) {}

    bool SetCmdLine( int argc, char* argv[] );

private:
    QString trim_adjust_slashes( const QString &dir );
    bool chans_parse( const QString s );
};

/* --------------------------------------------------------------- */
/* Globals ------------------------------------------------------- */
/* --------------------------------------------------------------- */

extern CGBL GBL;

#endif  // CGBL_H



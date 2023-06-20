#ifndef TOOL_H
#define TOOL_H

#include "KVParams.h"
#include "SGLTypes.h"

#include <QVector>

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

class Tool
{
private:
    double          srate;
    qint64          smpEOF;
    KVParams        kvp;
    QVector<uint>   ig2ic,
                    vZ;
    vec_i16         buf;
    int             acq[3],
                    nG,
                    SY,
                    bufSmps;
    bool            lf,
                    isSY;

public:
    Tool();
    virtual ~Tool();

    void entrypoint();

private:
    bool good_files();
    void secs_to_samps();
    bool channel_counts();
    void size_buffer();
    void run();
};

#endif  // TOOL_H



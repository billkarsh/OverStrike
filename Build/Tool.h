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
    qint64          smpEOF;
    KVParams        kvp;
    QVector<uint>   vZ;
    vec_i16         buf;
    int             nG,
                    nN,
                    bufSmps;

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
    void doLines();
    void line_fill_buf(
        const qint16    *Ya,
        const qint16    *Yb,
        qint64          Xab,
        int             i0,
        int             iLim );
    void doZeros();
};

#endif  // TOOL_H



#ifndef miscoDYN_BUF_H
#define miscoDYN_BUF_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of miscoDYN_BUF class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "misc.h"


/*
 * Class declaration
 */

/**
 * miscoDYN_BUF is an interface class providing all the necessary API to manage
 * auto-expanding, byte-based buffers.
 */
class miscoDYN_BUF
{
public:
    // Class constructor
    miscoDYN_BUF();

    // Assignment operator and copy constructor
    miscoDYN_BUF& operator=(const miscoDYN_BUF &dynBuf);
    miscoDYN_BUF(const miscoDYN_BUF &dynBuf);

    // Class destructor
    virtual ~miscoDYN_BUF();

    miscDYN_BUF*  GetInternalMiscDYN_BUF ();

    mcsCOMPL_STAT Reserve                (const miscDynSIZE length);

    mcsCOMPL_STAT Alloc                  (const miscDynSIZE length);

    mcsCOMPL_STAT Strip                  (void);

    mcsCOMPL_STAT Reset                  (void);

    mcsCOMPL_STAT GetNbStoredBytes       (miscDynSIZE      *storedBytes) const;

    mcsCOMPL_STAT GetNbAllocatedBytes    (miscDynSIZE      *allocatedBytes) const;

    char*         GetBuffer              (void) const;

    const char*   GetCommentPattern      (void) const;

    const char*   GetNextLine            (const char      *currentPos,
                                          char            *nextLine,
                                          const mcsUINT32  maxLineLength,
                                          const mcsLOGICAL skipCommentFlag = mcsTRUE);

    const char*   GetNextCommentLine     (const char        *currentPos,
                                          char        *nextLine,
                                          const mcsUINT32   maxLineLength);

    mcsCOMPL_STAT GetByteAt              (      char       *byte,
                                          const miscDynSIZE position);

    mcsCOMPL_STAT GetBytesFromTo         (      char       *bytes,
                                          const miscDynSIZE from,
                                          const miscDynSIZE to);

    mcsCOMPL_STAT GetStringFromTo        (      char       *str,
                                          const miscDynSIZE from,
                                          const miscDynSIZE to);

    mcsCOMPL_STAT SetCommentPattern      (const char       *commentPattern);

    mcsINT8       ExecuteCommand         (const char       *command);

    mcsCOMPL_STAT LoadFile               (const char       *fileName,
                                          const char       *commentPattern = NULL);

    mcsCOMPL_STAT SaveInASCIIFile        (const char       *fileName);

    mcsCOMPL_STAT SaveBufferedToFile     (const char       *fileName);
    mcsLOGICAL    IsSavingBuffer         ();
    mcsCOMPL_STAT SaveBufferIfNeeded     ();
    mcsCOMPL_STAT CloseFile              ();
    
    mcsCOMPL_STAT ReplaceByteAt          (const char        byte,
                                          const miscDynSIZE position);

    mcsCOMPL_STAT ReplaceBytesFromTo     (const char       *bytes,
                                          const miscDynSIZE length,
                                          const miscDynSIZE from,
                                          const miscDynSIZE to);

    mcsCOMPL_STAT ReplaceStringFromTo    (const char       *str,
                                          const miscDynSIZE from,
                                          const miscDynSIZE to);

    mcsCOMPL_STAT AppendBytes            (const char       *bytes,
                                          const miscDynSIZE length);

    mcsCOMPL_STAT AppendString           (const char       *str);

    mcsCOMPL_STAT AppendLine             (const char       *line);

    mcsCOMPL_STAT AppendCommentLine      (const char       *line);

    mcsCOMPL_STAT InsertBytesAt          (const char       *bytes,
                                          const miscDynSIZE length,
                                          const miscDynSIZE position);

    mcsCOMPL_STAT InsertStringAt         (const char       *str,
                                          const miscDynSIZE position);

    mcsCOMPL_STAT DeleteBytesFromTo      (const miscDynSIZE from,
                                          const miscDynSIZE to);

    friend  std::ostream&                operator<<(      std::ostream&   stream,
            const miscoDYN_BUF&   buffer);

protected:
    miscDYN_BUF _dynBuf;

private:
} ;

#endif /*!miscoDYN_BUF_H*/

/*___oOo___*/

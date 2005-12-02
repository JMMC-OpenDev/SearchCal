#ifndef miscoDYN_BUF_H
#define miscoDYN_BUF_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoDYN_BUF.h,v 1.12 2005-12-02 13:10:36 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/05/26 13:48:45  lafrasse
 * Code review : added const attribute to parameters that should have it, replaced the Display() method by operator<<(), and changed doxygen tag from '\' to '@'
 *
 * Revision 1.10  2005/04/08 06:54:32  gluck
 * Code review: minor changes
 *
 * Revision 1.9  2005/02/22 15:42:45  lafrasse
 * Added a default value to the miscoDYN_BUF::GetNextLine() 'skipCommentFlag' parameter (mcsTRUE)
 *
 * Revision 1.8  2005/02/22 15:10:53  lafrasse
 * Removed documentation duplication between 'misc' and 'misco', changed miscoDYN_BUF::GetNextLine() API, added miscoDYN_BUF::GetNextCommentLine(), miscoDYN_BUF::AppendLine() and miscoDYN_BUF::AppendCommentLine()
 *
 * Revision 1.7  2005/02/16 14:57:23  gzins
 * Updated prototype to GetNextLine
 *
 * Revision 1.6  2005/02/14 14:10:13  scetre
 * move _dynBuf from private to protected
 *
 * Revision 1.5  2005/02/14 08:09:04  gzins
 * Implemented assignment operator and copy constructor
 *
 * Revision 1.4  2005/02/13 11:02:17  gzins
 * Set mcsTRUE as default value to skipCommentFlag parameter
 *
 * Revision 1.3  2005/02/12 20:04:44  gzins
 * Go back to version 1.1
 *
 * Revision 1.1  2005/02/11 09:37:57  gzins
 * Created
 *
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

    mcsCOMPL_STAT Alloc              (const mcsINT32   length);

    mcsCOMPL_STAT Strip              (void);

    mcsCOMPL_STAT Reset              (void);

    mcsCOMPL_STAT GetNbStoredBytes   (mcsUINT32        *storedBytes) const;

    mcsCOMPL_STAT GetNbAllocatedBytes(mcsUINT32        *allocatedBytes) const;

    char*         GetBuffer          (void) const;

    const char*   GetCommentPattern  (void) const;

    const char*   GetNextLine        (const char       *currentPos,
                                            char       *nextLine,
                                      const mcsUINT32  maxLineLength,
                                      const mcsLOGICAL skipCommentFlag=mcsTRUE);

    const char*   GetNextCommentLine (const char        *currentPos,
                                            char        *nextLine,
                                      const mcsUINT32   maxLineLength);

    mcsCOMPL_STAT GetByteAt          (      char       *byte,
                                      const mcsUINT32  position);

    mcsCOMPL_STAT GetBytesFromTo     (      char       *bytes,
                                      const mcsUINT32  from,
                                      const mcsUINT32  to);

    mcsCOMPL_STAT GetStringFromTo    (      char       *str,
                                      const mcsUINT32  from,
                                      const mcsUINT32  to);

    mcsCOMPL_STAT SetCommentPattern  (const char       *commentPattern);

    mcsCOMPL_STAT LoadFile           (const char       *fileName,
                                      const char       *commentPattern=NULL);

    mcsCOMPL_STAT SavePartInFile     (const mcsUINT32   length,
                                      const char       *fileName);

    mcsCOMPL_STAT SaveInFile         (const char       *fileName);

    mcsCOMPL_STAT SaveInASCIIFile    (const char       *fileName);

    mcsCOMPL_STAT ReplaceByteAt      (const char       byte,
                                      const mcsUINT32  position);

    mcsCOMPL_STAT ReplaceBytesFromTo (const char       *bytes,
                                      const mcsUINT32  length,
                                      const mcsUINT32  from,
                                      const mcsUINT32  to);

    mcsCOMPL_STAT ReplaceStringFromTo(const char       *str,
                                      const mcsUINT32  from,
                                      const mcsUINT32  to);

    mcsCOMPL_STAT AppendBytes        (const char       *bytes,
                                      const mcsUINT32  length);

    mcsCOMPL_STAT AppendString       (const char       *str);

    mcsCOMPL_STAT AppendLine         (const char       *line);

    mcsCOMPL_STAT AppendCommentLine  (const char       *line);

    mcsCOMPL_STAT InsertBytesAt      (const char       *bytes,
                                      const mcsUINT32  length,
                                      const mcsUINT32  position);

    mcsCOMPL_STAT InsertStringAt     (const char       *str,
                                      const mcsUINT32  position);

    mcsCOMPL_STAT DeleteBytesFromTo  (const mcsUINT32  from,
                                      const mcsUINT32  to);

    friend  std::ostream&    operator<<(      std::ostream&   stream,
                                        const miscoDYN_BUF&   buffer);

protected:
    miscDYN_BUF _dynBuf;

private:
};

#endif /*!miscoDYN_BUF_H*/

/*___oOo___*/

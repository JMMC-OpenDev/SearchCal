#ifndef miscoDYN_BUF_H
#define miscoDYN_BUF_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoDYN_BUF.h,v 1.2 2005-02-11 11:04:38 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * 
 * \n
 * \ex
 * \code
 * #include <stdlib.h>
 * #include <iostream>
 * 
 * using namespace std;
 * 
 * #define MODULE_ID "mymod"
 *
 * #include "mcs.h"
 * #include "log.h"
 * #include "err.h"
 * #include "misco.h"
 * 
 * int main(int argc, char *argv[])
 * {
 *     // Initialize MCS services
 *     if (mcsInit(argv[0]) == FAILURE)
 *     {
 *         // Exit from the application with FAILURE
 *         exit (EXIT_FAILURE);
 *     }
 * 
 *     miscoDYN_BUF buf;
 * 
 *     buf.LoadFile("../doc/moduleDescription.xml");
 *     char* temp = NULL;
 *     while ((temp = buf.GetNextLine(temp, mcsTRUE)) != NULL)
 *     {
 *         cout << "Line = '" << temp << "'" << endl;
 *     }
 * 
 *     // Close MCS services
 *     mcsExit();
 *     
 *     // Exit from the application with SUCCESS
 *     exit (EXIT_SUCCESS);
 * }
 * \endcode
 */
class miscoDYN_BUF
{

public:
    // Class constructor
    miscoDYN_BUF();

    // Class destructor
    virtual ~miscoDYN_BUF();

    mcsCOMPL_STAT Alloc               (const mcsINT32    length);

    mcsCOMPL_STAT Strip               ();

    mcsCOMPL_STAT Reset               ();

    mcsCOMPL_STAT GetNbStoredBytes    (mcsUINT32         *storedBytes) const;

    mcsCOMPL_STAT GetNbAllocatedBytes (mcsUINT32         *allocatedBytes) const;

    char*         GetBuffer           () const;

    const char*   GetCommentPattern   () const;

    char*         GetNextLine         (const char        *currentLinePtr,
                                       const mcsLOGICAL  skipCommentFlag);

    mcsCOMPL_STAT GetByteAt           (      char        *byte,
                                       const mcsUINT32   position);

    mcsCOMPL_STAT GetBytesFromTo      (      char        *bytes,
                                       const mcsUINT32   from,
                                       const mcsUINT32   to);

    mcsCOMPL_STAT GetStringFromTo     (      char        *str,
                                       const mcsUINT32   from,
                                       const mcsUINT32   to);

    mcsCOMPL_STAT SetCommentPattern   (const char        *commentPattern);

    mcsCOMPL_STAT LoadFile            (const char        *fileName,
                                       const char        *commentPattern=NULL);

    mcsCOMPL_STAT SaveInFile          (const char        *fileName);

    mcsCOMPL_STAT ReplaceByteAt       (      char        byte,
                                       const mcsUINT32   position);

    mcsCOMPL_STAT ReplaceBytesFromTo  (      char        *bytes,
                                       const mcsUINT32   length,
                                       const mcsUINT32   from,
                                       const mcsUINT32   to);

    mcsCOMPL_STAT ReplaceStringFromTo (      char        *str,
                                       const mcsUINT32   from,
                                       const mcsUINT32   to);

    mcsCOMPL_STAT AppendBytes         (const char        *bytes,
                                       const mcsUINT32   length);

    mcsCOMPL_STAT AppendString        (const char        *str);

    mcsCOMPL_STAT InsertBytesAt       (      char        *bytes,
                                       const mcsUINT32   length,
                                       const mcsUINT32   position);

    mcsCOMPL_STAT InsertStringAt      (      char        *str,
                                       const mcsUINT32   position);

    mcsCOMPL_STAT DeleteBytesFromTo   (const mcsUINT32   from,
                                       const mcsUINT32   to);

    mcsCOMPL_STAT Display   () const;

//    friend  std::ostream&    operator<<(      std::ostream&   stream,
//                                        const miscoDYN_BUF&   buffer);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    miscoDYN_BUF(const miscoDYN_BUF&);
    miscoDYN_BUF& operator=(const miscoDYN_BUF&);

    miscDYN_BUF  dynBuf;
    miscDYN_BUF* buffer;
};

#endif /*!miscoDYN_BUF_H*/

/*___oOo___*/

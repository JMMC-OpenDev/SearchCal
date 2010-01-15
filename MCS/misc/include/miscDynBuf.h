#ifndef miscDynBuf_H
#define miscDynBuf_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscDynBuf.h,v 1.24 2010-01-15 17:03:30 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.23  2005/12/02 13:04:32  lafrasse
 * Added miscDynBufSavePartInFile() and miscDynBufSaveInASCIIFile().
 * Changed miscDynBufSaveInFile() to rely on miscDynBufSavePartInFile().
 *
 * Revision 1.22  2005/05/26 13:03:44  lafrasse
 * Code review : added const attribute to necessary parameters, and change doxygen attributes from '\' to '@'
 *
 * Revision 1.21  2005/05/17 15:34:41  lafrasse
 * Re-ordered functions
 *
 * Revision 1.20  2005/03/03 16:10:31  gluck
 * Code review corrections: code + documentation ...
 *
 * Revision 1.19  2005/02/22 11:11:38  lafrasse
 * Added miscDynBufGetNextCommentLine(), miscDynBufAppendLine() and miscDynBufAppendComentLine()
 *
 * Revision 1.18  2005/02/16 14:39:55  gzins
 * Updated miscDynBufGetNextLine() function to do not alter buffer content.
 *
 * Revision 1.17  2005/02/15 12:40:22  gzins
 * Removed miscDynBufGetNextLinePointer and miscDynBufGetBufferPointer macros
 *
 * Revision 1.16  2005/02/10 10:08:07  lafrasse
 * Added miscDynBufSaveInFile(), and moved as most miscDynBuf parameters as possible to const type
 *
 * Revision 1.15  2005/02/03 08:59:24  gzins
 * Defined 'bytes' parameter as constant in miscDynBufAppendBytes
 * Defined 'str' parameter as constant in miscDynBufAppendString
 *
 * Revision 1.14  2005/01/28 18:10:17  gzins
 * Renamed miscDynBufGetBufferPointer to miscDynBufGetBuffer
 * Renamed miscDynBufGetNextLinePointer to miscDynBufGetNextLine
 * Added macros for backward compatibility
 *
 * Revision 1.13  2005/01/28 17:54:41  gzins
 * Declared dynBuf parameter of miscDynBufGetBufferPointer as const
 *
 * gzins     21-Dec-2004  Renamed miscDynBufGetStoredBytesNumber to
 *                        miscDynBufGetNbStoredBytes and
 *                        miscDynBufGetAllocatedBytesNumber to
 * lafrasse  08-Nov-2004  Added miscDynBufGetNextLinePointer() and
 *                        miscDynBufLoadFile() function, plus a new field in
 *                        the Dynamic Buffer structure to store the comment
 *                        pattern to be skipped by
 *                        miscDynBufGetNextLinePointer() with
 *                        miscDynBufGetCommentPattern() and
 *                        miscDynBufSetCommentPattern() to deal with this field
 * lafrasse  30-Sep-2004  Changed miscDynBufAlloc second parameter type from
 *                        mcsUINT32 to mcsINT32
 * lafrasse  23-Aug-2004  Moved miscDynBufInit from local to public
 * lafrasse  02-Aug-2004  Moved mcs.h include in from miscDynBuf.c
 *                        Moved in null-terminated string specific functions
 *                        from miscDynStr.h
 * lafrasse  23-Jul-2004  Moved miscDYN_BUF_MAGIC_STRUCTURE_ID to miscPrivate.h,
 *                        added error management to
 *                        miscDynBufGetStoredBytesNumber and
 *                        miscDynBufGetAllocatedBytesNumber, plus
 *                        miscDynBufGetBytesFromTo parameter refinments.
 * lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
 * lafrasse  06-Jul-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of miscDynBuf functions.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++ code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/* 
 * MCS Headers
 */
#include "mcs.h"


/* 
 * Macro definition
 */

/**
 * Dynamic Buffer first position number abstraction.
 *
 * It is meant to make independant all the code from the number internally used
 * to reference the first byte of a Dynamic Buffer, in order to make your work
 * independant of our futur hypotetic implementation changes.
 */
#define miscDYN_BUF_BEGINNING_POSITION  ((mcsUINT32) 1u)


/*
 * Structure type definition
 */

/**
 * A Dynamic Buffer structure.
 *
 * It holds all the informtations needed to manage a miscDynBuf Dynamic Buffer.
 */
typedef struct
{
    void        *thisPointer;      /**< A pointer to itself. This is used to
                                     allow the Dynamic Buffer structure
                                     initialization state test (whether it has
                                     already been initialized as a miscDYN_BUF
                                     or not). */

    mcsUINT32   magicStructureId;  /**< A magic number to identify in a univocal
                                     way a MCS structure. This is used to allow
                                     the Dynamic Buffer structure initialization
                                     state test(whether it has already been
                                     initialized  as a miscDYN_BUF or not). */

    char        *dynBuf;           /**< A pointer to the Dynamic Buffer internal
                                     bytes buffer. */

    mcsSTRING4  commentPattern;    /**< A byte array containing the pattern
                                     identifying the comment to be skipped. */

    mcsUINT32   storedBytes;       /**< An unsigned integer counting the number
                                     of bytes effectively holden by Dynamic
                                     Buffer.
                                     */

    mcsUINT32   allocatedBytes;    /**< An unsigned integer counting the number
                                     of bytes already allocated in Dynamic
                                     Buffer. */
} miscDYN_BUF;


/*
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscDynBufInit                (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const mcsINT32    length);

mcsCOMPL_STAT miscDynBufStrip               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufGetNbStoredBytes    (const miscDYN_BUF *dynBuf,
                                             mcsUINT32         *storedBytes);

mcsCOMPL_STAT miscDynBufGetNbAllocatedBytes (const miscDYN_BUF *dynBuf,
                                             mcsUINT32         *allocatedBytes);

char*         miscDynBufGetBuffer           (const miscDYN_BUF *dynBuf);

mcsCOMPL_STAT miscDynBufGetByteAt           (const miscDYN_BUF *dynBuf,
                                             char              *byte,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufGetBytesFromTo      (const miscDYN_BUF *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufGetStringFromTo     (const miscDYN_BUF *dynBuf,
                                             char              *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

const char*   miscDynBufGetNextLine         (const miscDYN_BUF *dynBuf,
                                             const char        *currentPos,
                                             char              *nextLine,
                                             const mcsUINT32   maxLineLength,
                                             const mcsLOGICAL  skipCommentFlag);

const char*   miscDynBufGetNextCommentLine  (const miscDYN_BUF *dynBuf,
                                             const char        *currentPos,
                                                   char        *nextCommentLine,
                                             const mcsUINT32   maxCommentLineLength);

const char*   miscDynBufGetCommentPattern   (const miscDYN_BUF *dynBuf);

mcsCOMPL_STAT miscDynBufSetCommentPattern   (miscDYN_BUF       *dynBuf,
                                             const char        *commentPattern);

mcsCOMPL_STAT miscDynBufExecuteCommand      (miscDYN_BUF       *dynBuf,
                                             const char        *command);

mcsCOMPL_STAT miscDynBufLoadFile            (miscDYN_BUF       *dynBuf,
                                             const char        *fileName,
                                             const char        *commentPattern);

mcsCOMPL_STAT miscDynBufSavePartInFile      (const miscDYN_BUF *dynBuf,
                                             const mcsUINT32   length,
                                             const char        *fileName);

mcsCOMPL_STAT miscDynBufSaveInFile          (const miscDYN_BUF *dynBuf,
                                             const char        *fileName);

mcsCOMPL_STAT miscDynBufSaveInASCIIFile     (const miscDYN_BUF *dynBuf,
                                             const char        *fileName);

mcsCOMPL_STAT miscDynBufReplaceByteAt       (miscDYN_BUF       *dynBuf,
                                             const char        byte,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufReplaceBytesFromTo  (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufReplaceStringFromTo (miscDYN_BUF       *dynBuf,
                                             const char        *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const mcsUINT32   length);

mcsCOMPL_STAT miscDynBufAppendString        (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufAppendLine          (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufAppendCommentLine   (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufInsertStringAt      (miscDYN_BUF       *dynBuf,
                                             const char        *str,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

#ifdef __cplusplus
}
#endif

#endif /*!miscDynBuf_H*/

/*___oOo___*/

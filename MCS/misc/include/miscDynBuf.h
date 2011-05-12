#ifndef miscDynBuf_H
#define miscDynBuf_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

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
extern "C"
{
#endif


/*
 * System Headers
 */
#include <stdlib.h>

/*
 * MCS Headers
 */
#include "mcs.h"


/*
 * Macro definition
 */

/** use 64bits for size_type */
#define miscDynSIZE size_t

/**
 * Dynamic Buffer first position number abstraction.
 *
 * It is meant to make independant all the code from the number internally used
 * to reference the first byte of a Dynamic Buffer, in order to make your work
 * independant of our futur hypotetic implementation changes.
 */
#define miscDYN_BUF_BEGINNING_POSITION  ((miscDynSIZE) 1u)


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

    miscDynSIZE storedBytes;       /**< An unsigned integer counting the number
                                     of bytes effectively holden by Dynamic
                                     Buffer.
                                     */

    miscDynSIZE allocatedBytes;    /**< An unsigned integer counting the number
                                     of bytes already allocated in Dynamic
                                     Buffer. */

    FILE*       fileDesc;           /**<optional FILE descriptor to read/write */

    miscDynSIZE fileStoredBytes;    /**< An unsigned integer counting the number
                                     of bytes effectively in the FILE descriptor.
                                     */

    miscDynSIZE fileOffsetBytes;    /**< An unsigned integer counting the number
                                     of bytes read in the FILE descriptor.
                                     */
} miscDYN_BUF;


/*
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscDynBufInit                (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufReserve             (miscDYN_BUF       *dynBuf,
                                             const miscDynSIZE  length);

mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const miscDynSIZE  length);

mcsCOMPL_STAT miscDynBufStrip               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufGetNbStoredBytes    (const miscDYN_BUF *dynBuf,
                                             miscDynSIZE       *storedBytes);

mcsCOMPL_STAT miscDynBufGetNbAllocatedBytes (const miscDYN_BUF *dynBuf,
                                             miscDynSIZE       *allocatedBytes);

char*         miscDynBufGetBuffer           (const miscDYN_BUF *dynBuf);

mcsCOMPL_STAT miscDynBufGetByteAt           (const miscDYN_BUF *dynBuf,
                                             char              *byte,
                                             const miscDynSIZE position);

mcsCOMPL_STAT miscDynBufGetBytesFromTo      (const miscDYN_BUF *dynBuf,
                                             char              *bytes,
                                             const miscDynSIZE from,
                                             const miscDynSIZE to);

mcsCOMPL_STAT miscDynBufGetStringFromTo     (const miscDYN_BUF *dynBuf,
                                             char              *str,
                                             const miscDynSIZE from,
                                             const miscDynSIZE to);

const char*   miscDynBufGetNextLine         (      miscDYN_BUF *dynBuf,
                                             const char        *currentPos,
                                             char              *nextLine,
                                             const mcsUINT32   maxLineLength,
                                             const mcsLOGICAL  skipCommentFlag);

const char*   miscDynBufGetNextCommentLine  (      miscDYN_BUF *dynBuf,
                                             const char        *currentPos,
                                             char        *nextCommentLine,
                                             const mcsUINT32   maxCommentLineLength);

const char*   miscDynBufGetCommentPattern   (const miscDYN_BUF *dynBuf);

mcsCOMPL_STAT miscDynBufSetCommentPattern   (miscDYN_BUF       *dynBuf,
                                             const char        *commentPattern);

mcsCOMPL_STAT miscDynBufReplaceByteAt       (miscDYN_BUF       *dynBuf,
                                             const char        byte,
                                             const miscDynSIZE position);

mcsCOMPL_STAT miscDynBufReplaceBytesFromTo  (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const miscDynSIZE length,
                                             const miscDynSIZE from,
                                             const miscDynSIZE to);

mcsCOMPL_STAT miscDynBufReplaceStringFromTo (miscDYN_BUF       *dynBuf,
                                             const char        *str,
                                             const miscDynSIZE from,
                                             const miscDynSIZE to);

mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const miscDynSIZE length);

mcsCOMPL_STAT miscDynBufAppendString        (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufAppendLine          (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufAppendCommentLine   (miscDYN_BUF       *dynBuf,
                                             const char        *str);

mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             const char        *bytes,
                                             const miscDynSIZE length,
                                             const miscDynSIZE position);

mcsCOMPL_STAT miscDynBufInsertStringAt      (miscDYN_BUF       *dynBuf,
                                             const char        *str,
                                             const miscDynSIZE position);

mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const miscDynSIZE from,
                                             const miscDynSIZE to);

/* file I/O */
mcsCOMPL_STAT miscDynBufLoadFile            (miscDYN_BUF       *dynBuf,
                                             const char        *fileName,
                                             const char        *commentPattern);

mcsCOMPL_STAT miscDynBufSavePartInFile      (const miscDYN_BUF *dynBuf,
                                             const miscDynSIZE  length,
                                             const char        *fileName);

mcsCOMPL_STAT miscDynBufSaveInFile          (const miscDYN_BUF *dynBuf,
                                             const char        *fileName);

mcsCOMPL_STAT miscDynBufSaveInASCIIFile     (const miscDYN_BUF *dynBuf,
                                             const char        *fileName);

/* Command handling */
mcsINT8       miscDynBufExecuteCommand      (miscDYN_BUF       *dynBuf,
                                             const char        *command);

#ifdef __cplusplus
}
#endif

#endif /*!miscDynBuf_H*/

/*___oOo___*/

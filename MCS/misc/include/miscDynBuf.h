#ifndef miscDynBuf_H
#define miscDynBuf_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscDynBuf.h,v 1.9 2004-08-23 15:08:02 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  06-Jul-2004  Created
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
* lafrasse  23-Jul-2004  Moved miscDYN_BUF_MAGIC_STRUCTURE_ID to miscPrivate.h,
*                        added error management to
*                        miscDynBufGetStoredBytesNumber and
*                        miscDynBufGetAllocatedBytesNumber, plus
*                        miscDynBufGetBytesFromTo parameter refinments.
* lafrasse  02-Aug-2004  Moved mcs.h include in from miscDynBuf.c
*                        Moved in null-terminated string specific functions
*                        from miscDynStr.h
* lafrasse  23-Aug-2004  Moved miscDynBufInit from local to public
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the miscDynBuf functions declarations.
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
 * It is meant to isolate all of your code from the number internally used to
 * reference the first byte of a Dynamic Buffer, in order to make your work
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
                                     allow the testing of a Dynamic Buffer
                                     struture initialization state (wether
                                     it has allready been initialized as a
                                     miscDYN_BUF or not). */

    mcsUINT32   magicStructureId;  /**< A magic number to unically identify a
                                     MCS structure. This is used to allow the
                                     testing of a Dynamic Buffer struture
                                     initialization state (wether it has
                                     allready been initialized  as a miscDYN_BUF
                                     or not). */

    char        *dynBuf;           /**< A pointer to the Dynamic Buffer internal
                                     bytes buffer. */

    mcsUINT32   storedBytes;       /**< An unsigned integer counting the number
                                     of bytes effectively held by a Dynamic
                                     Buffer.
                                     */

    mcsUINT32   allocatedBytes;    /**< An unsigned integer counting the number
                                     of bytes allready allocated in a Dynamic
                                     Buffer. */
} miscDYN_BUF;


/*
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscDynBufInit                (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   length);

mcsCOMPL_STAT miscDynBufStrip               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufGetStoredBytesNumber(miscDYN_BUF       *dynBuf,
                                             mcsUINT32         *storedBytes);

mcsCOMPL_STAT miscDynBufGetAllocatedBytesNumber(
                                             miscDYN_BUF       *dynBuf,
                                             mcsUINT32         *allocatedBytes);

char*         miscDynBufGetBufferPointer    (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufGetByteAt           (miscDYN_BUF       *dynBuf,
                                             char              *byte,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufGetBytesFromTo      (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufGetStringFromTo     (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufReplaceByteAt       (miscDYN_BUF       *dynBuf,
                                             char              byte,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufReplaceBytesFromTo  (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufReplaceStringFromTo (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length);

mcsCOMPL_STAT miscDynBufAppendString        (miscDYN_BUF       *dynBuf,
                                             char              *str);

mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufInsertStringAt      (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

#ifdef __cplusplus
}
#endif

#endif /*!miscDynBuf_H*/

/*___oOo___*/

#ifndef miscDynBuf_H
#define miscDynBuf_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscDynBuf.h,v 1.1 2004-07-08 12:06:46 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  06-Jul-2004  Created
*
*
*******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++ code.
*/

#ifdef __cplusplus
extern "C" {
#endif


/* 
 * Constants definition
 */
#define miscDYN_BUF_MAGIC_STRUCTURE_ID ((mcsUINT32)2813741963u)

#define miscDYN_BUF_BEGINNING_POSTION  ((mcsUINT32)1u)


/*
 * Structure type definition
 */
typedef struct
{
    void      *thisPointer;
    mcsUINT32 magicStructureId;

    char      *dynBuf;
    mcsUINT32 storedBytes;
    mcsUINT32 allocatedBytes;
} miscDYN_BUF;


/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   length);

mcsCOMPL_STAT miscDynBufStrip               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf);

mcsUINT32     miscDynBufGetStoredBytesNumber(miscDYN_BUF       *dynBuf);

mcsUINT32     miscDynBufGetAllocatedBytesNumber(
                                             miscDYN_BUF       *dynBuf);

char*         miscDynBufGetBufferPointer    (miscDYN_BUF       *dynBuf);

mcsCOMPL_STAT miscDynBufGetByteAt           (miscDYN_BUF       *dynBuf,
                                             char              *byte,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufGetBytesFromTo      (miscDYN_BUF       *dynBuf,
                                             char              **bytes,
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

mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length);

mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   position);

mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

#ifdef __cplusplus
}
#endif


#endif /*!miscDynBuf_H*/


/*___oOo___*/

/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: miscDynBuf.c,v 1.6 2004-07-20 13:58:40 lafrasse Exp $"
 *
 * who       when         what
 * --------  -----------  ------------------------------------------------------
 * lafrasse  05-Jul-2004  Created
 * lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
 * lafrasse  12-Jul-2004  Code factorization and error codes polishing
 * lafrasse  19-Jul-2004  Corrected some bugs ('from = to' parameters)
 * lafrasse  20-Jul-2004  Used 'memmove()' instead of temporary buffers
 *
 *
 ******************************************************************************/

/**
 * \file
 * Contains all the miscDynBuf Dynamic Buffer functions definitions.
 * 
 * All the algorithms behind Dynamic Buffer management are grouped in this file.
 *
 * \sa To see all those functions declarations and a minimal code example, see
 * miscDynBuf.h
 * \sa To see all the other 'misc' module functions declarations, see misc.h
 */

static char *rcsId="@(#) $Id: miscDynBuf.c,v 1.6 2004-07-20 13:58:40 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <string.h>
#include <stdlib.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"


/* 
 * Local Headers
 */
#include "miscDynBuf.h"
#include "miscPrivate.h"
#include "miscErrors.h"


/* 
 * Local functions declaration 
 */
static        mcsCOMPL_STAT miscDynBufInit  (miscDYN_BUF        *dynBuf);

static        mcsCOMPL_STAT miscDynBufVerifyPositionParameterValidity(
                                             miscDYN_BUF        *dynBuf,
                                             const mcsUINT32    position);

static        mcsCOMPL_STAT miscDynBufVerifyFromToParametersValidity(
                                             miscDYN_BUF        *dynBuf,
                                             const mcsUINT32    from,
                                             const mcsUINT32    to);

static        mcsCOMPL_STAT miscDynBufVerifyBytesAndLengthParametersValidity(
                                             char              *bytes,
                                             const mcsUINT32   length);


/* 
 * Local functions definition
 */

/**
 * Verify if a Dynamic Buffer has already been initialized, else initialize it.
 *
 * This function is used internally, in order to test if a Dynamic Buffer has
 * been initialized before use. If not, the Dynamic Buffer will be initialized.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
static        mcsCOMPL_STAT miscDynBufInit  (miscDYN_BUF        *dynBuf)
{
    char *functionName = "miscDynBufInit\0";
    
    /* Test the 'dynBuf' parameter validity... */
    if (dynBuf == NULL)
    {
        errAdd(miscERR_DYN_BUF_NULL_VALUE, functionName, "dynBuf");
        return FAILURE;
    }

    /* Test the 'pointer to itself' and 'structure identifier magic number'
     * validity...
     */
    if (dynBuf->thisPointer != dynBuf
        || dynBuf->magicStructureId != miscDYN_BUF_MAGIC_STRUCTURE_ID)
    {
        /* Try to initialize all the structure with '0' */
        if ((memset(dynBuf, 0, sizeof(miscDYN_BUF))) == NULL)
        {
            errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
            return FAILURE;
        }

        /* Set its 'pointer to itself' correctly */
        dynBuf->thisPointer = dynBuf;
        /* Set its 'structure identifier magic number' correctly */
        dynBuf->magicStructureId = miscDYN_BUF_MAGIC_STRUCTURE_ID;
    }

    return SUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'position' is correct (eg. inside the Dynamic Buffer range.
 *
 * This function is only used internally by funtions receiving 'position'
 * parameter.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param position a position inside the Dynamic Buffer
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
static        mcsCOMPL_STAT miscDynBufVerifyPositionParameterValidity(
                                             miscDYN_BUF        *dynBuf,
                                             const mcsUINT32    position)
{
    char *functionName = "miscDynBufVerifyPositionParameterValidity\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* Test the position parameter validity... */
    if (position < miscDYN_BUF_BEGINNING_POSITION
        || position > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, functionName, "position");
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'from' and 'to' position are correct (eg. inside the Dynamic Buffer range,
 * and "from lower than 'to').
 *
 * This function is only used internally by funtions receiving 'from' and 'to'
 * parameters.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param from a position inside the Dynamic Buffer
 * \param to a position inside the Dynamic Buffer
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
static        mcsCOMPL_STAT miscDynBufVerifyFromToParametersValidity(
                                             miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    char *functionName = "miscDynBufVerifyFromToParametersValidity\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* Test the 'from' parameter validity */
    if (from < miscDYN_BUF_BEGINNING_POSITION || from > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, functionName, "from");
        return FAILURE;
    }

    /* Test the 'to' parameter validity */
    if (to < miscDYN_BUF_BEGINNING_POSITION || to > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, functionName, "to");
        return FAILURE;
    }

    /* Test the 'from' and 'to' parameters validity against each other */
    if (to < from)
    {
        errAdd(miscERR_DYN_BUF_BAD_FROM_TO, functionName);
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'from' and 'to' position are correct (eg. inside the Dynamic Buffer range,
 * and "from lower than 'to').
 *
 * This function is only used internally by funtions receiving 'from' and 'to'
 * parameters.
 *
 * \param bytes the bytes buffer pointer to test
 * \param length the number of bytes in the buffer to test
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
static        mcsCOMPL_STAT miscDynBufVerifyBytesAndLengthParametersValidity(
                                             char              *bytes,
                                             const mcsUINT32   length)
{
    char *functionName = "miscDynBufVerifyBytesAndLengthParametersValidity\0";

    /* Test the 'bytes' parameter validity */
    if (bytes == NULL)
    {
        errAdd(miscERR_DYN_BUF_NULL_VALUE, functionName, "bytes");
        return FAILURE;
    }

    /* Test the 'length' parameter validity */
    if (length == 0)
    {
        errAdd(miscERR_DYN_BUF_NULL_VALUE, functionName, "length");
        return FAILURE;
    }

    return SUCCESS;
}



/*
 * Public functions definition
 */

/**
 * Allocate and add a number of bytes to a Dynamic Buffer already allocated
 * bytes.
 *
 * If the Dynamic Buffer already has some allocated bytes, its length is
 * expanded by the desired one. If the Dynamic Buffer is enlarged, the previous
 * content will remain untouched after the reallocation. New allocated bytes
 * will all contain '0'.
 *
 * \remark The call to this function is optional, as a Dynamic Buffer will
 * expand itself on demand when invoquing other miscDynBuf functions as
 * miscDynBufAppendBytes(), miscDynBufInsertBytesAt(), etc... So, this
 * function call is only usefull when you know by advance the maximum bytes
 * length the Dynamic Buffer can reach accross its entire life, and thus want
 * to minimize the CPU time spent to expand the Dynamic Buffer allocated
 * memory on demand.\n\n
 *  
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param length the number of bytes by which the Dynamic Buffer should be
 * expanded
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   length)
{
    char *functionName = "miscDynBufAlloc\0";
    char *newBuf = NULL;

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* If the current buffer already has the desired length... */
    if (length == 0)
    {
        /* Do nothing */
        return SUCCESS;
    }

    /* If the buffer has no memory allocated... */
    if (dynBuf->allocatedBytes == 0)
    {
        /* Try to allocate the desired length */
        if ((dynBuf->dynBuf = calloc(length, sizeof(char))) == NULL)
        {
            errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
            return FAILURE;
        }

        /* Try to Write '0' on all the newly allocated memory */
        if (memset(dynBuf->dynBuf, 0, length) == NULL)
        {
            errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
            return FAILURE;
        }
    }
    else /* The buffer needs to be expanded... */
    {
        /* Try to get more memory */
        if ((newBuf = realloc(dynBuf->dynBuf, (dynBuf->storedBytes + length)))
             == NULL)
        {
            errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
            return FAILURE;
        }

        /* Store the expanded buffer address */
        dynBuf->dynBuf = newBuf;

        /* If the buffer was containig nothing... */
        if (dynBuf->storedBytes == 0)
        {
            /* Try to write '0' on all the newly allocated memory */
            if ((memset(dynBuf->dynBuf, 0, dynBuf->allocatedBytes)) == NULL)
            {
                errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
                return FAILURE;
            }
        }
    }

    /* Increase the buffer allocated length value */
    dynBuf->allocatedBytes += length;

    return SUCCESS;
}


/**
 * Dealloc the unused allocated memory of a Dynamic Buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufStrip(miscDYN_BUF *dynBuf)
{
    char *functionName = "miscDynBufStrip\0";
    char *newBuf = NULL;

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* Try to give back the unused memory */
    if ((newBuf = realloc(dynBuf->dynBuf, dynBuf->storedBytes)) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Store the new buffer address */
    dynBuf->dynBuf = newBuf;

    /* Update the buffer allocated length value */
    dynBuf->allocatedBytes = dynBuf->storedBytes;

    return SUCCESS;
}


/**
 * Reset a Dynamic Buffer.
 *
 * Possibly allocated memory remains untouched, until the reseted Dynamic Buffer
 * is reused to store other bytes.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf)
{
    char *functionName = "miscDynBufReset\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* Update the Dynamic Buffer stored bytes number value, to make it act as
     * if there were nothing in the buffer
     */
    dynBuf->storedBytes = 0;

    return SUCCESS;
}


/**
 * Destroy a Dynamic Buffer.
 *
 * Possibly allocated memory is freed and zerod - so be sure that it is
 * desirable to delete the data contained inside the buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf)
{
    char *functionName = "miscDynBufDestroy\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return FAILURE;
    }

    /* If some memory was allocated... */
    if (dynBuf->allocatedBytes != 0)
    {
        /* Free the allocated memory */
        free(dynBuf->dynBuf);
    }

    /* Try to initialize all the structure with '0' */
    if ((memset((char *)dynBuf, 0, sizeof(miscDYN_BUF))) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * Return the number of bytes stored in a Dynamic Buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return the stored length of a Dynamic Buffer, or 0 if an error occured
 */
mcsUINT32     miscDynBufGetStoredBytesNumber(miscDYN_BUF       *dynBuf)
{
    char *functionName = "miscDynBufGetStoredBytesNumber\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return 0;
    }

    /* Return the Dynamic Buffer stored bytes number */
    return dynBuf->storedBytes;
}


/**
 * Return the number of bytes allocated in a Dynamic Buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return the allocated length of a Dynamic Buffer, or 0 if an error occured
 */
mcsUINT32     miscDynBufGetAllocatedBytesNumber(miscDYN_BUF       *dynBuf)
{
    char *functionName = "miscDynBufGetAllocatedBytesNumber\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return 0;
    }

    /* Return the Dynamic Buffer allocated bytes number */
    return dynBuf->allocatedBytes;
}


/**
 * Return a pointer to a Dynamic Buffer internal bytes buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \return a pointer to a Dynamic Buffer buffer, or NULL if an error occured
 */
char*         miscDynBufGetBufferPointer    (miscDYN_BUF       *dynBuf)
{
    char *functionName = "miscDynBufGetBufferPointer\0";

    /* Try to initialize the received Dynamic Buffer if it is not */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, functionName);
        return NULL;
    }

    /* Return the Dynamic Buffer buffer address */
    return dynBuf->dynBuf;
}


/**
 * Give back a Dynamic Buffer byte stored at a given position.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param byte the address of to the byte that will hold the Dynamic Buffer
 * seeked byte
 * \param position the position of the Dynamic Buffer seeked byte
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufGetByteAt           (miscDYN_BUF       *dynBuf,
                                             char              *byte,
                                             const mcsUINT32   position)
{
    char *functionName = "miscDynBufGetByteAt\0";

    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufVerifyPositionParameterValidity(dynBuf, position) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Test the 'write to' byte buffer address parameter validity */
    if (byte == NULL)
    {
        errAdd(miscERR_DYN_BUF_NULL_VALUE, functionName);
        return FAILURE;
    }

    /* Write back the seeked character inside the byte buffer parameter */
    *byte = dynBuf->dynBuf[position - miscDYN_BUF_BEGINNING_POSITION];

    return SUCCESS;
}


/**
 * Give back a part of a Dynamic Buffer in an already allocated extern buffer.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param bytes the address of the receiving, already allocated extern buffer
 * \param from the first Dynamic Buffer byte to be copied in the extern buffer
 * \param to the last Dynamic Buffer byte to be copied in the extern buffer
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufGetBytesFromTo      (miscDYN_BUF       *dynBuf,
                                             char              **bytes,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    char *functionName = "miscDynBufGetBytesFromTo\0";

    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufVerifyFromToParametersValidity(dynBuf, from, to) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Test the 'bytes' parameter validity */
    if (bytes == NULL)
    {
        errAdd(miscERR_DYN_BUF_NULL_VALUE, functionName, "bytes");
        return FAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be copied */
    int lengthToCopy = (to - from) + 1;

    /* Compute the first 'to be read' Dynamic Buffer byte position */
    char *positionToReadFrom = dynBuf->dynBuf
                               + (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Try to copy the Dynamic Buffer desired part in the extern buffer */
    if (memcpy(*bytes, positionToReadFrom, lengthToCopy) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * Overwrite a Dynamic Buffer byte at a given position.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param byte the byte to be written in the Dynamic Buffer
 * \param position the position of the Dynamic Buffer byte to be overwritten
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufReplaceByteAt       (miscDYN_BUF       *dynBuf,
                                             char              byte,
                                             const mcsUINT32   position)
{
    char *functionName = "miscDynBufReplaceByteAt\0";

    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufVerifyPositionParameterValidity(dynBuf, position) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Overwrite the specified Dynamic Buffer byte with the received one */
    dynBuf->dynBuf[position - 1] = byte;

    return SUCCESS;
}


/**
 * Replace a given range of Dynamic Buffer bytes by extern buffer bytes.
 *
 * The Dynamic Buffer replaced bytes will bve overwritten. Their range can be
 * smaller or bigger than the extern buffer bytes number.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param bytes the address of the extern buffer bytes to be written in
 * \param length the number of extern buffer bytes to be written in
 * \param from the position of the first Dynamic Buffer byte to be substituted
 * \param to the position of the last Dynamic Buffer byte to be substituted
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufReplaceBytesFromTo  (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    char *functionName = "miscDynBufReplaceBytesFromTo\0";

    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufVerifyFromToParametersValidity(dynBuf, from, to) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufVerifyBytesAndLengthParametersValidity(bytes, length)
        == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Compute the number of bytes by which the Dynamic Buffer should be
     * expanded
     */
    mcsINT32 bytesToAlloc = length
                            - (((to - miscDYN_BUF_BEGINNING_POSITION)
                                - (from - miscDYN_BUF_BEGINNING_POSITION)) + 1);

    /* If the Dynamic Buffer needs to be expanded... */
    if (bytesToAlloc > 0)
    {
        /* Try to allocate the desired bytes number in the Dynamic Buffer */
        if (miscDynBufAlloc(dynBuf, bytesToAlloc) == FAILURE)
        {
            errAdd(miscERR_DYN_BUF_AUTO_ALLOC, functionName);
            return FAILURE;
        }
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    int lengthToBackup = dynBuf->storedBytes
                         - ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be backep up' Dynamic Buffer byte position */
    char *positionToBackup = dynBuf->dynBuf
                             + ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be overwritten' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf
                              + (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Try to move the 'not-to-be-overwritten' Dynamic Buffer bytes to their
     * definitive place
     */
    if (memmove(positionToWriteIn + length, positionToBackup, lengthToBackup)
        == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Try to copy the extern buffer bytes in the Dynamic Buffer */
    if (memcpy(positionToWriteIn, bytes, length) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Update the Dynamic Buffer stored length value using the computed
     * signed 'bytesToAlloc' value
     */
    dynBuf->storedBytes += bytesToAlloc;

    return SUCCESS;
}


/**
 * Copy extern buffer bytes at the end of a Dynamic Buffer.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param bytes the address of the extern buffer bytes to be written in
 * \param length the number of extern buffer bytes to be written in
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length)
{
    char *functionName = "miscDynBufAppendBytes\0";

    /* Try to expand the received Dynamic Buffer size */
    if (miscDynBufAlloc(dynBuf, length) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_ALLOC, functionName);
        return FAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufVerifyBytesAndLengthParametersValidity(bytes, length)
        == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Try to copy the extern buffer bytes at the end of the Dynamic Buffer */
    if (memcpy(dynBuf->dynBuf + dynBuf->storedBytes, bytes, length) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Update the Dynamic Buffer stored length value using the number of the
     * extern buffer bytes
     */
    dynBuf->storedBytes += length;

    return SUCCESS;
}


/**
 * Insert extern buffer bytes in a Dynamic Buffer at a given position.
 *
 * The Dynamic Buffer bytes are not overwritten, but shiffted to the right.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param bytes a pointer to the extern buffer bytes to be inserted
 * \param length the number of extern buffer bytes to be inserted
 * \param position the position of the first Dynamic Buffer byte to write at
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   position)
{
    char *functionName = "miscDynBufInsertBytesAt\0";

    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufVerifyPositionParameterValidity(dynBuf, position) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufVerifyBytesAndLengthParametersValidity(bytes, length)
        == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Try to expand the received Dynamic Buffer size */
    if (miscDynBufAlloc(dynBuf, length) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_ALLOC, functionName);
        return FAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    int lengthToBackup = dynBuf->storedBytes
                         - (position - miscDYN_BUF_BEGINNING_POSITION);

    /* Compute the first 'to be overwritten' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf
                              + (position - miscDYN_BUF_BEGINNING_POSITION);

    /* Try to move the 'not-to-be-overwritten' Dynamic Buffer bytes to their
     * definitive place
     */
    if (memmove(positionToWriteIn + length, positionToWriteIn, lengthToBackup)
        == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Try to copy the extern buffer bytes in the Dynamic Buffer */
    if (memcpy(positionToWriteIn, bytes, length) == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Update the Dynamic Buffer stored length value using the extern buffer
     * bytes number
     */
    dynBuf->storedBytes += length;

    return SUCCESS;
}


/**
 * Delete a given range of Dynamic Buffer bytes.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param from the position of the first Dynamic Buffer byte to be deleted
 * \param to the position of the last Dynamic Buffer byte to be deleted
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    char *functionName = "miscDynBufDeleteBytesFromTo\0";

    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufVerifyFromToParametersValidity(dynBuf, from, to) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_BAD_PARAMETERS, functionName);
        return FAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    int lengthToBackup = dynBuf->storedBytes
                         - ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be backep up' Dynamic Buffer byte position */
    char *positionToBackup = dynBuf->dynBuf
                             + ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be deleted' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf
                              + (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Try to move the 'not-to-be-deleted' Dynamic Buffer bytes to their
     * definitive place
     */
    if (memmove(positionToWriteIn, positionToBackup, lengthToBackup)
        == NULL)
    {
        errAdd(miscERR_DYN_BUF_MEM_FAILURE, functionName);
        return FAILURE;
    }

    /* Update the Dynamic Buffer stored length value using the deleted bytes
     * number
     */
    dynBuf->storedBytes -= ((to - miscDYN_BUF_BEGINNING_POSITION)
                             - (from - miscDYN_BUF_BEGINNING_POSITION)) + 1;

    return SUCCESS;
}


/*___oOo___*/

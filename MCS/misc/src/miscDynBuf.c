/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: miscDynBuf.c,v 1.1 2004-07-08 12:06:46 lafrasse Exp $"
 *
 * who       when         what
 * --------  -----------  ------------------------------------------------------
 * lafrasse  05-Jul-2004  Created
 *
 *
 *******************************************************************************/

static char *rcsId="@(#) $Id: miscDynBuf.c,v 1.1 2004-07-08 12:06:46 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
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


/**
 * Verify if a Dynamic Buffer has allredy been initialized, else initialize it.
 *
 * This function is used internally in order to test if a dynamic buffer has
 * been initialized before use. If not, the Dynamic Buffer will be initialized.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return an execution status code
 */
static        mcsCOMPL_STAT miscDynBufInit  (miscDYN_BUF  *dynBuf)
{
    /* If the dynamic buffer is not allocated... */
    if (dynBuf == NULL)
    {
        errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufInit",
               "Received a Null Pointer instead of a Valid Pointer");
        return FAILURE;
    }
    else
    {
        /* If its 'pointer to itself' or its 'structure identifier magic number'
           are not correct... */
        if (dynBuf->thisPointer != dynBuf ||
            dynBuf->magicStructureId != miscDYN_BUF_MAGIC_STRUCTURE_ID)
        {
            /* Try to initialize all the structure with '0' */
            if ((memset((char *)dynBuf, 0, sizeof(miscDYN_BUF))) == NULL)
            {
                errAdd(miscERR_ALLOC_MEM);
                return FAILURE;
            }
            else
            {
                /* Set its 'pointer to itself' correctly */
                dynBuf->thisPointer = dynBuf;
                /* Set its 'structure identifier magic number' correctly */
                dynBuf->magicStructureId = miscDYN_BUF_MAGIC_STRUCTURE_ID;
            }
        }
        return SUCCESS;
    }
}


/**
 * Allocate a number of bytes for a Dynamic Buffer, plus those allready
 * allocated.
 *
 * The call to this functiun is optional, as the Dynamic Buffer will expand
 * itself on demand. It is usefull only when you know by advance the maximum
 * length of the buffer accros all its futur life, and thus want to minimize the 
 * CPU time used to expand the allocated memory.
 *
 * New memory allocated will contain '0'.
 *
 * If the buffer is already allocated, its length is expanded by the desired
 * one.
 * If the buffer is enlarged, the content will remain untouched after the
 * reallocation.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param length the length by which the buffer should be expanded
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufAlloc               (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   length)
{
    char *newBuf;

    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufAlloc");
        return FAILURE;
    }
    else
    {
        /* If the current buffer allready has the desired length... */
        if (length == 0)
        {
            /* Do nothing */
            return SUCCESS;
        }
        else
        {
            /* If the buffer has no memory allocated... */
            if (dynBuf->allocatedBytes == 0)
            {
                /* Try to allocate the desired length */
                if ((dynBuf->dynBuf = calloc(length, sizeof(char))) == NULL)
                {
                    errAdd(miscERR_ALLOC_MEM);
                    return FAILURE;
                }
        
                /* Try to Write '0' on all the newly allocated memory */
                if (memset(dynBuf->dynBuf, 0, length) == NULL)
                {
                    errAdd(miscERR_ALLOC_MEM);
                    return FAILURE;
                }
            }
            else 
            {
                /* Try to get more memory */
                if ((newBuf = realloc(dynBuf->dynBuf,
                     (dynBuf->storedBytes + length))) == NULL)
                {
                    errAdd(miscERR_ALLOC_MEM);
                    return FAILURE;
                }
        
                /* Store the buffer address */
                dynBuf->dynBuf = newBuf;
        
                /* If the buffer was containig nothing... */
                if (dynBuf->storedBytes == 0)
                {
                    /* Try to write '0' on all the newly allocated memory */
                    if ((memset(dynBuf->dynBuf, 0, dynBuf->allocatedBytes))
                         == NULL)
                    {
                        errAdd(miscERR_ALLOC_MEM);
                        return FAILURE;
                    }
                }
            }
        }
    
        /* Store the new buffer allocated length */
        dynBuf->allocatedBytes += length;
    
        return SUCCESS;
    }
}

/**
 * Dealloc the unused memory of a Dynamic Buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufStrip               (miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufStrip");
        return FAILURE;
    }
    else
    {
        /* Try to give back the unused memory */
        char *newBuf = realloc(dynBuf->dynBuf, dynBuf->storedBytes);

        /* If the memory operation has gone wrong */
        if (newBuf == NULL)
        {
            errAdd(miscERR_ALLOC_MEM);
            return FAILURE;
        }
        else
        {
            /* Store the buffer address */
            dynBuf->dynBuf = newBuf;
            dynBuf->allocatedBytes = dynBuf->storedBytes;
            return SUCCESS;
        }
    }
}

/**
 * Reset a Dynamic Buffer.
 *
 * Possibly allocated memory remains untouched.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufReset               (miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufReset");
        return FAILURE;
    }
    else
    {
        /* Act as if there were nothing in the buffer */
        dynBuf->storedBytes = 0;
        return SUCCESS;
    }
}

/**
 * Destroy a Dynamic Buffer.
 *
 * Possibly allocated memory is freed and zerod - so be sure that it is
 * desirable to delete the data contained in the buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufDestroy             (miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "misctDynBufDestroy");
        return FAILURE;
    }
    else
    {
        /* If some memory was allocated... */
        if (dynBuf->allocatedBytes != 0)
        {
            /* Free the allocated memory */
            free(dynBuf->dynBuf);
        }

        /* Try to initialize all the structure with '0' */
        if ((memset((char *)dynBuf, 0, sizeof(miscDYN_BUF))) == NULL)
        {
            errAdd(miscERR_ALLOC_MEM);
            return FAILURE;
        }
        else
        {
            return SUCCESS;
        }
    }
}

/**
 * Return the length of a Dynamic Buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return the length of the Dynamic Buffer structure buffer, or 0 if an error
 * occured
 */
mcsUINT32     miscDynBufGetStoredBytesNumber(miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufGetStoredBytesNumber");
        return 0;
    }
    else
    {
        return dynBuf->storedBytes;
    }
}

/**
 * Return the number of bytes allocated in a Dynamic Buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return the length of the Dynamic Buffer structure buffer, or 0 if an error
 * occured
 */
mcsUINT32     miscDynBufGetAllocatedBytesNumber(miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufGetAllocatedBytesNumber");
        return 0;
    }
    else
    {
        return dynBuf->allocatedBytes;
    }
}

/**
 * Return a pointer to the actual (character) buffer of a Dynamic Buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \return the length of the Dynamic Buffer structure buffer, or NULL if an
 * error occured
 */
char*         miscDynBufGetBufferPointer    (miscDYN_BUF       *dynBuf)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufGetBufferPointer");
        return NULL;
    }
    else
    {
        return dynBuf->dynBuf;
    }
}

/**
 * Return a specific character stored in a Dynamic Buffer at a given position.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param byte a pointer to a char to return the seeked character
 * \param position the position of the seeked character in the Dynamic Buffer
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufGetByteAt           (miscDYN_BUF       *dynBuf,
                                             char              *byte,
                                             const mcsUINT32   position)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufGetByteAt");
        return FAILURE;
    }
    else
    {
        /* If the given position is outside the buffer*/
        if (position < miscDYN_BUF_BEGINNING_POSTION
            || position > dynBuf->storedBytes)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufGetByteAt",
                   "Received Position Outside the Buffer");
            return FAILURE;
        }
        else
        {
            /* If the receiving buffer seems not allocated */
            if(byte == NULL)
            {
                errAdd(miscERR_DYN_BUF_ACTION,
                       "miscDynBufGetByteAt",
                       "Received a Null Pointer instead of a Valid Pointer");
                return FAILURE;
            }
            else
            {
                *byte = *(dynBuf->dynBuf +
                          (position - miscDYN_BUF_BEGINNING_POSTION ));
                return SUCCESS;
            }
        }
    }
}


/**
 * Copy a part of a Dynamic Buffer in an allready allocated buffer.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param bytes the address of the receiving and allready allocated char buffer
 * \param from the first character to copy
 * \param to the last character to copy
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufGetBytesFromTo      (miscDYN_BUF       *dynBuf,
                                             char              **bytes,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufGetBytesFromTo");
        return FAILURE;
    }
    else
    {
        /* If the FROM position is outside the buffer */
        if (from < miscDYN_BUF_BEGINNING_POSTION || from > dynBuf->storedBytes)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufGetBytesFromTo",
                   "Received FROM Position Outside the Buffer");
            return FAILURE;
        }
        else
        {
            /* If the TO position is outside the buffer */
            if (to < miscDYN_BUF_BEGINNING_POSTION || to > dynBuf->storedBytes)
            {
                errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufGetBytesFromTo",
                       "Received TO Position Outside the Buffer");
                return FAILURE;
            }
            else
            {
                /* If the positions are wrong */
                if (to <= from)
                {
                    errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufGetBytesFromTo",
                           "Received Wrong Positions (TO <= FROM)");
                    return FAILURE;
                }
                else
                {
                    /* If the receiving buffer seems not allocated */
                    if(bytes == NULL)
                    {
                        errAdd(miscERR_DYN_BUF_ACTION,
                               "miscDynBufGetBytesFromTo",
                               "Received a Null Pointer instead of a Valid Pointer");
                        return FAILURE;
                    }
                    else
                    {
                        /* Try to copy the memory */
                        if (memcpy(*bytes, (dynBuf->dynBuf + (from -
                            miscDYN_BUF_BEGINNING_POSTION)),
                            (to - from) + 1) == NULL)
                        {
                            errAdd(miscERR_ALLOC_MEM);
                            return FAILURE;
                        }
                        else
                        {
                            return SUCCESS;
                        }
                    }
                }
            }
        }
    }
}


/**
 * Overwrite a specific character in a Dynamic Buffer at a given position.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param byte the character to store
 * \param position the position of the character to be overwritten
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufReplaceByteAt       (miscDYN_BUF       *dynBuf,
                                             char              byte,
                                             const mcsUINT32   position)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufSetByte");
        return FAILURE;
    }
    else
    {
        /* If the given position is outside the buffer*/
        if (position < miscDYN_BUF_BEGINNING_POSTION
            || position > dynBuf->storedBytes)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufSetByte",
                   "Received Position Outside the Buffer");
            return FAILURE;
        }
        else
        {
            dynBuf->dynBuf[position - 1] = byte;
            return SUCCESS;
        }
    }
}


/**
 * Replace a given range of Dynamic Buffer bytes by a buffer contents.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param bytes a pointer to the char buffer to insert
 * \param length the length of the char buffer to be copied
 * \param from the position of the first character to be substituted
 * \param to the position of the last character to be substituted
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufReplaceBytesFromTo  (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufReplaceBytesFromTo");
        return FAILURE;
    }
    else
    {
        /* If the FROM position is outside the buffer */
        if (from < miscDYN_BUF_BEGINNING_POSTION || from > dynBuf->storedBytes)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufReplaceBytesFromTo",
                   "Received FROM Position Outside the Buffer");
            return FAILURE;
        }
        else
        {
            /* If the TO position is outside the buffer */
            if (to < miscDYN_BUF_BEGINNING_POSTION || to > dynBuf->storedBytes)
            {
                errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufReplaceBytesFromTo",
                       "Received TO Position Outside the Buffer");
                return FAILURE;
            }
            else
            {
                /* If the positions are wrong */
                if (to <= from)
                {
                    errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufReplaceBytesFromTo",
                           "Received Wrong Positions (TO <= FROM)");
                    return FAILURE;
                }
                else
                {
                    /* If the length is null */
                    if (length <= 0)
                    {
                        errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufReplaceBytesFromTo",
                               "Received a Null Length");
                        return FAILURE;
                    }
                    else
                    {
                        mcsINT32 bytesToAlloc = length - (to - from +
                                                miscDYN_BUF_BEGINNING_POSTION);
                        if (bytesToAlloc > 0)
                        {
                            /* If the received dynamic buffer could not be allocated... */
                            if (miscDynBufAlloc(dynBuf, bytesToAlloc) == FAILURE)
                            {
                                errAdd(miscERR_DYN_BUF_ACTION,
                                       "miscDynBufReplaceBytesFromTo",
                                       "Could not Allocate the Received Dynamic Buffer");
                                return FAILURE;
                            }
                        }
        
                        int lengthToBackup = dynBuf->storedBytes - to;
                        char *positionToBackup = dynBuf->dynBuf + to;
                        char *positionToWriteIn = dynBuf->dynBuf + (from - miscDYN_BUF_BEGINNING_POSTION);
                
                        /* Try to allocate the desired length */
                        char *tmpBuf = calloc(lengthToBackup, sizeof(char));
                        if (tmpBuf == NULL)
                        {
                            errAdd(miscERR_ALLOC_MEM);
                            return FAILURE;
                        }
                        else
                        {
                            /* Try to copy the memory */
                            if (memcpy(tmpBuf, positionToBackup, lengthToBackup)
                                == NULL)
                            {
                                errAdd(miscERR_ALLOC_MEM);
                                return FAILURE;
                            }
                            else
                            {
                                /* Try to copy the memory */
                                if (memcpy(positionToWriteIn, bytes, length) == NULL)
                                {
                                    errAdd(miscERR_ALLOC_MEM);
                                    return FAILURE;
                                }
                                else
                                {
                                    /* Try to copy the memory */
                                    if (memcpy(dynBuf->dynBuf + length, tmpBuf,
                                               lengthToBackup) == NULL)
                                    {
                                        errAdd(miscERR_ALLOC_MEM);
                                        return FAILURE;
                                    }
                                    else
                                    {
                                        dynBuf->storedBytes += bytesToAlloc;
                                        return SUCCESS;
                                    }
                                }
                            }
                        }
                   }
                }
            }
        }
    }
}


/**
 * Copy a buffer at the end of a Dynamic Buffer.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param bytes a pointer to the char buffer to copy
 * \param length the length of the char buffer to be copied
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufAppendBytes         (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length)
{
    /* If the received dynamic buffer could not be allocated... */
    if (miscDynBufAlloc(dynBuf, length) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufAppendBytes",
               "Could not Allocate the Received Dynamic Buffer");
        return FAILURE;
    }
    else
    {
        /* If the received buffer seems not allocated */
        if(bytes == NULL)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufAppendBytes",
                   "Received a Null Pointer instead of a Valid Pointer");
            return FAILURE;
        }
        else
        {
            /* Try to copy the memory */
            if (memcpy(dynBuf->dynBuf + dynBuf->storedBytes, bytes, length)
                == NULL)
            {
                errAdd(miscERR_ALLOC_MEM);
                return FAILURE;
            }
            else
            {
                dynBuf->storedBytes += length;
                return SUCCESS;
            }
        }
    }
}


/**
 * Insert a buffer at a given position of a Dynamic Buffer.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param bytes a pointer to the char buffer to insert
 * \param length the length of the char buffer to be inserted
 * \param position the position of the first character to write at
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufInsertBytesAt       (miscDYN_BUF       *dynBuf,
                                             char              *bytes,
                                             const mcsUINT32   length,
                                             const mcsUINT32   position)
{
    /* If the given position is outside the buffer*/
    if (position < miscDYN_BUF_BEGINNING_POSTION
        || position > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufInsertBytesAt",
               "Received Position Outside the Buffer");
        return FAILURE;
    }
    else
    {
        /* If the received dynamic buffer could not be allocated... */
        if (miscDynBufAlloc(dynBuf, length) == FAILURE)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufInsertBytesAt",
                   "Could not Allocate the Received Dynamic Buffer");
            return FAILURE;
        }
        else
        {
            /* If the receiving buffer seems not allocated */
            if(bytes == NULL)
            {
                errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufInsertBytesAt",
                       "Received a Null Pointer instead of a Valid Pointer");
                return FAILURE;
            }
            else
            {
                int lengthToBackup = dynBuf->storedBytes - (position -
                                     miscDYN_BUF_BEGINNING_POSTION);
                char *positionToWriteIn = dynBuf->dynBuf + (position -
                                          miscDYN_BUF_BEGINNING_POSTION);
        
                /* Try to allocate the desired length */
                char *tmpBuf = calloc(lengthToBackup, sizeof(char));
                if (tmpBuf == NULL)
                {
                    errAdd(miscERR_ALLOC_MEM);
                    return FAILURE;
                }
                else
                {
                    /* Try to copy the memory */
                    if (memcpy(tmpBuf, positionToWriteIn, lengthToBackup)
                        == NULL)
                    {
                        errAdd(miscERR_ALLOC_MEM);
                        return FAILURE;
                    }
                    else
                    {
                        /* Try to copy the memory */
                        if (memcpy(positionToWriteIn, bytes, length) == NULL)
                        {
                            errAdd(miscERR_ALLOC_MEM);
                            return FAILURE;
                        }
                        else
                        {
                            /* Try to copy the memory */
                            if (memcpy(dynBuf->dynBuf + (position -
                                miscDYN_BUF_BEGINNING_POSTION) + length,
                                tmpBuf, lengthToBackup) == NULL)
                            {
                                errAdd(miscERR_ALLOC_MEM);
                                return FAILURE;
                            }
                            else
                            {
                                dynBuf->storedBytes += length;
                                return SUCCESS;
                            }
                        }
                    }
                }
            }
        }
    }
}


/**
 * Delete a given range of Dynamic Buffer.
 *
 * First character has position 1.
 *
 * \param dynBuf a pointer to a Dynamic Buffer structure
 * \param from the position of the first character to be deleted
 * \param to the position of the last character to be deleted
 * \return an execution status code
 */
mcsCOMPL_STAT miscDynBufDeleteBytesFromTo   (miscDYN_BUF       *dynBuf,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to)
{
    /* If the received dynamic buffer has not allredy been initialized... */
    if (miscDynBufInit(dynBuf) == FAILURE)
    {
        errAdd(miscERR_DYN_BUF_AUTO_INIT, "miscDynBufDeleteBytesFromTo");
        return FAILURE;
    }
    else
    {
        /* If the FROM position is outside the buffer */
        if (from < miscDYN_BUF_BEGINNING_POSTION || from > dynBuf->storedBytes)
        {
            errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufDeleteBytesFromTo",
                   "Received FROM Position Outside the Buffer");
            return FAILURE;
        }
        else
        {
            /* If the TO position is outside the buffer */
            if (to < miscDYN_BUF_BEGINNING_POSTION || to > dynBuf->storedBytes)
            {
                errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufDeleteBytesFromTo",
                       "Received TO Position Outside the Buffer");
                return FAILURE;
            }
            else
            {
                /* If the positions are wrong */
                if (to <= from)
                {
                    errAdd(miscERR_DYN_BUF_ACTION, "miscDynBufDeleteBytesFromTo",
                           "Received Wrong Positions (TO <= FROM)");
                    return FAILURE;
                }
                else
                {
                    int lengthToBackup = dynBuf->storedBytes - to;
                    char *positionToBackup = dynBuf->dynBuf + to;
                    char *positionToWriteIn = dynBuf->dynBuf + (from - miscDYN_BUF_BEGINNING_POSTION);
            
                    /* Try to allocate the desired length */
                    char *tmpBuf = calloc(lengthToBackup, sizeof(char));
                    if (tmpBuf == NULL)
                    {
                        errAdd(miscERR_ALLOC_MEM);
                        return FAILURE;
                    }
                    else
                    {
                        /* Try to copy the memory */
                        if (memcpy(tmpBuf, positionToBackup, lengthToBackup)
                            == NULL)
                        {
                            errAdd(miscERR_ALLOC_MEM);
                            return FAILURE;
                        }
                        else
                        {
                            /* Try to copy the memory */
                            if (memcpy(positionToWriteIn, tmpBuf,
                                       lengthToBackup) == NULL)
                            {
                                errAdd(miscERR_ALLOC_MEM);
                                return FAILURE;
                            }
                            else
                            {
                                dynBuf->storedBytes -= to - (from - miscDYN_BUF_BEGINNING_POSTION);
                                return SUCCESS;
                            }
                        }
                    }
               }
            }
        }
    }
}


/*___oOo___*/

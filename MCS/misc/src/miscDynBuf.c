/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: miscDynBuf.c,v 1.36 2005-03-03 16:10:31 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.35  2005/02/22 11:11:38  lafrasse
 * Added miscDynBufGetNextCommentLine(), miscDynBufAppendLine() and miscDynBufAppendComentLine()
 *
 * Revision 1.34  2005/02/17 14:32:42  gzins
 * Improved intialisation of static dynamic buffers
 *
 * Revision 1.33  2005/02/16 15:54:33  gzins
 * Fixed bug in miscDynBufGetNextLine; wrong test to prevent array overflow
 *
 * Revision 1.32  2005/02/16 14:54:43  gzins
 * Minor change in documentation
 *
 * Revision 1.31  2005/02/16 14:39:55  gzins
 * Updated miscDynBufGetNextLine() function to do not alter buffer content.
 *
 * Revision 1.30  2005/02/11 08:46:26  gzins
 * Fixed minor bug when testing if file to be opened is or not a regular file
 *
 * Revision 1.29  2005/02/10 23:13:04  lafrasse
 * Corrected a 'segmentation fault' bug when using miscDynBufGetNextLine() on an
 * empty Dynamic Buffer
 *
 * Revision 1.28  2005/02/10 22:59:23  lafrasse
 * Corrected a copy-paste typo in miscDynBufGetNextLine()
 *
 * Revision 1.27  2005/02/10 10:08:07  lafrasse
 * Added miscDynBufSaveInFile(), and moved as most miscDynBuf parameters as possible to const type
 *
 * Revision 1.26  2005/02/03 15:45:43  gzins
 * Updated miscDynBufVerifyIsInitialized to suppress all errResetStack calls
 *
 * Revision 1.25  2005/02/03 08:59:31  gzins
 * Defined 'bytes' parameter as constant in miscDynBufAppendBytes
 * Defined 'str' parameter as constant in miscDynBufAppendString
 *
 * Revision 1.24  2005/01/28 18:35:10  gzins
 * Added automatic initialisation of structure when needed
 *
 * Revision 1.23  2005/01/28 18:10:30  gzins
 * Renamed miscDynBufGetBufferPointer to miscDynBufGetBuffer
 * Renamed miscDynBufGetNextLinePointer to miscDynBufGetNextLine
 *
 * Revision 1.22  2005/01/28 17:56:46  gzins
 * Declared dynBuf parameter of miscDynBufGetBufferPointer as const
 * Removed automatic initialisation of dynamic buffer structure
 *
 * lafrasse  05-Jul-2004  Created
 * lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
 * lafrasse  12-Jul-2004  Code factorization and error codes polishing
 * lafrasse  19-Jul-2004  Corrected some bugs ('from = to' parameters)
 * lafrasse  20-Jul-2004  Used 'memmove()' instead of temporary buffers
 * lafrasse  22-Jul-2004  Removed all '\0' from char arrays
 *                        Corrected a bug in miscDynBufAlloc that could cause a
 *                        Segmentation fault when bytes were already allocated
 * lafrasse  23-Jul-2004  Added error management to
 *                        miscDynBufGetNbStoredBytes and
 *                        miscDynBufGetNbAllocatedBytes, plus
 *                        miscDynBufGetBytesFromTo parameter refinments and
 *                        error code factorization
 * lafrasse  02-Aug-2004  Moved mcs.h include to miscDynBuf.h
 *                        Moved in null-terminated string specific functions
 *                        from miscDynStr.c
 * lafrasse  23-Aug-2004  Moved miscDynBufInit from local to public
 * lafrasse  30-Sep-2004  Added MEM_DEALLOC_mcsFAILURE error management, and
 *                        corrected to memory allocation bug in miscDynBufAlloc,
 *                        miscDynBufStrip, miscDynBufReplaceBytesFromTo and
 *                        miscDynBufAppenBytes
 * lafrasse  08-Nov-2004  Added miscDynBufGetNextLinePointer() and
 *                        miscDynBufLoadFile() function, plus the code to
 *                        correctly initialize the new commentPattern field in
 *                        miscDynBufInit() and miscDynBufGetCommentPattern() and
 *                        miscDynBufSetCommentPattern() to deal with this field
 * gzins     16-Nov-2004  Added miscDynBufVerifyIsInitialized() and update
 *                        miscDynBufInit() to inconditionaly initialize
 *                        the dynamic buffer.
 * lafrasse  03-Dec-2004  Added error management code to miscDynBufLoadFile()
 * gzins     07-Dec-2004  Closed open file in miscDynBufLoadFile
 * gzins     14-Dec-2004  Renamed miscERR_MEM_mcsFAILURE to miscERR_ALLOC
 * scetre    21-Dec-2004  Added '\0' at the end of buffer in miscDynBufLoadFile
 * gzins     21-Dec-2004  Renamed miscDynBufGetStoredBytesNumber to
 *                        miscDynBufGetNbStoredBytes and
 *                        miscDynBufGetAllocatedBytesNumber to
 *                        miscDynBufGetNbAllocatedBytes
 *
 ******************************************************************************/

/**
 * \file
 * Definition of miscDynBuf functions.
 * 
 * All the algorithms behind Dynamic Buffer management are grouped in this file.
 * Except for string specific functions where handled buffer is null terminated,
 * other buffer are raw buffer and can handle all kind of data, such as numbers,
 * images...
 *
 * \warning A Dynamic Buffer <b>should be initialised and destroyed</b>, before
 * and after use respectively through miscDynBufInit() and miscDynBufDestroy()
 * functions.
 *
 * \n \b Code \b Example:\n
 * \n A simple main using a Dynamic Buffer.
 * \code
 * #include "miscDynBuf.h"
 *
 * int main (int argc, char *argv[])
 * {
 *     miscDYN_BUF dynBuf;
 *     char tab1[3] = {0, 1, 2};
 *
 *     /# Initialise the Dynamic Buffer, MANDATORY before Dynamic Buffer use #/
 *     miscDynBufInit(&dynBuf);
 *
 *     /# Add bytes at the end of the Dynamic Buffer #/
 *     miscDynBufAppendBytes(&dynBuf, (char*)tab1, 3 * sizeof(int));
 *     .
 *     . ...
 *     .
 *     char tab2[7] = {3, 4, 5, 6, 7, 8, 9};
 *     miscDynBufAppendBytes(&dynBuf, (char*)tab2, 7 * sizeof(int));
 *     .
 *     . ...
 *     .
 *     /# Reset a Dynamic Buffer #/
 *     miscDynBufReset(&dynBuf);
 *
 *     /# Add a string at the end of the Dynamic Buffer #/
 *     char *tmp = "bytes to";
 *     miscDynBufAppendString(&dynBuf, tmp);
 *     tmp = " append...";
 *     miscDynBufAppendString(&dynBuf, tmp);
 *     printf("DynBuf contains '%s'.\n", miscDynBufGetBuffer(&dynBuf));
 *
 *     /# Destroy the Dynamic Buffer, MANDATORY after Dynamic Buffer use #/
 *     miscDynBufDestroy(&dynBuf);
 *
 *     exit (EXIT_mcsSUCCESS);
 * }
 * \endcode
 */

static char *rcsId="@(#) $Id: miscDynBuf.c,v 1.36 2005-03-03 16:10:31 gluck Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


/*
 * MCS Headers 
 */
#include "err.h"


/* 
 * Local Headers
 */
#include "miscDynBuf.h"
#include "miscString.h"
#include "miscPrivate.h"
#include "miscErrors.h"


/* 
 * Local functions declaration 
 */
static mcsLOGICAL miscDynBufIsInitialised(const miscDYN_BUF *dynBuf);

static mcsCOMPL_STAT miscDynBufChkPositionParam(const miscDYN_BUF *dynBuf,
                                                const mcsUINT32 position);

static mcsCOMPL_STAT miscDynBufChkFromToParams(const miscDYN_BUF *dynBuf,
                                               const mcsUINT32 from,
                                               const mcsUINT32 to);

static mcsCOMPL_STAT miscDynBufChkBytesAndLengthParams(const char *bytes,
                                                       const mcsUINT32 length);

static mcsUINT32 miscDynBufChkStringParam(const char *str);


/* 
 * Local functions definition
 */

/**
 * Verify if a Dynamic Buffer has already been initialized.
 *
 * This function is only used internally by funtions to check that the dynamic
 * buffer has been correctly initialized. If not, do it.
 *
 * \param dynBuf address of Dynamic Buffer structure
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsLOGICAL miscDynBufIsInitialised(const miscDYN_BUF *dynBuf)
{
    /* Test the 'dynBuf' pointer validity */
    if (dynBuf == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "dynBuf");
        return mcsFALSE;
    }

    /*
     * Test the 'pointer to itself' and 'structure identifier magic number'
     * validity...
     */
    if (dynBuf->thisPointer != dynBuf || 
        dynBuf->magicStructureId != miscDYN_BUF_MAGIC_STRUCTURE_ID)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'position' is correct (eg. inside the Dynamic Buffer range).
 *
 * This function is only used internally by funtions receiving 'position'
 * parameter.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param position a position inside the Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT miscDynBufChkPositionParam(const miscDYN_BUF *dynBuf,
                                                const mcsUINT32 position)
{
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return mcsFAILURE;
    }

    /* Test the position parameter validity... */
    if (position < miscDYN_BUF_BEGINNING_POSITION || 
        position > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, "position");
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'from' and 'to' position are correct (eg. inside the Dynamic Buffer range,
 * and 'from' parameterlower than 'to' parameter).
 *
 * This function is only used internally by funtions receiving 'from' and 'to'
 * parameters.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param from a position inside the Dynamic Buffer
 * \param to a position inside the Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT miscDynBufChkFromToParams(const miscDYN_BUF *dynBuf,
                                               const mcsUINT32 from,
                                               const mcsUINT32 to)
{
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return mcsFAILURE;
    }

    /* Test the 'from' parameter validity */
    if (from < miscDYN_BUF_BEGINNING_POSITION || from > dynBuf->storedBytes)
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, "from");
        return mcsFAILURE;
    }

    /* Test the 'to' parameter validity */
    if ((to < miscDYN_BUF_BEGINNING_POSITION) || (to > dynBuf->storedBytes))
    {
        errAdd(miscERR_DYN_BUF_BAD_POSITION, "to");
        return mcsFAILURE;
    }

    /* Test the 'from' and 'to' parameters validity against each other */
    if (to < from)
    {
        errAdd(miscERR_DYN_BUF_BAD_FROM_TO);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'bytes' pointer and 'length' size are correct (eg. bytes != 0 & length != 0).
 *
 * This function is only used internally by funtions receiving 'bytes' and
 * 'length' parameters.
 *
 * \param bytes bytes buffer pointer to test
 * \param length number of bytes in the buffer to test
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT miscDynBufChkBytesAndLengthParams(const char *bytes,
                                                       const mcsUINT32 length)
{
    /* Test the 'bytes' parameter validity */
    if (bytes == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "bytes");
        return mcsFAILURE;
    }

    /* Test the 'length' parameter validity */
    if (length == 0)
    {
        errAdd(miscERR_NULL_PARAM, "length");
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Verify if a received string (a null terminated char array) is valid or not.
 *
 * \param str address of the string, already allocated extern buffer
 *
 * \n
 * \return string length, or 0 if it is not valid
 */
static mcsUINT32 miscDynBufChkStringParam(const char *str)
{
    /* Test the 'str' parameter validity */
    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return 0;
    }

    /* Return the number of bytes stored in the received string including its
     * ending '\0'
     */
    return(strlen(str) + 1);
}


/*
 * Public functions definition
 */

/**
 * Initialize a Dynamic Buffer.
 *
 * \param dynBuf address of Dynamic Buffer structure
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufInit(miscDYN_BUF *dynBuf)
{
    /* Test the 'dynBuf' parameter validity... */
    if (dynBuf == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "dynBuf");
        return mcsFAILURE;
    }

    /* Initialize all the structure with '0' */
    memset(dynBuf, 0, sizeof(miscDYN_BUF));

    /* Set its 'pointer to itself' correctly */
    dynBuf->thisPointer = dynBuf;
    /* Set its 'structure identifier magic number' correctly */
    dynBuf->magicStructureId = miscDYN_BUF_MAGIC_STRUCTURE_ID;
    /* Set its 'comment pattern' to nothing */
    dynBuf->commentPattern[0] = '\0';

    return mcsSUCCESS;
}


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
 * \param dynBuf address of a Dynamic Buffer structure
 * \param length number of bytes by which the Dynamic Buffer should be expanded
 * (if length value is less than or equal to 0, nothing is done).
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \remark The call to this function is optional, as a Dynamic Buffer will
 * expand itself on demand when invoquing other miscDynBuf functions as
 * miscDynBufAppendBytes(), miscDynBufInsertBytesAt(), etc... So, this
 * function call is only usefull when you know by advance the maximum bytes
 * length the Dynamic Buffer can reach accross its entire life, and thus want
 * to minimize the CPU time spent to expand the Dynamic Buffer allocated
 * memory on demand.
 */
mcsCOMPL_STAT miscDynBufAlloc(miscDYN_BUF *dynBuf, const mcsINT32 length)
{
    char *newBuf = NULL;

    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /* If the current buffer already has sufficient length... */
    if (length <= 0)
    {
        /* Do nothing */
        return mcsSUCCESS;
    }

    /* If the buffer has no memory allocated... */
    if (dynBuf->allocatedBytes == 0)
    {
        /* Allocate the desired length */
        if ((dynBuf->dynBuf = calloc(length, sizeof(char))) == NULL)
        {
            errAdd(miscERR_ALLOC);
            return mcsFAILURE;
        }

        /* Write '0' on all the newly allocated memory */
        memset(dynBuf->dynBuf, 0, length);
    }
    else /* The buffer needs to be expanded */
    {
        /* Get more memory */
        if ((newBuf = realloc(dynBuf->dynBuf, 
                              dynBuf->allocatedBytes + length)) == NULL)
        {
            errAdd(miscERR_ALLOC);
            return mcsFAILURE;
        }

        /* Store the expanded buffer address */
        dynBuf->dynBuf = newBuf;

        /* If the buffer was containig nothing... */
        if (dynBuf->storedBytes == 0)
        {
            /* Write '0' on all the newly allocated memory */
            memset(dynBuf->dynBuf, 0, dynBuf->allocatedBytes);
        }
    }

    /* Increase the buffer allocated length value */
    dynBuf->allocatedBytes += length;

    return mcsSUCCESS;
}


/**
 * Deallocate the unused allocated memory of a Dynamic Buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufStrip(miscDYN_BUF *dynBuf)
{
    char *newBuf = NULL;

    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /* If the received Dynamic Buffer needs to be stripped */
    if (dynBuf->storedBytes < dynBuf->allocatedBytes)
    {
        /* If the Dynamic Buffer needs to be completly freed */
        if (dynBuf->storedBytes == 0)
        {
            /* De-allocate it */
            if (dynBuf->dynBuf == NULL)
            {
                errAdd(miscERR_NULL_PARAM, "dynBuf->dynBuf");
                return mcsFAILURE;
            }

            free(dynBuf->dynBuf);
        }
        else
        {
            /* Give back the unused memory */
            if ((newBuf = realloc(dynBuf->dynBuf, dynBuf->storedBytes)) == NULL)
            {
                errAdd(miscERR_ALLOC);
                return mcsFAILURE;
            }
        }

        /* Store the new buffer address */
        dynBuf->dynBuf = newBuf;
    
        /* Update the buffer allocated length value */
        dynBuf->allocatedBytes = dynBuf->storedBytes;
    }

    return mcsSUCCESS;
}


/**
 * Reset a Dynamic Buffer.
 *
 * Possibly allocated memory remains untouched, until the reseted Dynamic Buffer
 * is reused to store other bytes.
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufReset(miscDYN_BUF *dynBuf)
{
    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /*
     * Update the Dynamic Buffer stored bytes number value, to make it act as
     * if there were nothing in the buffer
     */
    dynBuf->storedBytes = 0;

    return mcsSUCCESS;
}


/**
 * Destroy a Dynamic Buffer.
 *
 * Possibly allocated memory is freed and zerod - so be sure that it is
 * desirable to delete the data contained inside the buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufDestroy(miscDYN_BUF *dynBuf)
{
    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /* If some memory was allocated... */
    if (dynBuf->allocatedBytes != 0)
    {
        /* Free the allocated memory */
        free(dynBuf->dynBuf);
    }

    /* Initialize all the structure with '0' */
    memset((char *)dynBuf, 0, sizeof(miscDYN_BUF));

    return mcsSUCCESS;
}


/**
 * Give back the number of bytes stored in a Dynamic Buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param storedBytes address of the variable which will contain number of
 * stored bytes by the Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufGetNbStoredBytes(const miscDYN_BUF *dynBuf,
                                         mcsUINT32 *storedBytes)
{
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return mcsFAILURE;
    }

    /* Return the Dynamic Buffer stored bytes number */
    *storedBytes = dynBuf->storedBytes;

    return mcsSUCCESS;
}


/**
 * Give back the number of bytes allocated in a Dynamic Buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param allocatedBytes address of the variable which will contain number of
 * allocated bytes by the Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufGetNbAllocatedBytes(const miscDYN_BUF *dynBuf,
                                            mcsUINT32 *allocatedBytes)
{
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return mcsFAILURE;
    }

    /* Return the Dynamic Buffer allocated bytes number */
    *allocatedBytes = dynBuf->allocatedBytes;

    return mcsSUCCESS;
}


/**
 * Return a pointer to a Dynamic Buffer internal bytes buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 *
 * \n
 * \return a pointer to a Dynamic Buffer buffer, or NULL if an error occured
 */
char* miscDynBufGetBuffer(const miscDYN_BUF *dynBuf)
{
    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return ((char*) NULL);
    }

    /* Return the Dynamic Buffer buffer address */
    return dynBuf->dynBuf;
}


/**
 * Return a pointer to a Dynamic Buffer internal comment pattern string.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 *
 * \n
 * \return a pointer to a Dynamic Buffer comment pattern, or NULL if an error
 * occured
 */
const char* miscDynBufGetCommentPattern(const miscDYN_BUF *dynBuf)
{
    /* Initialize the received Dynamic Buffer if it is not */
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return ((const char *) NULL);
    }

    /* Return the Dynamic Buffer buffer address */
    return dynBuf->commentPattern;
}


/**
 * Return the next line of a Dynamic Buffer.
 *
 * It retrieves the next line  from the given \em currentPos position in the
 * dynamic buffer and copies its content (untill \\t) into the \em nextLine
 * buffer. If there is no new line (no \\t), it copies until the end of the
 * dynamic buffer (\\0) or until the bytes read cannot be stored in the \em
 * nextLine buffer.\n
 *
 * If the given position is a NULL pointer, the search starts from the beginning
 * of the buffer. If \em skipCommentFlag is set to mcsTRUE, comment lines
 * (starting with pattern defined either when loading file with
 * miscDynBufLoadFile() or using miscDynBufSetCommentPattern()) are skipped.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param currentPos position from which the next line will be searched, or NULL
 * to begin on the first line of the buffer.
 * \param nextLine buffer in which the next line will be stored.
 * \param maxLineLength next line buffer maximun length.
 * \param skipCommentFlag boolean specifying weither the line beginning by the
 * Dynamic Buffer comment pattern should be skipped or not
 *
 * \n
 * \return a pointer to the new buffer position to be used to get the next line,
 * or NULL either if an error occured either or the end of the buffer has been
 * reached.
 *
 * \n
 * \ex
 * \code
 * miscDYN_BUF dynBuf;
 *
 * /# Load file, where comment lines start with hash sign #/
 * if (miscDynBufLoadFile(&dynBuf, "myFile", "#") == mcsFAILURE)
 * {
 *     /# Handle error #/
 *     return (mcsFAILURE);
 * }
 *
 * /# Get all (from the beginning) the lines, skipping the comment lines #/
 * char *currPtr = NULL;
 * mcsSTRING1024 line;
 * while ((currPtr = miscDynBufGetNextLine(&dynBuf, currPtr, line,
 *                                         sizeof(line), mcsTRUE) != 
 *                                         NULL))
 * {
 *     /# Print out line content #/
 *     printf("%s\n", line);
 * }
 * \endcode
 */
const char* miscDynBufGetNextLine (const miscDYN_BUF *dynBuf,
                                   const char *currentPos,
                                   char *nextLine,
                                   const mcsUINT32 maxLineLength,
                                   const mcsLOGICAL skipCommentFlag)
{
    /* Initialize the received Dynamic Buffer if it is not */
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        errAdd(miscERR_DYN_BUF_NOT_INITIALIZED);
        return ((char *)NULL);
    }

    /* Get the current Dynamic Buffer internal buffer pointer */
    char *bufferStart = miscDynBufGetBuffer(dynBuf);
    if (bufferStart == NULL)
    {
        errAdd(miscERR_DYN_BUF_IS_EMPTY);
        return ((char *)NULL);
    }

    /* Get the current Dynamic Buffer internal buffer length */
    mcsUINT32 length;
    if (miscDynBufGetNbStoredBytes(dynBuf, &length) == mcsFAILURE)
    {
        return ((char*)NULL);
    }
    char *bufferEnd = bufferStart + length;

    /* If buffer is empty */
    if (length == 0)
    {
        return ((char*)NULL);
    }

    /* If the given current Line Pointer is outside of the Dynamic Buffer */
    if ((currentPos != NULL) &&
        ((currentPos < bufferStart) || (currentPos > bufferEnd)))
    {
        return ((char*)NULL);
    }
    
    /* Gets the next '\n' occurence after currentPos */
    mcsLOGICAL nextLineFound = mcsFALSE;
    do 
    {
        /* Get next line in buffer */
        if (currentPos != NULL)
        {
            /* If there is no more line, return */
            currentPos = strchr(currentPos, '\n');
            if ((currentPos == NULL) || ((currentPos + 1) == bufferEnd))
            { 
                return ((char*)NULL);
            }

            /* Else skip CR character */
            currentPos = currentPos + 1;
        }
        else
        {
            /* Restart from beginning of buffer */
            currentPos = bufferStart;
        }            

        /* If it is a comment line and it should be skipped, skip it */
        if ((skipCommentFlag == mcsFALSE) ||
            (miscIsCommentLine(currentPos, miscDynBufGetCommentPattern(dynBuf))
            == mcsFALSE))
        {
            nextLineFound = mcsTRUE;
        }
    } while (nextLineFound == mcsFALSE);

    /* Copy next into buffer */
    mcsINT32 i = 0;
    memset(nextLine, '\0', maxLineLength); 
    while (((currentPos + i) < bufferEnd) &&
             (i < (maxLineLength - 1)) &&
             (currentPos[i] != '\n'))
    {
        /* Copy current character */
        nextLine[i] = currentPos[i];

        /* Go to the next one */
        i++;
    }

    return (currentPos);
}


/**
 * Return the next comment line of a Dynamic Buffer.
 *
 * This function works the same way as miscDynBufGetNextLine().
 *
 * \param dynBuf the address of a Dynamic Buffer structure
 * \param currentPos the  position from which the next comment line will be
 * searched, or NULL pointer to begin at the first line of the buffer.
 * \param nextLine buffer in which the next comment line will be stored.
 * \param maxLineLength next line buffer maximun length.
 *
 * \n
 * \return a pointer to the new buffer position used to get the comment next
 * line, or NULL either if an error occured or end of the buffer is reached.
 * 
 * \sa miscDynBufGetNextLine().
 */
const char*   miscDynBufGetNextCommentLine (const miscDYN_BUF *dynBuf,
                                            const char *currentPos,
                                            char *nextLine,
                                            const mcsUINT32 maxLineLength)
{
    /* Skip lines, until it reaches a comment one (or the end of the buffer) */
    do
    {
        currentPos = miscDynBufGetNextLine(dynBuf, currentPos, nextLine,
                                           maxLineLength, mcsFALSE);
    }
    while ((currentPos != NULL) &&
           (miscIsCommentLine(currentPos, miscDynBufGetCommentPattern(dynBuf))
           == mcsFALSE));

    return currentPos;
}


/**
 * Give back a Dynamic Buffer byte stored at a given position.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param byte address of the extern byte that will hold the seeked Dynamic
 * Buffer byte
 * \param position position of the Dynamic Buffer seeked byte
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufGetByteAt(const miscDYN_BUF *dynBuf,
                                  char *byte,
                                  const mcsUINT32 position)
{
    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufChkPositionParam(dynBuf, position) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Test the 'write to' byte buffer address parameter validity */
    if (byte == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "byte");
        return mcsFAILURE;
    }

    /* Write back the seeked character inside the byte buffer parameter */
    *byte = dynBuf->dynBuf[position - miscDYN_BUF_BEGINNING_POSITION];

    return mcsSUCCESS;
}


/**
 * Give back a part of a Dynamic Buffer in an already allocated extern buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param bytes address of the receiving, already allocated extern buffer
 * \param from first Dynamic Buffer byte to be copied in the extern buffer
 * \param to last Dynamic Buffer byte to be copied in the extern buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 */
mcsCOMPL_STAT miscDynBufGetBytesFromTo(const miscDYN_BUF *dynBuf,
                                       char *bytes,
                                       const mcsUINT32 from,
                                       const mcsUINT32 to)
{
    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufChkFromToParams(dynBuf, from, to) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Test the 'bytes' parameter validity */
    if (bytes == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "bytes");
        return mcsFAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be copied */
    mcsINT32 lengthToCopy = (to - from) + 1;

    /* Compute the first 'to be read' Dynamic Buffer byte position */
    char *positionToReadFrom = dynBuf->dynBuf + 
                               (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Copy the Dynamic Buffer desired part in the extern buffer */
    if (memcpy(bytes, positionToReadFrom, lengthToCopy) == NULL)
    {
        errAdd(miscERR_ALLOC);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Give back a part of a Dynamic Buffer as a null terminated string, in an
 * already allocated extern buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param str address of the receiving, already allocated extern buffer
 * \param from first Dynamic Buffer byte to be copied in the extern string
 * \param to last Dynamic Buffer byte to be copied in the extern string
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 */
mcsCOMPL_STAT miscDynBufGetStringFromTo(const miscDYN_BUF *dynBuf,
                                        char *str,
                                        const mcsUINT32 from,
                                        const mcsUINT32 to)
{
    /* Get the requested bytes array from the Dynamic Buffer */
    if (miscDynBufGetBytesFromTo(dynBuf, str, from, to) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Compute the 'str' buffer length */
    mcsUINT32 stringLength = (to - miscDYN_BUF_BEGINNING_POSITION) - 
                             (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Add an '\0' at the end of the received result */
    str[stringLength + 1] = '\0';

    return mcsSUCCESS;
}


/**
 * Set the null-terminated string holding the comment pattern to be skipped when
 * running throughout the Dynamic Buffer line by line.
 *
 * If no comment pattern is specified, no line will be skipped while using
 * miscDynBufGetNextLine().
 *
 * Comment pattern doesn't have to be defined explicitly through this function
 * if miscDynBufLoadFile() is used: comment pattern can be specified directly
 * through one of the function parameter.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param commentPattern a null-terminated string defining the comment pattern
 * that can be skipped when using miscDynBufGetNextLine(), or NULL pointer if it
 * is not defined
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufSetCommentPattern(miscDYN_BUF *dynBuf,
                                          const char *commentPattern)
{
    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /* If the commentPattern parameter is undefined */
    if (commentPattern == NULL)
    {
        dynBuf->commentPattern[0] = '\0';
    }
    else
    {
        /* Store the commentPattern in the Dynamic Buffer */
        strncpy(dynBuf->commentPattern,
                commentPattern,
                sizeof(dynBuf->commentPattern));
        dynBuf->commentPattern[sizeof(dynBuf->commentPattern) - 1] = '\0';
    }

    return mcsSUCCESS;
}


/**
 * Load the content of a specified file into a Dynamic Buffer.
 *
 * The Dynamic Buffer will be overwritten.
 * Comment pattern is used to define comment pattern directly through this
 * function call. It allows then to skip comment lines directly when
 * miscDynBufGetNextLine() is used, without calling explicitly
 * miscDynBufSetCommentPattern() function before.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param fileName path specifying the file to be loaded in the Dynamic Buffer
 * \param commentPattern bytes that defines the comment pattern, or NULL pointer
 * if it is not defined
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning
 * - The given Dynamic Buffer content (if any) will be \em destroyed by this
 * function call.
 * - The given file path must have been \em resolved before this function call.
 * See miscResolvePath() documentation for more information.
 */
mcsCOMPL_STAT miscDynBufLoadFile(miscDYN_BUF *dynBuf,
                                 const char *fileName,
                                 const char *commentPattern)
{
    /* Destroy the received Dynamic Buffer first */
    if (miscDynBufDestroy(dynBuf) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Get the file size */
    struct stat fileStats;
    if (stat(fileName, &fileStats) == -1)
    {
        errAdd(miscERR_DYN_BUF_COULD_NOT_READ_FILE, fileName, strerror(errno));
        return mcsFAILURE;
    }
    size_t fileSize = fileStats.st_size;

    /* Allocate some memory to store the file content */
    if (miscDynBufAlloc(dynBuf, fileSize) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Open the specified file */
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        errAdd(miscERR_DYN_BUF_COULD_NOT_READ_FILE, fileName, strerror(errno));
        return mcsFAILURE;
    }

    /* Get all the content of the specified file */
    size_t readSize = fread((void*)dynBuf->dynBuf, sizeof(char),
                            fileSize, file);

    /* Test if the file seems to be loaded correctly */
    if ((readSize != fileSize) || (dynBuf->dynBuf == NULL))
    {
        errAdd(miscERR_DYN_BUF_COULD_NOT_READ_FILE, fileName, strerror(errno));
        return mcsFAILURE;
    }

    /* Close file */
    fclose(file);
    
    /* Update the Dynamic Buffer internal counters */
    dynBuf->storedBytes = fileSize;

    /* Set the Dynamic Buffer comment pattern */
    return(miscDynBufSetCommentPattern(dynBuf, commentPattern));
}


/**
 * Put Dynamic Buffer content in a specified file.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param fileName path specifying the file to be over-written with the Dynamic
 * Buffer content
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 * 
 * \n
 * \warning
 * - The given file will be over-written on each call.
 * - The given file path must have been \em resolved before this function call.
 * See miscResolvePath() documentation for more information.
 */
mcsCOMPL_STAT miscDynBufSaveInFile(const miscDYN_BUF *dynBuf,
                                   const char *fileName)
{
    /* Verify the Dynamic Buffer initialisation state */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        return mcsFAILURE;
    }

    /* Open (or create)  the specified file in 'write' mode */
    FILE *file = fopen(fileName, "w");
    if (file == NULL)
    {
        errAdd(miscERR_DYN_BUF_COULD_NOT_SAVE_FILE, fileName, strerror(errno));
        return mcsFAILURE;
    }

    /* Get the Dynamic Buffer size to be saved */
    mcsUINT32 dynBufSize;
    if (miscDynBufGetNbStoredBytes(dynBuf, &dynBufSize) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    /* Put all the content of the Dynamic Buffer in the file */
    size_t savedSize = fwrite((void*)miscDynBufGetBuffer(dynBuf), sizeof(char),
                              dynBufSize, file);

    /* Test if the Dynamic Buffer has been saved correctly */
    if (savedSize != dynBufSize)
    {
        errAdd(miscERR_DYN_BUF_COULD_NOT_SAVE_FILE, fileName, strerror(errno));
        return mcsFAILURE;
    }

    /* Close file */
    fclose(file);
    
    return mcsSUCCESS;
}


/**
 * Replace a Dynamic Buffer byte at a given position.
 *
 * The Dynamic Buffer replaced byte will be overwritten.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param byte byte to be written in the Dynamic Buffer
 * \param position position of the Dynamic Buffer byte to be overwritten
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufReplaceByteAt(miscDYN_BUF *dynBuf,
                                      char byte,
                                      const mcsUINT32 position)
{
    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufChkPositionParam(dynBuf, position) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Overwrite the specified Dynamic Buffer byte with the received one */
    dynBuf->dynBuf[position - miscDYN_BUF_BEGINNING_POSITION] = byte;

    return mcsSUCCESS;
}


/**
 * Replace a given range of Dynamic Buffer bytes by extern buffer bytes.
 *
 * The Dynamic Buffer replaced bytes will be overwritten. Their range can be
 * smaller or bigger than the extern buffer bytes number. If range is bigger
 * than extern buffer bytes number, Dynamic Buffer will be expanded dynamically.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param bytes address of the extern buffer bytes to be written in the Dynamic
 * Buffer
 * \param length number of extern buffer bytes to be written in the Dynamic
 * Buffer
 * \param from position of the first Dynamic Buffer byte to be substituted
 * \param to position of the last Dynamic Buffer byte to be substituted
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 * 
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufReplaceBytesFromTo(miscDYN_BUF *dynBuf,
                                           char *bytes,
                                           const mcsUINT32 length,
                                           const mcsUINT32 from,
                                           const mcsUINT32 to)
{
    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufChkFromToParams(dynBuf, from, to) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufChkBytesAndLengthParams(bytes, length) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* 
     * Compute the number of bytes by which the Dynamic Buffer should be
     * expanded and allocate them
     */
    mcsINT32 bytesToAlloc = length - 
                            (((to - miscDYN_BUF_BEGINNING_POSITION) - 
                            (from - miscDYN_BUF_BEGINNING_POSITION)) + 1);
    if (miscDynBufAlloc(dynBuf, bytesToAlloc) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    mcsINT32 lengthToBackup = dynBuf->storedBytes - 
                         ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be backep up' Dynamic Buffer byte position */
    char *positionToBackup = dynBuf->dynBuf + 
                             ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be overwritten' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf + 
                              (from - miscDYN_BUF_BEGINNING_POSITION);

    /* Move the 'not-to-be-overwritten' Dynamic Buffer bytes to their
     * definitive place
     */
    memmove(positionToWriteIn + length, positionToBackup, lengthToBackup);

    /* Copy the extern buffer bytes in the Dynamic Buffer */
    memcpy(positionToWriteIn, bytes, length);

    /* Update the Dynamic Buffer stored length value using the computed
     * signed 'bytesToAlloc' value
     */
    dynBuf->storedBytes += bytesToAlloc;

    return mcsSUCCESS;
}


/**
 * Replace a given range of Dynamic Buffer bytes by an extern string without its
 * ending '\\0'.
 *
 * The Dynamic Buffer replaced bytes will be overwritten. Their range can be
 * smaller or bigger than the extern string length. If range is bigger than
 * extern buffer bytes number, Dynamic Buffer will be expanded dynamically. If
 * the end of the Dynamic Buffer has to be replaced, the string ending '\\0' is
 * keeped.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param str address of the extern string to be written in the Dynamic Buffer
 * \param from position of the first Dynamic Buffer byte to be substituted
 * \param to the position of the last Dynamic Buffer byte to be substituted
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufReplaceStringFromTo (miscDYN_BUF  *dynBuf,
                                             char *str,
                                             const mcsUINT32 from,
                                             const mcsUINT32 to)
{
    /* Test the 'str' parameter validity */
    mcsUINT32 stringLength;
    stringLength = miscDynBufChkStringParam(str);
    if (stringLength == 0)
    {
        return mcsFAILURE;
    }

    /* If the end of the Dynamic Buffer has to be replaced... */
    if (to == dynBuf->storedBytes)
    {
        /* 
         * Increment the number of bytes to be copied from the string in order
         * to add the ending '\0' to the Dynamic Buffer
         */
        ++stringLength;
    }
    
    /* 
     * Replace Dynamic Buffer specified bytes with the string (with or
     * without its ending '\0')
     */
    return(miscDynBufReplaceBytesFromTo(dynBuf, str, stringLength-1, from, to));
}


/**
 * Copy extern buffer bytes at the end of a Dynamic Buffer.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param bytes address of the extern buffer bytes to be written at the end of
 * the Dynamic Buffer
 * \param length number of extern buffer bytes to be written at the end of the
 * Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufAppendBytes(miscDYN_BUF *dynBuf,
                                    const char *bytes,
                                    const mcsUINT32 length)
{
    /* Initialize the received Dynamic Buffer if it is not */
    if (miscDynBufIsInitialised(dynBuf) == mcsFALSE)
    {
        if (miscDynBufInit(dynBuf) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    /* Expand the received Dynamic Buffer size */
    mcsUINT32 nonUsedBytes;
    mcsINT32 bytesToAlloc;
    nonUsedBytes = dynBuf->allocatedBytes - dynBuf->storedBytes;
    bytesToAlloc = length - nonUsedBytes;
    if (miscDynBufAlloc(dynBuf, bytesToAlloc) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufChkBytesAndLengthParams(bytes, length) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Copy the extern buffer bytes at the end of the Dynamic Buffer */
    memcpy(dynBuf->dynBuf + dynBuf->storedBytes, bytes, length);

    /* 
     * Update the Dynamic Buffer stored length value using the number of the
     * extern buffer bytes
     */
    dynBuf->storedBytes += length;

    return mcsSUCCESS;
}


/**
 * Copy an extern string (a null terminated char array) at the end of a
 * Dynamic Buffer, adding an '\\0' at the end of it.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param str address of the extern string to be written at the end of the
 * Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufAppendString(miscDYN_BUF *dynBuf,
                                     const char *str)
{
    /* Test the 'str' parameter validity */
    mcsUINT32 stringLength;
    stringLength = miscDynBufChkStringParam(str);
    if (stringLength == 0)
    {
        return mcsFAILURE;
    }

    /* Get the Dynamic Buffer stored bytes number */
    mcsUINT32 storedBytes;
    if (miscDynBufGetNbStoredBytes(dynBuf, &storedBytes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* If the Dynamic Buffer already contain something... */
    if (storedBytes != 0)
    {
        /* Get the last character of the Dynamic Buffer */
        char lastDynBufChr = '\0';
        if (miscDynBufGetByteAt(dynBuf, &lastDynBufChr, storedBytes)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    
        /* 
         * If the Dynamic Buffer was already holding a null-terminated string...
         */
        if (lastDynBufChr == '\0')
        {
            /* Remove the ending '\0' from the Dynamic Buffer */
            if (miscDynBufDeleteBytesFromTo(dynBuf, storedBytes, storedBytes)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    /* Append the string bytes, including its '\0' */
    return(miscDynBufAppendBytes(dynBuf, str, stringLength));
}


/**
 * Copy a carriage return ('\\n') followed by an extern string (a null
 * terminated char array) at the end of a Dynamic Buffer, adding an '\\0' at the
 * end of it.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param line address of the extern string to be written at the end of a
 * Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDynBufAppendLine(miscDYN_BUF *dynBuf,
                                   const char *line)
{
    /* Test the 'line' parameter validity */
    mcsUINT32 lineLength;
    lineLength = miscDynBufChkStringParam(line);
    if (lineLength == 0)
    {
        return mcsFAILURE;
    }

    /* Get the Dynamic Buffer stored bytes number */
    mcsUINT32 storedBytes;
    if (miscDynBufGetNbStoredBytes(dynBuf, &storedBytes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* If the Dynamic Buffer already contain something... */
    if (storedBytes != 0)
    {

        /* Get the last character of the Dynamic Buffer */
        char lastDynBufChr = '\0';
        if (miscDynBufGetByteAt(dynBuf, &lastDynBufChr, storedBytes)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    
        /* 
         * If the Dynamic Buffer was already holding a null-terminated string...
         */
        if (lastDynBufChr == '\0')
        {
            /* Replace the ending '\0' by an '\n' */
            if (miscDynBufReplaceByteAt(dynBuf, '\n', storedBytes)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    /* Append the line, with its '\0' */
    return(miscDynBufAppendBytes(dynBuf, line, lineLength));
}


/**
 * Copy a carriage return ('\\n') followed by the Dynamic Buffer \em internal
 * comment pattern and an extern string (a null terminated char array) at the
 * end of a Dynamic Buffer, adding an '\\0' at the  end of it.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param line address of the extern string to be written at the end of a
 * Dynamic Buffer
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \sa miscDynBufGetCommentPattern() and miscDynBufSetCommentPattern()
 */
mcsCOMPL_STAT miscDynBufAppendCommentLine(miscDYN_BUF *dynBuf,
                                          const char *line)
{
    /* Test the 'line' parameter validity */
    mcsUINT32 lineLength;
    lineLength = miscDynBufChkStringParam(line);
    if (lineLength == 0)
    {
        return mcsFAILURE;
    }

    /* Append the comment pattern to the Dynamic Buffer */
    if (miscDynBufAppendLine(dynBuf, miscDynBufGetCommentPattern(dynBuf))
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Append the line, with its '\0' */
    return(miscDynBufAppendString(dynBuf, line));
}


/**
 * Insert extern buffer bytes in a Dynamic Buffer at a given position.
 *
 * The Dynamic Buffer bytes at the right side of insertion point are not
 * overwritten, but shiffted to the right.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param bytes pointer to the extern buffer bytes to be inserted
 * \param length number of extern buffer bytes to be inserted
 * \param position position of the first Dynamic Buffer byte to write
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufInsertBytesAt(miscDYN_BUF *dynBuf,
                                      char *bytes,
                                      const mcsUINT32 length,
                                      const mcsUINT32 position)
{
    /* Test the 'dynBuf' and 'position' parameters validity */
    if (miscDynBufChkPositionParam(dynBuf, position) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Test the 'bytes' and 'length' parameters validity */
    if (miscDynBufChkBytesAndLengthParams(bytes, length) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Expand the received Dynamic Buffer size */
    if (miscDynBufAlloc(dynBuf, length) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    mcsINT32 lengthToBackup = dynBuf->storedBytes - 
                         (position - miscDYN_BUF_BEGINNING_POSITION);

    /* Compute the first 'to be overwritten' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf + 
                              (position - miscDYN_BUF_BEGINNING_POSITION);

    /* 
     * Move the 'not-to-be-overwritten' Dynamic Buffer bytes to their
     * definitive place
     */
    memmove(positionToWriteIn + length, positionToWriteIn, lengthToBackup);

    /* Copy the extern buffer bytes in the Dynamic Buffer */
    memcpy(positionToWriteIn, bytes, length);

    /* 
     * Update the Dynamic Buffer stored length value using the extern buffer
     * bytes number
     */
    dynBuf->storedBytes += length;

    return mcsSUCCESS;
}


/**
 * Insert an extern string (a null terminated char array) without its ending
 * '\\0' in a Dynamic Buffer at a given position.
 *
 * The Dynamic Buffer bytes at the right side of insertion point are not
 * overwritten, but shiffted to the right.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param str pointer to the extern string to be inserted
 * \param position position of the first Dynamic Buffer byte to write
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 * 
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufInsertStringAt(miscDYN_BUF *dynBuf,
                                       char *str,
                                       const mcsUINT32 position)
{
    /* Test the 'str' parameter validity */
    mcsUINT32 stringLength;
    stringLength = miscDynBufChkStringParam(str);
    if (stringLength == 0)
    {
        return mcsFAILURE;
    }

    /* Insert the string without its ending '\0' in the Dynamic Buffer */
    return(miscDynBufInsertBytesAt(dynBuf, str, stringLength - 1, position));
}


/**
 * Delete a given range of Dynamic Buffer bytes.
 *
 * \param dynBuf address of a Dynamic Buffer structure
 * \param from position of the first Dynamic Buffer byte to be deleted
 * \param to position of the last Dynamic Buffer byte to be deleted
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \n
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.
 */
mcsCOMPL_STAT miscDynBufDeleteBytesFromTo(miscDYN_BUF *dynBuf,
                                          const mcsUINT32 from,
                                          const mcsUINT32 to)
{
    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    if (miscDynBufChkFromToParams(dynBuf, from, to) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    mcsINT32 lengthToBackup = dynBuf->storedBytes - 
                         ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be backep up' Dynamic Buffer byte position */
    char *positionToBackup = dynBuf->dynBuf + 
                             ((to - miscDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be deleted' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf + 
                              (from - miscDYN_BUF_BEGINNING_POSITION);

    /* 
     * Move the 'not-to-be-deleted' Dynamic Buffer bytes to their
     * definitive place
     */
    memmove(positionToWriteIn, positionToBackup, lengthToBackup);

    /* 
     * Update the Dynamic Buffer stored length value using the deleted bytes
     * number
     */
    dynBuf->storedBytes -= ((to - miscDYN_BUF_BEGINNING_POSITION) - 
                           (from - miscDYN_BUF_BEGINNING_POSITION)) + 1;

    return mcsSUCCESS;
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errUnpackLocalStack_L.c,v 1.2 2005-01-24 14:45:09 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>

/*
 * Common Software Headers
 */
#include "mcs.h"
#include "log.h"

/*
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/**
 * Re-initialize the error structure to start a new error stack.
 * 
 * \param  error Error structure to be reset.
 */
mcsCOMPL_STAT errUnpackLocalStack(errERROR *error, char *buffer, 
                                  mcsUINT32 bufLen)
{
    mcsUINT32    i;
    mcsUINT32   nbErrors;
    mcsUINT32   bufPos;

    logExtDbg("errUnpackLocalStack()"); 
    
    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Replace CR by '\0' */
    nbErrors = 0;
    for (i = 0; i < bufLen; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
            nbErrors += 1;
        }
    }
    
    /* For each error message */
    bufPos = 0;
    for ( i = 0; i < nbErrors; i++)
    {
        mcsMODULEID  moduleId;
        mcsINT32     errorId;
        mcsSTRING64  timeStamp;
        mcsFILE_LINE location;
        char         severity;
        mcsSTRING256 runTimePar;
        mcsPROCNAME  procName;

        /* Retreive error structure fields */
        if (sscanf(&buffer[bufPos], "%s %*s %s %s %s %d %c %[^^]",
                timeStamp, moduleId, procName, location, &errorId, &severity,
                runTimePar) != 7)
        {
             logWarning("invalid buffer format");
            return mcsFAILURE;
        }

        bufPos += strlen(&buffer[bufPos]) + 1;
        
        /* Add error to the stack */
        if (errPushInLocalStack(error, timeStamp, mcsGetProcName(), moduleId, 
                                location, errorId, 
                                severity, runTimePar) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/


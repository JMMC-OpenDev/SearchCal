/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.4  2005/01/27 14:11:52  gzins
* Changed errERROR to errERROR_STACK
* Added isErrUser parameter
*
* Revision 1.3  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errUnpackLocalStack_L.c,v 1.5 2005-02-09 14:26:03 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
mcsCOMPL_STAT errUnpackLocalStack(errERROR_STACK *error,
                                  const char     *buffer, 
                                  mcsUINT32      bufLen)
{
    mcsUINT32 i;
    mcsUINT32 nbErrors;
    mcsUINT32 bufPos;

    logExtDbg("errUnpackLocalStack()"); 
    
    /* Try tp copy the given buffer */
    char *temp = (char*)malloc(bufLen);
    if (temp == NULL)
    {
        logWarning("could NOT allocate temporary buffer");
        return mcsFAILURE;
    }
    memcpy(temp, buffer, bufLen);

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Replace CR by '\0' */
    nbErrors = 0;
    for (i = 0; i < bufLen; i++)
    {
        if (temp[i] == '\n')
        {
            temp[i] = '\0';
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
        mcsINT32     isErrUser;
        char         severity;
        mcsSTRING256 runTimePar;
        mcsPROCNAME  procName;

        /* Retreive error structure fields */
        if (sscanf(&temp[bufPos], "%s %*s %s %s %s %d %d %c %[^^]",
                timeStamp, moduleId, procName, location, &errorId, &isErrUser,
                &severity, runTimePar) != 8)
        {
             logWarning("invalid buffer format");
            return mcsFAILURE;
        }

        bufPos += strlen(&temp[bufPos]) + 1;
        
        /* Add error to the stack */
        if (errPushInLocalStack(error, timeStamp, mcsGetProcName(), moduleId, 
                                location, errorId, isErrUser,
                                severity, runTimePar) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/


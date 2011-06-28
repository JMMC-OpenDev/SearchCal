/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errUnpackLocalStack function.
 */


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
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT errUnpackLocalStack(errERROR_STACK *error,
                                  const char     *buffer, 
                                  mcsUINT32      bufLen)
{
    mcsUINT32 i;
    mcsUINT32 nbErrors;
    mcsUINT32 bufPos;

    logTrace("errUnpackLocalStack()"); 
    
    if (error == NULL)
    {
        return mcsFAILURE;
    }
    
    /* Try tp copy the given buffer */
    char *temp = (char*)malloc(bufLen);
    if (temp == NULL)
    {
        logWarning("could NOT allocate temporary buffer");
        return mcsFAILURE;
    }
    memcpy(temp, buffer, bufLen);

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
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


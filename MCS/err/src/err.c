/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: err.c,v 1.2 2004-06-21 17:09:47 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */

#include <string.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

/*
 * Common Software Headers
 */
#include "mcs.h"
#include "misc.h"
#include "log.h"

/*
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/*
 * Declaration of local functions
 */
extern mcsCOMPL_STAT addInStack(errERROR   *error,
                                   const char *timeStamp,
                                   const char *procName,
                                   const char *moduleId,
                                   const char *location,
                                   mcsINT32   errorId,
                                   char       severity,
                                   char       *runTimePar);
/* 
 * Definition of local functions
 */ 
mcsCOMPL_STAT addInStack(errERROR      *error,
                            const char *timeStamp,
                            const char *procName,
                            const char *moduleId,
                            const char *location,
                            mcsINT32   errorId,
                            char       severity,
                            char       *runTimePar)
{

    mcsINT32 errNum;

    logExtDbg("addInStack()"); 

    /* Check parameter */
    if (error  ==  NULL)
    {
        logPrint(MODULE_ID, logWARNING, __FILE_LINE__, "%s", 
                "addInStack() - le parametre 'error' est un pointeur NULL");
       return(FAILURE);
    }

    /* Si la pile est remplie */
    if (error->stackSize == errSTACK_SIZE)
    {
        logPrint(MODULE_ID, logWARNING, __FILE_LINE__, "%s", 
                "addInStack() - la pile d'erreur est complete: "
                "log automatique des messages d'erreur");

        /* Affichage des messages de la pile d'erreur */
        errCloseStack(error);
    }

    /* Incremente le nombre d'erreurs dans la pile, et met a jour l'etat de la
     * pile */
    error->stackSize++;
    error->stackEmpty = mcsFALSE;

    /* Ajoute l'erreur a la pile */
    errNum = error->stackSize - 1;

    /* Complete les champs */
    error->stack[errNum].sequenceNumber = error->stackSize;
    strncpy((char *)error->stack[errNum].timeStamp, timeStamp,
            (sizeof(error->stack[errNum].timeStamp)-1));
    strncpy((char *)error->stack[errNum].procName, mcsGetProcName(),
            (sizeof(mcsPROCNAME)-1));
    strncpy((char *)error->stack[errNum].moduleId, moduleId,
            (sizeof(mcsMODULEID)-1));
    strncpy((char *)error->stack[errNum].location, location,
            (sizeof(mcsFILE_LINE) -1));
    strcpy((char *) error->stack[errNum].moduleId, moduleId);
    error->stack[errNum].errorId = errorId;
    error->stack[errNum].severity = severity;
    strcpy((char *)error->stack[errNum].runTimePar, runTimePar);
 
    return SUCCESS;
}

/*
 * Public functions
 */
mcsCOMPL_STAT errAddInStack(errERROR          *error,
                            const mcsMODULEID moduleId,
                            mcsINT32          errorId,
                            const char        *fileLine, ...)
{
    va_list      argPtr;
    mcsSTRING64  timeStamp;
    char         severity;
    mcsSTRING256 format;
    mcsSTRING256 runTimePar;

    logExtDbg("errAdd()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetStack(error);
    } 

    /* Get the error format and severity of error */
/*    errGetErrorDesc(moduleId, errorId, severity, format); */
    severity = 'W';
    sprintf(format, "Error #%d", errorId);

    /* Get the current UTC date/time */
    miscGetUtcTimeStr(timeStamp, 5);


    /* Fill the error message */
    va_start(argPtr,fileLine);
    vsprintf(runTimePar, format, argPtr);
    va_end(argPtr);
    
    /* Add error to the stack */
    return (addInStack(error, timeStamp, mcsGetProcName(), moduleId,
                          fileLine, errorId, severity, runTimePar));
}

mcsLOGICAL errIsInStack (errERROR          *error,
                         const mcsMODULEID moduleId,
                         mcsINT32          errorId)
{
    mcsINT32 i;

    logExtDbg("errIsInStack()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetStack(error);
        return mcsFALSE;
    } 

    /* For each error in stack */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* If module names match */
        if (strcmp(error->stack[i].moduleId, moduleId) == 0)
        {
            /* If error Ids match */
            if (error->stack[i].errorId == errorId)
            {
                /* Return TRUE */
                return mcsTRUE;
            }
        }
    }

    /* Else return FALSE */
    return mcsFALSE;
}

mcsCOMPL_STAT errResetStack(errERROR *error)
{
    mcsINT32 i;

    logExtDbg("errResetStack()");

    /* Initialize the error structure */
    for ( i = 0; i < error->stackSize; i++)
    {
        memset((char *)error->stack[i].timeStamp, '\0',
                sizeof(error->stack[i].timeStamp));
        error->stack[i].sequenceNumber = -1;
        memset((char *)error->stack[i].procName, '\0',
                sizeof(error->stack[i].procName));
        memset((char *)error->stack[i].location, '\0',
                sizeof(error->stack[i].location));
        memset((char *)error->stack[i].moduleId, '\0',
                sizeof(error->stack[i].moduleId));
        error->stack[i].severity = ' ';
        memset((char *)error->stack[i].runTimePar, '\0',
                sizeof(error->stack[i].runTimePar));
    }
    error->stackSize = 0;
    error->stackOverflow = mcsFALSE;
    error->stackEmpty = mcsTRUE;

    error->thisPtr = error;

    return SUCCESS;
}

mcsCOMPL_STAT errCloseStack(errERROR *error)
{
    mcsINT32     i;
    mcsSTRING128 tab;

    logExtDbg("errCloseStack()");
 
    /* If error stack is initialised */
    if (error->thisPtr == error)
    {
        memset(tab, '\0', sizeof(tab));

        /* For each error message */
        for ( i = 0; i < error->stackSize; i++)
        {
            /* Format the  message */
            char log[errMSG_MAX_LEN];
            char logBuf[512];

            memset(logBuf, '\0', sizeof(logBuf));

            sprintf(logBuf,"%d %c %s",
                    error->stack[i].sequenceNumber,
                    error->stack[i].severity,
                    error->stack[i].runTimePar);

            /* Check length */
            if ((strlen(logBuf) + strlen(tab)) > errMSG_MAX_LEN)
            {
                logBuf[errMSG_MAX_LEN - strlen(tab)] = '\0';
            }

            sprintf(log,"%s%s", tab, logBuf);

            /* Send message to log system */
            logData (error->stack[i].moduleId,
                     logERROR,
                     error->stack[i].timeStamp, 
                     error->stack[i].location, log);

            /* Add tab to show error message hierarchy */
            strcat(tab, " ");
        }
    }

    /* Re-initialise error stack */
    errResetStack(error);

    return SUCCESS;
}

mcsCOMPL_STAT errDisplay(errERROR *error)
{
    char buffer[2048];

    logExtDbg("errDisplay()");
     
    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetStack(error);
    } 

    /* Format error messages */
    errStoreExtrBuffer(error, buffer, 2048, mcsTRUE); 

    /* Display error messages */
    printf ("%s", buffer);

    return SUCCESS;
}

mcsLOGICAL errIsEmpty (errERROR *error)
{
    logExtDbg("errIsEmpty()");

    /* Return empty flag */
    return (error->stackEmpty);
}

mcsCOMPL_STAT errStoreExtrBuffer(errERROR *error, char *buffer,
        mcsUINT32 bufSize, mcsLOGICAL tabulate)
{
    mcsINT32     i;
    mcsSTRING128 tab;
    mcsUINT32    bufLen;

    logExtDbg("errStoreExtrBuffer()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetStack(error);
    } 

    memset(tab, '\0', sizeof(tab));
    memset(buffer, '\0', bufSize);
    bufLen = 0;
    
    /* For each error message */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* Format error message */
        char log[errMSG_MAX_LEN];
        char logBuf[512];

        sprintf(logBuf,"%s %d %c %s",
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Check length */
        if ((strlen(logBuf) + strlen(tab)) > errMSG_MAX_LEN)
        {
            logBuf[errMSG_MAX_LEN - strlen(tab)] = '\0';
        }

        memset(log, '\0', sizeof(log));
        sprintf(log,"%s - %s %s %s%s\n",
                error->stack[i].timeStamp, error->stack[i].moduleId,
                error->stack[i].procName, tab, logBuf);

        /* Store message into buffer */
        if ((bufSize - bufLen) > strlen(log))
        {
            strncpy(&buffer[bufLen], log, strlen(log));
            bufLen += strlen(log);
        }
        else
        {
            logPrint(MODULE_ID, logWARNING, __FILE_LINE__, 
                    "errStoreExtrBuffer() - buffer too small "
                    "(current size: %d  requested size: %.80s)",
                    bufSize, bufLen + strlen(log));
            return FAILURE;
        }
        
        /* Add tab to show error message hierarchy */
        if (tabulate == mcsTRUE)
        {
            strcat(tab, " ");
        }
    }

    return SUCCESS;
}

mcsCOMPL_STAT errLoadExtrBuffer(errERROR *error, char *buffer, 
        mcsUINT32 bufLen)
{
    mcsUINT32    i;
    mcsUINT32   nbErrors;
    mcsUINT32   bufPos;

    logExtDbg("errLoadExtrBuffer()"); 
    
    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetStack(error);
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
            logPrint(MODULE_ID, logWARNING,  __FILE_LINE__, "%s", 
                    "errLoadExtrBuffer() - Format du buffer incorrect");
            return FAILURE;
        }

        bufPos += strlen(&buffer[bufPos]) + 1;
        
        /* Add error to the stack */
        if (addInStack(error, timeStamp, mcsGetProcName(), moduleId, 
                    location, errorId, severity, runTimePar) == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}


/*___oOo___*/


/*******************************************************************************
*  JMMC Project
*  
*  "@(#) $Id: log.c,v 1.1 2004-05-13 12:54:51 mella Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* mella   07/05/04  Preliminary version based on IBAC from VLT/ESO
* 
*/

/* 
 * System Headers 
 */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/*
 * Local Headers 
 */
#include "log.h"

/*
 * Default initialization
 */
static logRULE logRule = {
    mcsTRUE,
    mcsTRUE,
    logWARNING,
    logWARNING,
    logWARNING,
    mcsTRUE,
    mcsTRUE
};
static logRULE *logRulePtr = &logRule;

static mcsPROCNAME logProcName = "procUNKNOWN";

static mcsMODULEID logModName = "NoMOD";

/**
 * Get names to identify the process and the module where it comes from.
 * One not identified process is named procUNKNOWN and relates to NoMOD module
 * name.
 *
 * @param processName name of the process.
 * @param moduleName name of the software module.
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logIdentify(const mcsPROCNAME processName, const mcsMODULEID moduleName){
    /* store values into global variables */
    strncpy(logProcName, processName, mcsPROCNAME_LEN);
    strncpy(logModName, moduleName, mcsMODULEID_LEN);
    return SUCCESS;
}

/**
 * Toggle the date output. Useful in test mode.
 * 
 * @param flag mcsTRUE/mcsFALSE
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL flag)
{
    /* Set 'print date' flag  */
    logRulePtr->printDate = flag;
    return SUCCESS;
}

/**
 * Toggle the fileline output. Useful in test mode.
 * 
 * @param flag mcsTRUE/mcsFALSE
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL flag)
{
    /* Set 'print line/file' flag  */
    logRulePtr->printFileLine = flag;
    return SUCCESS;
}



/**
 * Print action log.
 *
 * @param level  TBD
 * @param logFormat msg 
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logPrintAction(logLEVEL level, const char *logFormat, ...)
{ 
    va_list         argPtr;
    mcsCOMPL_STAT   stat = SUCCESS;

    /* If the specified level is less than or egal to the action level */
    if (level <= logRulePtr->actionLevel)
    {
        /* do a simple output which could be filtered and displayed in
         * a special position color ...
         * \todo implement real function
         */
        va_start(argPtr,logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout,"\n"); fflush(stdout);
        va_end(argPtr);
    }/* End if */

    return stat;
}


/**
 * Set logging level.
 *  
 * @param level TBD
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetLogLevel (logLEVEL level)
{
    /* Set the logging level to the specified one */
    logRulePtr->logLevel = level;

    return SUCCESS; 
}

/**
 * Toggle the log.
 *
 * @param flag mcsTRUE/mcsFALSE
 *
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetLog(mcsLOGICAL flag)
{
   /* Set the logging mode to the specified one */
   logRulePtr->log = flag;
   return SUCCESS;
}


/**
 * Get logging level.
 *
 * @return logLEVEL actual logging level 
 */
logLEVEL logGetLogLevel ()
{
    /* Returns the logging level  */
    return (logRulePtr->logLevel);
}



/**
 * Toggle the verbosity.
 *
 * @param flag mcsTRUE/mcsFALSE
 * 
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetVerbose(mcsLOGICAL flag)
{

    /* Set the verbose mode to the specified one */
    logRulePtr->verbose = flag;
    return SUCCESS;

}

/**
 * Set verbosity level.
 *
 * @param level  
 *
 * @return mcsCOMPL_STAT
 */
mcsCOMPL_STAT logSetVerboseLevel (logLEVEL level)
{
    
    /* Set the verbosity level to the specified one */
    logRulePtr->verboseLevel = level;

    return SUCCESS; 

}


/**
 * Get verbosity level.
 */
logLEVEL logGetVerboseLevel ()
{

    /* Returns the verbosity level  */
    return (logRulePtr->verboseLevel);

}


/**
 * Set action level.
 * 
 * @param level TBD
 * 
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetActionLevel (logLEVEL level)
{
    /* Set the verbosity level to the specified one */
    logRulePtr->actionLevel = level;

    return SUCCESS; 
}


/**
 * Get action level.
 */
logLEVEL logGetActionLevel ()
{
    /* Returns the action level  */
    return (logRulePtr->actionLevel);
}

/**
 * Log data to logsystem if level satisfies and optionally
 * to the stdout if verbose is specified.
 * 
 * @return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logPrint( logLEVEL level,
                        const char *fileLine,
                        const char *logFormat, ...)
{ 
    char buffer[4*logTEXT_LEN];
    va_list         argPtr;
    mcsCOMPL_STAT   stat = SUCCESS;

    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel))
    {
        /* Log information */
        va_start(argPtr,logFormat);
        vsprintf(buffer, logFormat, argPtr);
        stat = logData(buffer);
        va_end(argPtr);
    }
	
    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel))
    {
        fprintf(stdout, "%s[%s] - ", logProcName, logModName);
        if ( (fileLine != NULL ) && (logRulePtr->printFileLine == mcsTRUE)) 
        {
            fprintf(stdout, "%s - ", fileLine);
        }
        va_start(argPtr,logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout,"\n"); fflush(stdout);
        va_end(argPtr);
    }
    /* End if */

    return stat;

}

/**
 * Place msg into the logging system.
 * \todo fill for the future 
 *  
 * @param msg message to store.
 * 
 * @return mcsCOMPL_STAT 
 */
static mcsCOMPL_STAT logData(const char * msg)
{
    /* TBD */
    /* fprintf(stdout, "logData [%s]\n", msg); */
    return SUCCESS;
}




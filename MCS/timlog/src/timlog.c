/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: timlog.c,v 1.3 2005-02-15 10:27:46 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     17-Dec-2004  Created
 * gzins     20-Dec-2004  Added moduleName and fileLine argument to timlogStart
 *
 ******************************************************************************/

/**
 * \file
 * Definition of timer log functions.
  */

static char *rcsId="@(#) $Id: timlog.c,v 1.3 2005-02-15 10:27:46 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/* 
 * Local Headers
 */
#include "timlog.h"
#include "timlogPrivate.h"
#include "timlogErrors.h"

/*
 * Local Variables
 */
/** Hash table containing all register actions */
static miscHASH_TABLE timlogHashTable;
static mcsLOGICAL     timlogHashTableCreated = mcsFALSE;

/*
 * Public functions definition
 */
/**
 * Adds a start time marker.
 *
 * This function adds a time marker for the specified action to the list of
 * actions to be timed. If time marker exits, it is simply replaces by the new
 * one; i.e. the start time is reset to the current time.
 *
 * \param moduleName name of the module.
 * \param level log level used to log message when timlogStop is called for
 * this action.
 * \param fileLine file name and line number from where the message is issued.
 * \param actionName name of the action to be timed.
 */
void timlogStart(const mcsMODULEID moduleName, const logLEVEL level,
                 const char *fileLine, const char* actionName)
{
    logExtDbg("timlogStart(%s)", actionName);
    
    /* If hash table not created */
    if (timlogHashTableCreated == mcsFALSE)
    {
        /* Creates hash table */
        miscHashCreate (&timlogHashTable, 20);
        timlogHashTableCreated = mcsTRUE;
    }
    /* End if */
    
    /* Allocates and sets new time marker. */
    timlogENTRY *entry;
    
    if ((entry = (timlogENTRY *)malloc(sizeof(timlogENTRY))) == NULL)
    {
        errAdd(timlogERR_ALLOC_MEM);
        errCloseStack();
        return;
    }
    strncpy((char *)entry->moduleName, moduleName, sizeof(mcsMODULEID)-1);
    strncpy((char *)entry->fileLine, fileLine, sizeof(mcsSTRING128)-1);
    strncpy((char *)entry->actionName, actionName, sizeof(mcsSTRING64)-1);
    entry->level = level;
    gettimeofday (&entry->startTime, NULL);

    /* Adds marker in the Hash Table. If marker already exist in table, it is
     * replace by the new one. */
    if (miscHashAddElement(&timlogHashTable, actionName,
                           (void **)&entry, mcsTRUE) == FAILURE)
    {
        free (entry);
        errCloseStack();
        return;		
    }
}

/**
 * Terminate an action.
 *
 * This functions indicates the termination of the specified action, and logs,
 * according to the level, the elapsed time in execution.
 *
 * If the specified action has not been register using timlogStart() then an
 * error is logged.
 *
 * \param actionName name of the action which is terminated.
 */
void timlogStop(const char* actionName)
{
    logExtDbg("timlogStop(%s)", actionName);

    /* Gets current time */
    struct timeval endTime;
    gettimeofday (&endTime, NULL); 
    
    /**** Check the time marker is defined */ 
    /* Check if hash table is initialized */
    if (timlogHashTableCreated == mcsFALSE)
    {
        errAdd(timlogERR_NO_TIME_MARKER, actionName);
        errCloseStack();
        return;
    }

    /* Check timer is in hash table */
    timlogENTRY *entry;
    entry = miscHashGetElement(&timlogHashTable, actionName);
    if (entry == NULL)
    {
        errAdd(timlogERR_NO_TIME_MARKER, actionName);
        errCloseStack();
        return;
    }

    /* Determines the elapsed time from start time. */
    mcsINT32 hour;
    mcsINT32 min;
    mcsINT32 sec;
    mcsINT32 msec;
    mcsINT32 usec;

    sec  = endTime.tv_sec - entry->startTime.tv_sec;
    usec = endTime.tv_usec -  entry->startTime.tv_usec;

    if ( usec < 0 )
    {
        sec  -= 1;
        usec += 1000000;
    }
    hour = sec/3600;
    sec %= 3600;
    min  = sec/60;
    sec %= 60;
    msec = usec / 1000;

    /* Format message */
    mcsSTRING256 logMessage;
    sprintf((char *)logMessage, 
            "Elapsed time in execution of '%s' %02d:%02d:%02d.%03d",
            actionName, hour, min, sec, msec);
    /* Logs timer information */
    logPrint(entry->moduleName, entry->level, entry->fileLine, logMessage);

    /* Deletes time marker */
    if (miscHashDeleteElement(&timlogHashTable, (char *)actionName) == FAILURE)
    {
        errCloseStack();
        return;
    }
}

/**
 * Clears the current list of actions to be timed.
 **/
void timlogClear()
{
    /* Delete hash table */
    miscHashDelete(&timlogHashTable);
    timlogHashTableCreated = mcsFALSE;
}
/*___oOo___*/

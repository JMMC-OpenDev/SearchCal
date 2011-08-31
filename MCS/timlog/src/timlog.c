/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of timer log functions.
  */


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
#include "thrd.h"

/* 
 * Local Headers
 */
#include "timlog.h"
#include "timlogPrivate.h"
#include "timlogErrors.h"

/*
 * Local Variables
 */

/**
 * Shared mutex to protect the internal hash table
 */
static thrdMUTEX timlogHashTableMutex = MCS_MUTEX_STATIC_INITIALIZER;

/** Hash table containing all register actions */
static miscHASH_TABLE timlogHashTable;
static mcsLOGICAL     timlogHashTableCreated = mcsFALSE;

/*
 * Local macro
 */
#define HASH_TABLE_LOCK(error) { \
    if (thrdMutexLock(&timlogHashTableMutex) == mcsFAILURE) \
    { \
        errAdd(timlogERR_HASH_MUTEX); \
        return error; \
    } \
}

#define HASH_TABLE_UNLOCK(error) { \
    if (thrdMutexUnlock(&timlogHashTableMutex) == mcsFAILURE) \
    { \
        errAdd(timlogERR_HASH_MUTEX); \
        return error; \
    } \
}

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
    /* If time-related log is disabled */
    if (logGetPrintDate() == mcsFALSE)
    {
        /* Do nothing ! */
        return;
    }

    logTrace("timlogStart(%s)", actionName);
    
    /* Allocates and sets new time marker. */
    timlogENTRY *entry;
    
    if ((entry = (timlogENTRY *)malloc(sizeof(timlogENTRY))) == NULL)
    {
        errAdd(timlogERR_ALLOC_MEM);
        errCloseStack();
        return;
    }
    
    strncpy(entry->moduleName, moduleName, sizeof(mcsMODULEID) - 1);
    strncpy(entry->fileLine, fileLine, sizeof(mcsSTRING128) - 1);
    strncpy(entry->actionName, actionName, sizeof(mcsSTRING64) - 1);
    entry->level = level;
    gettimeofday (&entry->startTime, NULL);
    
    mcsSTRING64 key;
    /* Prefix the action name with the thread Identifier */
    mcsUINT32 threadId = mcsGetThreadId();
    snprintf(key, sizeof(mcsSTRING64) - 1, "%d-%s", threadId, actionName);

    /* If hash table not created */
    if (timlogHashTableCreated == mcsFALSE)
    {
        timlogInit();
    }

    HASH_TABLE_LOCK();
    
    /* Adds marker in the Hash Table. If marker already exist in table, it is
     * replace by the new one. */
    if (miscHashAddElement(&timlogHashTable, key,
                           (void **)&entry, mcsTRUE) == mcsFAILURE)
    {
        free(entry);
        errCloseStack();
    }
    
    HASH_TABLE_UNLOCK();
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
    timlogStopTime(actionName, NULL);
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
 * \param elapsedTime elapsed time in milliseconds as long (64 bits)
 */
void timlogStopTime(const char* actionName, mcsINT64* elapsedTime)
{
    /* If time-related log is disabled */
    if (logGetPrintDate() == mcsFALSE)
    {
        /* Do nothing ! */
        return;
    }

    logTrace("timlogStop(%s)", actionName);
    
    /* Gets current time */
    struct timeval endTime;
    gettimeofday (&endTime, NULL); 
    
    mcsSTRING64 key;
    /* Prefix the action name with the thread Identifier */
    mcsUINT32 threadId = mcsGetThreadId();
    snprintf(key, sizeof(mcsSTRING64) - 1, "%d-%s", threadId, actionName);
    
    /**** Check the time marker is defined */ 

    HASH_TABLE_LOCK();

    /* Check if hash table is initialized */
    if (timlogHashTableCreated == mcsFALSE)
    {
        HASH_TABLE_UNLOCK();
        errAdd(timlogERR_NO_TIME_MARKER, actionName);
        errCloseStack();
        return;
    }

    /* Check timer is in hash table */
    timlogENTRY *entry;
    entry = miscHashGetElement(&timlogHashTable, key);
    if (entry == NULL)
    {
        HASH_TABLE_UNLOCK();
        errAdd(timlogERR_NO_TIME_MARKER, actionName);
        errCloseStack();
        return;
    }

    HASH_TABLE_UNLOCK();

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
    hour = sec / 3600;
    sec %= 3600;
    min  = sec / 60;
    sec %= 60;
    msec = usec / 1000;
    
    if (elapsedTime != NULL)
    {
        *elapsedTime = (hour * 3600 + sec) * 1000 + msec;
    }

    /* Format message */
    mcsSTRING256 logMessage;
    
    snprintf(logMessage, sizeof(logMessage) - 1, "Elapsed time in execution of '%s' %02d:%02d:%02d.%03d",
            actionName, hour, min, sec, msec);
    
    /* Logs timer information */
    logPrint(entry->moduleName, entry->level, entry->fileLine, logMessage);

    HASH_TABLE_LOCK();

    /* Deletes time marker and frees the entry */
    if (miscHashDeleteElement(&timlogHashTable, key) == mcsFAILURE)
    {
        errCloseStack();
    }

    HASH_TABLE_UNLOCK();
}

/**
 * Format the given elapsed time in milliseconds
 * @param elapsedTime elapsed time in milliseconds
 * @param time formatted time
 */
void timlogFormatTime(mcsINT64 elapsedTime, mcsSTRING16 time)
{
    mcsINT32 hour;
    mcsINT32 min;
    mcsINT32 sec;
    mcsINT32 msec;

    msec = elapsedTime % 1000;
    sec  = elapsedTime / 1000;

    hour = sec / 3600;
    sec %= 3600;
    min  = sec / 60;
    sec  %= 60;
    
    sprintf(time, "%02d:%02d:%02d.%03d", hour, min, sec, msec);
}

/**
 * Discard an action.
 *
 * This functions indicates the termination of one specified action (CANCELLED), and 
 * cleans up internal hash table.
 *
 * \param actionName name of the action which is terminated.
 */
void timlogCancel(const char* actionName)
{
    /* If time-related log is disabled */
    if (logGetPrintDate() == mcsFALSE)
    {
        /* Do nothing ! */
        return;
    }

    logTrace("timlogCancel(%s)", actionName);
    
    mcsSTRING64 key;
    /* Prefix the action name with the thread Identifier */
    mcsUINT32 threadId = mcsGetThreadId();
    snprintf(key, sizeof(mcsSTRING64) - 1, "%d-%s", threadId, actionName);
    
    /**** Check the time marker is defined */ 

    HASH_TABLE_LOCK();

    /* Check if hash table is initialized */
    if (timlogHashTableCreated == mcsFALSE)
    {
        HASH_TABLE_UNLOCK();
        return;
    }

    /* Deletes time marker and frees the entry */
    if (miscHashDeleteElement(&timlogHashTable, key) == mcsFAILURE)
    {
        errAdd(timlogERR_NO_TIME_MARKER, actionName);
        errCloseStack();
    }

    HASH_TABLE_UNLOCK();
}

/**
 * Initialize the internal hash table
 */
void timlogInit()
{
    HASH_TABLE_LOCK();
    
    /* If hash table not created */
    if (timlogHashTableCreated == mcsFALSE)
    {
        /* Creates hash table */
        miscHashCreate (&timlogHashTable, 100);
        timlogHashTableCreated = mcsTRUE;
    }
    /* End if */
    
    HASH_TABLE_UNLOCK();
}

/**
 * Clears the current list of actions to be timed.
 **/
void timlogClear()
{
    HASH_TABLE_LOCK();
    
    /* If hash table not created */
    if (timlogHashTableCreated == mcsTRUE)
    {
        /* Delete hash table */
        miscHashDelete(&timlogHashTable);
        timlogHashTableCreated = mcsFALSE;
    }
    
    HASH_TABLE_UNLOCK();
}
/*___oOo___*/

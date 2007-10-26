/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdbENTRY.cpp,v 1.1 2007-10-26 13:25:26 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Definition of sdbENTRY class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sdbENTRY.cpp,v 1.1 2007-10-26 13:25:26 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "sdbENTRY.h"
#include "sdbPrivate.h"
#include "sdbErrors.h"


/*
 * System Headers 
 */
#include <unistd.h>


/*
 * Static members dfinition 
 */


/**
 * Class constructor
 */
sdbENTRY::sdbENTRY()
{
    _initSucceed = mcsFALSE;
    _isNewMessage = mcsFALSE;
    memset(_buffer, '\0', sizeof(mcsSTRING256));
}

/**
 * Class destructor
 */
sdbENTRY::~sdbENTRY()
{
}


/*
 * Public methods
 */
/**
 * Initialize internal synchronization mecanism.
 *
 * @wa Must be called before any other sdbENTRY calls.
 *
 * @return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT sdbENTRY::Init(void)
{
    if (Destroy() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Mutex initialisation */
    if (thrdMutexInit(&_mutex) == mcsFAILURE)
    {
        _initSucceed = mcsFALSE;
        return mcsFAILURE;
    }

    _initSucceed = mcsTRUE;
    return mcsSUCCESS;
}

/**
 * Destroy internal synchronization mecanism.
 *
 * @wa Must be called after all other sdbENTRY calls.
 *
 * @return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT sdbENTRY::Destroy(void)
{
    if (_initSucceed == mcsTRUE)
    {
        /* Mutex destruction */
        if (thrdMutexDestroy(&_mutex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    _initSucceed = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * Write a new message in the entry.
 *
 * @param message a null-terminated string.
 *
 * @return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT sdbENTRY::Write(const char* message)
{
    logTrace("sdbENTRY::Write()");

    /* Verify parameter vailidity */
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }
    
    // If the object has not been fully initialized yet
    if (_initSucceed == mcsFALSE)
    {
        return mcsFAILURE;
    }

    logDebug("Get the right to write in the internal buffer.");
    if (thrdMutexLock(&_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logDebug("Internal buffer locked.");

    // Copy the new message to the internal buffer
    strncpy(_buffer, message, sizeof(mcsSTRING256));
    _isNewMessage = mcsTRUE;

    logDebug("Release the right to write in the internal buffer.");
    if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logDebug("Internal buffer unlocked.");

    return mcsSUCCESS;
}

/**
 * Read a message from the entry.
 *
 * @param message an already allocated buffer to return the entry content.
 * @param waitNewMessage if mcsTRUE wait for a new message to be posted,
 * otherwise return the current one.
 * @param timeoutInMs if not -1, wait for a new message until the specified
 * amount of milliseonds and return the current one, otherwise wait forever.
 *
 * @return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT sdbENTRY::Read(char*             message,
                             mcsLOGICAL        waitNewMessage,
                             mcsINT32          timeoutInMs)
{
    logTrace("sdbENTRY::Read()");

    /* Verify parameter vailidity */
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }

    // If the object has not been fully initialized yet
    if (_initSucceed == mcsFALSE)
    {
        return mcsFAILURE;
    }

    // Return the current message right away
    if (waitNewMessage == mcsFALSE)
    {
        logDebug("Get the right to read in the internal buffer.");
        if (thrdMutexLock(&_mutex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logDebug("Internal buffer locked.");

        // Copy the new message to the internal buffer
        strncpy(message, _buffer, sizeof(mcsSTRING256));
        _isNewMessage = mcsFALSE;

        logDebug("Release the right to read in the internal buffer.");
        if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logDebug("Internal buffer unlocked.");

        return mcsSUCCESS;
    }

    // Wait forever
    if (timeoutInMs == -1)
    {
        int idleTime = 100000; // 100ms
        // Forever
        while (1)
        {            
            // Lock data access
            logDebug("Get the right to read in the internal buffer.");
            if (thrdMutexLock(&_mutex) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logDebug("Internal buffer locked.");

            // If data has changed
            if (_isNewMessage == mcsTRUE)
            {
                // Return it
                strncpy(message, _buffer, sizeof(mcsSTRING256));
                _isNewMessage = mcsFALSE;

                // Unlock data access
                logDebug("Release the right to read in the internal buffer.");
                if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                logDebug("Internal buffer unlocked.");

                return mcsSUCCESS;
            }

            // Unlock data access
            logDebug("Release the right to read in the internal buffer.");
            if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logDebug("Internal buffer unlocked.");

            // Sleep for a while before restarting
            logDebug("Sleeping %dms before restarting.", idleTime / 1000);
            usleep(idleTime);
        }

        logError("Assertion failed (exited while(1) loop !).");
    }
    else
    {
        // Compute the time left in microseconds
        int timeLeft = (timeoutInMs * 1000);

        // Compute the idle time of each step (the least beetwen 100us and 1/10th
        // of the initial timeout)
        int idleTime = min(100000, timeLeft / 10);

        // While there is some time left
        while (timeLeft > 0)
        {            
            // Lock data access
            logDebug("Get the right to read in the internal buffer.");
            if (thrdMutexLock(&_mutex) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logDebug("Internal buffer locked.");

            // If data has changed
            if (_isNewMessage == mcsTRUE)
            {
                // Return it
                strncpy(message, _buffer, sizeof(mcsSTRING256));
                _isNewMessage = mcsFALSE;

                // Unlock data access
                logDebug("Release the right to read in the internal buffer.");
                if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                logDebug("Internal buffer unlocked.");

                return mcsSUCCESS;
            }

            // Unlock data access
            logDebug("Release the right to read in the internal buffer.");
            if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logDebug("Internal buffer unlocked.");

            // Sleep for a while before restarting
            logDebug("Sleeping %dms before restarting.", idleTime / 1000);
            usleep(idleTime);

            // Compute the time left before timeout
            timeLeft -= idleTime;
            logDebug("%dms left before timeout.", timeLeft / 1000);
        }

        logDebug("Stopped (timed out).");
    }

    return mcsFAILURE;
}

/**
 * Return whter the object has been fully initialized or not.
 *
 * @return mcsTRUE if the object has been fully initialized, mcsFALSE otherwise.
 */
mcsLOGICAL sdbENTRY::IsInit()
{
    logTrace("sdbENTRY::IsInit()");

    return _initSucceed;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/

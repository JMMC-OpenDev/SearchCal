/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of sdbENTRY class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sdbENTRY.cpp,v 1.3 2011-03-01 12:33:56 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
#include <string.h>
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
 * Static members definition 
 */


/**
 * Class constructor
 */
sdbENTRY::sdbENTRY()
{
    thrdMutexInit(&_mutex);
    _isNewMessage = mcsFALSE;
    memset(_buffer, '\0', sizeof(mcsSTRING256));
}

/**
 * Class destructor
 */
sdbENTRY::~sdbENTRY()
{
    thrdMutexDestroy(&_mutex);
}


/*
 * Public methods
 */

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

    // Check parameter
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }
    
    logDebug("Enter critical section for writing");
    if (thrdMutexLock(&_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Copy the new message to the internal buffer
    strncpy(_buffer, message, sizeof(mcsSTRING256));
    _isNewMessage = mcsTRUE;

    logDebug("Exit critical section after writing");
    if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Read a message from the entry.
 *
 * @param message an already allocated buffer to return the entry content.
 * @param waitNewMessage if mcsTRUE wait for a new message to be posted,
 * otherwise return the current one.
 * @param timeoutInSec if not -1, wait for a new message until the specified
 * amount of seconds and return the current one, otherwise wait forever.
 *
 * @return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT sdbENTRY::Read(char*             message,
                             mcsLOGICAL        waitNewMessage,
                             mcsINT32          timeoutInSec)
{
    logTrace("sdbENTRY::Read()");

    // Check parameter
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }

    // Loop configuration
    int period;     /* Number of usec the program is suspended */
    int decrement;  /* Timer decremenent */
    int remTime;    /* Remaining time (in usec) before timeout expiration */
    // Wait forever
    if (timeoutInSec == -1)
    {
        remTime = 1000;
        period = 100000; // 100 ms
        decrement = 0;   // Remaining time will be never decremented
    }
    // Given time-out
    else
    {
        remTime = (timeoutInSec * 1000000);
        period = 100000; // 100 ms
        decrement = period;
    }

    // Loop
    mcsLOGICAL done=mcsFALSE;
    while (done == mcsFALSE)
    {            
        // Lock data access
        logDebug("Enter critical section for reading");
        if (thrdMutexLock(&_mutex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // If message has been changed
        if ((waitNewMessage == mcsFALSE) || (_isNewMessage == mcsTRUE))
        {
            // Return it
            strncpy(message, _buffer, sizeof(mcsSTRING256));
            _isNewMessage = mcsFALSE;

            done = mcsTRUE;
        }

        // Unlock data access
        logDebug("Exit critical section after reading");
        if (thrdMutexUnlock(&_mutex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Suspend process if no message was available
        if (done == mcsFALSE)
        {
            // Check if time-out has expired
            if (remTime <=0)
            {
                errAdd(sdbERR_TIMEOUT_EXPIRED);
                return mcsFAILURE;
            }

            // Supsend process
            logDebug("Sleeping %dms before restarting.", period / 1000);
            usleep(period);
            remTime -= decrement;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/

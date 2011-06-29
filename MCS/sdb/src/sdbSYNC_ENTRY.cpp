/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of sdbSYNC_ENTRY class.
 */


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
#include "sdbSYNC_ENTRY.h"
#include "sdbPrivate.h"
#include "sdbErrors.h"


/*
 * Static members dfinition 
 */


/**
 * Class constructor
 */
sdbSYNC_ENTRY::sdbSYNC_ENTRY()
{
    _emptyBufferSemaphore = 0;
    _fullBufferSemaphore  = 0;
    
    _initSucceed = mcsFALSE;
    _lastMessage = mcsFALSE;
}

/**
 * Class destructor
 */
sdbSYNC_ENTRY::~sdbSYNC_ENTRY()
{
}


/*
 * Public methods
 */
/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbSYNC_ENTRY::Init(void)
{
    // Static member initialization
    memset(_buffer, '\0', sizeof(_buffer));
    _lastMessage = mcsFALSE;

    if (Destroy() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Semaphores initialisation */
    if (thrdSemaphoreInit(&_emptyBufferSemaphore, 1) == mcsFAILURE)
    {
        _initSucceed = mcsFALSE;
        return mcsFAILURE;
    }
    if (thrdSemaphoreInit(&_fullBufferSemaphore, 0) == mcsFAILURE)
    {
        _initSucceed = mcsFALSE;
        return mcsFAILURE;
    }

    _initSucceed = mcsTRUE;
    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbSYNC_ENTRY::Destroy(void)
{
    if (_initSucceed == mcsTRUE)
    {
        /* Semaphores destruction */
        if (thrdSemaphoreDestroy(_emptyBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (thrdSemaphoreDestroy(_fullBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    _initSucceed = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbSYNC_ENTRY::Write(const char* message, const mcsLOGICAL lastMessage)
{
    logTrace("sdbSYNC_ENTRY::Write()");

    /* Verify parameter vailidity */
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }
    
    if (_initSucceed == mcsTRUE)
    {
        /* Wait for buffer emptyness */
        logDebug("Waiting for the buffer to be empty.");
        if (thrdSemaphoreWait(_emptyBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logDebug("The buffer has been emptied.");
    }
        
    logDebug("Storing the new message in the buffer.");
    _lastMessage = lastMessage;
    strncpy(_buffer, message, sizeof(_buffer));
    
    if (_initSucceed == mcsTRUE)
    {
        /* Signal that a new message has been posted */
        logDebug("Signals that the new message has been posted.");
        if (thrdSemaphoreSignal(_fullBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbSYNC_ENTRY::Wait(char* message, mcsLOGICAL* lastMessage)
{
    logTrace("sdbSYNC_ENTRY::Wait()");

    /* Verify parameter vailidity */
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }
    if (lastMessage == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "lastMessage");
        return mcsFAILURE;
    }

    if (_initSucceed == mcsTRUE)
    {
        /* Wait for a new message to be posted */
        logDebug("Waiting for a new message in the buffer.");
        if (thrdSemaphoreWait(_fullBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logDebug("A new message has been received in the buffer.");
    }
    
    logDebug("Giving back the new message.");
    *lastMessage = _lastMessage;
    strncpy(message, _buffer, sizeof(_buffer));

    if (_initSucceed == mcsTRUE)
    {
        /* Signal buffer emptyness */
        logDebug("Signals that the new message has been used.");
        if (thrdSemaphoreSignal(_emptyBufferSemaphore) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsLOGICAL sdbSYNC_ENTRY::IsInit()
{
    logTrace("sdbSYNC_ENTRY::IsInit()");

    return _initSucceed;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/

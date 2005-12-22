/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdbSYNC_ENTRY.cpp,v 1.2 2005-12-22 14:10:35 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/12/20 13:52:34  lafrasse
 * Added preliminary support for INTRA-process action log
 *
 ******************************************************************************/

/**
 * @file
 * Definition of sdbENTRY class.
 */

static char *rcsId="@(#) $Id: sdbSYNC_ENTRY.cpp,v 1.2 2005-12-22 14:10:35 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
 * Static members dfinition 
 */
thrdSEMAPHORE  sdbENTRY::_emptyBufferSemaphore = 0;
thrdSEMAPHORE  sdbENTRY::_fullBufferSemaphore = 0;
mcsSTRING256   sdbENTRY::_buffer;
mcsLOGICAL     sdbENTRY::_lastMessage = mcsFALSE;


/**
 * Class constructor
 */
sdbENTRY::sdbENTRY()
{
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
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbENTRY::Init(void)
{
    /* Semaphores initialisation */
    if (thrdSemaphoreInit(&_emptyBufferSemaphore, 1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (thrdSemaphoreInit(&_fullBufferSemaphore, 0) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Static member initialization
    memset(_buffer, '\0', sizeof(_buffer));
    _lastMessage = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbENTRY::Destroy(void)
{
    /* Semaphores initialisation */
    if (thrdSemaphoreDestroy(_emptyBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (thrdSemaphoreDestroy(_fullBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbENTRY::Write(const char* message, const mcsLOGICAL lastMessage)
{
    logTrace("sdbENTRY::Write()");

    /* Verify parameter vailidity */
    if (message == NULL)
    {
        errAdd(sdbERR_NULL_PARAM, "message");
        return mcsFAILURE;
    }

    /* Wait for buffer emptyness */
    logDebug("Waiting for the buffer to be empty.");
    if (thrdSemaphoreWait(_emptyBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logDebug("The buffer has been emptied.");
    
    logInfo("Storing the new message in the buffer.");
    _lastMessage = lastMessage;
    strncpy(_buffer, message, sizeof(_buffer));

    /* Signal that a new message has been posted */
    logDebug("Signals that the new message has been posted.");
    if (thrdSemaphoreSignal(_fullBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * !!! NOT YET DOCUMENTED cause THIS IMPLEMENTATION IS TEMPORARY !!!
 */
mcsCOMPL_STAT sdbENTRY::Wait(char* message, mcsLOGICAL* lastMessage)
{
    logTrace("sdbENTRY::Wait()");

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

    /* Wait for a new message to be posted */
    logDebug("Waiting for a new message in the buffer.");
    if (thrdSemaphoreWait(_fullBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logDebug("A new message has been received in the buffer.");
    
    logInfo("Giving back the new message.");
    *lastMessage = _lastMessage;
    strncpy(message, _buffer, sizeof(_buffer));

    /* Signal buffer emptyness */
    logDebug("Signals that the new message has been used.");
    if (thrdSemaphoreSignal(_emptyBufferSemaphore) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/

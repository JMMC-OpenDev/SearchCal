/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhHANDLER.C,v 1.2 2004-11-17 09:52:13 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
* gzins     17-Nov-2004  Used evhXXX_CALLBACK pointer instead of instance
*                        reference in order to fix bug related to the
*                        referencing of deleted callback instance.
*
*******************************************************************************/

/**
 * \file
 * Declaration of the evhHANDLER class
 */

static char *rcsId="@(#) $Id: evhHANDLER.C,v 1.2 2004-11-17 09:52:13 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
#include <list>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "msg.h"

/*
 * Local Headers 
 */
#include "evhHANDLER.h"
#include "evhPrivate.h"
#include "evhErrors.h"

/*
 * Class constructor
 */
evhHANDLER::evhHANDLER()
{
}

/*
 * Class destructor
 */
evhHANDLER::~evhHANDLER()
{
}

/*
 * Public methods
 */

/**
 * Add a callback for command.
 *
 * It adds the specified callback for the event defined by the given command
 * key.
 *
 * \return always SUCCESS.
 *
 * \sa evhCMD_KEY
 */
mcsCOMPL_STAT evhHANDLER::AddCallback(const evhCMD_KEY &key,
                                      evhCMD_CALLBACK &callback)
{
    logExtDbg("evhHANDLER::AddCallback()");

    // If event is already registered
    evhCALLBACK_LIST *cbList;
    cbList = Find(key);
    if (cbList != NULL)
    {
        // Add the new callback to the list
        evhCMD_CALLBACK  *newCallback = new evhCMD_CALLBACK(callback);
        if (cbList->AddAtTail(newCallback) == FAILURE)
        {
            return FAILURE;
        } 
    }
    // Else
    else
    {
        // Add it
        evhCMD_KEY       *newKey      = new evhCMD_KEY(key);
        evhCMD_CALLBACK  *newCallback = new evhCMD_CALLBACK(callback);
        evhCALLBACK_LIST *newCbList   = new evhCALLBACK_LIST();
        newCbList->AddAtTail(newCallback);
        _eventList.push_back(pair<evhKEY *, evhCALLBACK_LIST *>(newKey, newCbList));
    }
    // End if

    return SUCCESS;
}

/**
 * Add a callback for I/O stream.
 *
 * It adds the specified callback for the event defined by the given I/O
 * stream key.
 *
 * \return always SUCCESS.
 *
 * \sa evhIOSTREAM_KEY
 */
mcsCOMPL_STAT evhHANDLER::AddCallback(const evhIOSTREAM_KEY &key,
                                      evhIOSTREAM_CALLBACK &callback)
{
    logExtDbg("evhHANDLER::AddCallback()");

    // If event is already registered
    evhCALLBACK_LIST *cbList;
    cbList = Find(key);
    if (cbList != NULL)
    {
        // Add the new callback to the list
        evhIOSTREAM_CALLBACK  *newCallback = new evhIOSTREAM_CALLBACK(callback);
        if (cbList->AddAtTail(newCallback) == FAILURE)
        {
            return FAILURE;
        } 
    }
    // Else
    else
    {
        // Add it
        evhIOSTREAM_KEY      *newKey      = new evhIOSTREAM_KEY(key);
        evhIOSTREAM_CALLBACK *newCallback = new evhIOSTREAM_CALLBACK(callback);
        evhCALLBACK_LIST     *newCbList   = new evhCALLBACK_LIST();
        newCbList->AddAtTail(newCallback);
        _eventList.push_back(pair<evhKEY *, evhCALLBACK_LIST *>(newKey, newCbList));
    }
    // End if
    return SUCCESS;
}

mcsCOMPL_STAT evhHANDLER::MainLoop(void)
{
    logExtDbg("evhHANDLER::MainLoop()");

    // For ever
    for(;;)
    {
        evhKEY *key;

        // Wait for event
        key = Select();

        // If an error occured
        if (key == NULL)
        {
            // Stop loop
            return FAILURE;
        }
        // Else if a command received 
        else if (key->GetType() == evhTYPE_COMMAND)
        {
            // Read message
            msgMESSAGE msg;
            if (msgReceive(&msg, 0) == FAILURE)
            {
                return FAILURE;
            }

            // Build event key
            ((evhCMD_KEY *)key)->SetCommand(msgGetCommand(&msg));

            // Run attached callback
            if (Run(*(evhCMD_KEY *)key, msg) == FAILURE)
            {
                return FAILURE;
            }

            // If EXIT command received
            if (strcmp(msgGetCommand(&msg), "EXIT") == 0)
            {
                return SUCCESS;
            }
        }        
        // Else if a I/O stream received 
        else if (key->GetType() == evhTYPE_IOSTREAM)
        {
                        // Run attached callback
            if (Run(*(evhIOSTREAM_KEY *)key, ((evhIOSTREAM_KEY *)key)->GetSd()) == FAILURE)
            {
                return FAILURE;
            }
        }
    }

    return SUCCESS;
}


/**
 * Look up a key object in the map.
 * 
 * This method returns an pointer to the callback list corresponding to the
 * given key object \em key. If key object is not found, a NULL pointer is
 * returned.
 *
 * \param key key object of the searched element.
 *
 * \return pointer to the callback list if key is found, NULL otherwise.
 */
evhCALLBACK_LIST *evhHANDLER::Find(const evhKEY &key)
{
    logExtDbg("evhHANDLER::Find()");

     // For each registered event
    std::list<std::pair<evhKEY *, evhCALLBACK_LIST *> >::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->IsSame(key))
        {
            // Return callback list pointer 
            return ((*iter).second);
        }
        // End if
    }
    // End for

   return NULL;
}

mcsCOMPL_STAT evhHANDLER::Run(const evhCMD_KEY &key, msgMESSAGE &msg)
{
    logExtDbg("evhHANDLER::Run()");

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->Match(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->Run(msg) == FAILURE)
            {
                return FAILURE;
            } 
        }
        // End if
    }
    // End for
    return SUCCESS;
}

mcsCOMPL_STAT evhHANDLER::Run(const evhIOSTREAM_KEY &key, int fd)
{
    logExtDbg("evhHANDLER::Run()");

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->Match(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->Run(fd) == FAILURE)
            {
                return FAILURE;
            } 
        }
        // End if
    }
    // End for
    return SUCCESS;
}

/**
 * Wait for events.
 * 
 * This method performs an asynchroneous wait for events. It returns whenever
 * a new message is received or a stream descriptor attached (see
 * evhIOSTREAM_KEY) is ready.
 */
evhKEY *evhHANDLER::Select()
{
    logExtDbg("evhHANDLER::Select()");

    fd_set      readMask;
    
    mcsINT32 msgQueueSd;
    mcsINT32 nbOfSds = 0;
    mcsINT32 sds[evhMAX_NO_OF_SDS];
    mcsINT32 maxSd = 0;

    // Get the socket descriptor for the message queue
    msgQueueSd = msgGetMessageQueue();

    // Get the list of listened I/O streams
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is an I/O stream event 
        if (((*iter).first)->GetType() == evhTYPE_IOSTREAM)
        {
            // Add the stream descriptor to the list
            sds[nbOfSds] = ((evhIOSTREAM_KEY *)((*iter).first))->GetSd();
            nbOfSds++;
        }
        // End if
    }
    // End for
    
    // If there is no I/O to listen
    if ((msgQueueSd == -1) && (nbOfSds == 0))
    {
        // Raise an error
        errAdd(evhERR_NO_STREAM_TO_LISTEN);

        // Return an error code
        return NULL;
    }
    // End if

    // Set the set of descriptors for reading
    FD_ZERO(&readMask);
    // Message queue
    if (msgQueueSd != -1)
    {
        FD_SET(msgQueueSd, &readMask);
        maxSd = msgQueueSd + 1;
    }
    // I/O streams
    for (int sd=0; sd < nbOfSds; sd++)
    {
        FD_SET(sds[sd], &readMask);
        maxSd = mcsMAX(maxSd, sds[sd] + 1);
    }

    // Wait for message
    int status;
    status = select(maxSd, &readMask, NULL, NULL, (struct timeval *)NULL);

    // If an error occured during select()
    if (status == -1)
    {
        // Raise an error
        errAdd(evhERR_SELECT, strerror(errno));

        // Return an error code
        return NULL;
    }
 
    // If a message is received from message manager 
    if ((msgQueueSd != -1) && (FD_ISSET(msgQueueSd , &readMask)))
    {
        logTest("Message received...");
        return &(_cmdEvent.SetCommand(mcsNULL_CMD));
    }
    else
    {
        for (int sd=0; sd < nbOfSds; sd++)
        {
            if (FD_ISSET(sds[sd], &readMask))
            {
                return &(_iostreamEvent.SetSd(sds[sd]));
            }
        }
    }
    return NULL;
}


/*___oOo___*/

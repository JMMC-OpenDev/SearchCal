/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhHANDLER.cpp,v 1.4 2004-12-22 08:59:02 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
* gzins     17-Nov-2004  Used evhXXX_CALLBACK pointer instead of instance
*                        reference in order to fix bug related to the
*                        referencing of deleted callback instance.
* gzins     18-Nov-2004  Updated main loop to accept message as argument.
*                        Returned error when no callback is attached to the
*                        received command.
* gzins     08-Dec-2004  Added some method documentation
* gzins     08-Dec-2004  Added purge of events whith no more callback attached
* gzins     22-Dec-2004  Implemented GetHelp()
*
*******************************************************************************/

/**
 * \file
 * Declaration of the evhHANDLER class
 */

static char *rcsId="@(#) $Id: evhHANDLER.cpp,v 1.4 2004-12-22 08:59:02 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
#include <list>
using namespace std;
#include <errno.h>

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
#include "evhHELP_CMD.h"
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
 * \return SUCCESS, or FAILURE if an error occurs.
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

/**
 * Handle the events main loop.
 *
 * An application calls this function when it is ready to handle events,
 * usually in the main() function after initialization of the application's
 * object.
 *
 * Inside this function events are received and callbacks dispatched.
 * Whenever a callback returns, execution is resumed by the MainLoop() that is
 * ready to handle the following event.
 * The function can return only if:
 *   - a callback has returned with an error condition, rising the
 *     evhCB_FAILURE bit in its return code.
 *     In this case MainLoop() returns FAILURE.
 *   - an internal error occurred in the handling of events.
 *     In this case MainLoop() returns FAILURE
 *
 * If the callback returns with the DELETE flag up, it is always deleted, no
 * regard of the other flags. If the callback returns with the FAILURE and/or
 * RETURN flag up, no other callback in the callback lists is called, but the
 * main loop exit immediately. 
 *    
 * At the end of every callback and before waiting for the next event, the
 * method checks also that the error stack is clean and empty.  If a callback
 * is returned with no error code and the error stack is not clean, this means
 * that some error has occurred in the callback, but it has not been properly
 * handled.  In this case the main loop logs an error and close the otherwise
 * pending error stack.
 *
 * The programmer should check for these errors to identify bad error handling
 * in the code.
 *
 * \return SUCCESS or FAILURE (see above).
 */
mcsCOMPL_STAT evhHANDLER::MainLoop(msgMESSAGE *msg)
{
    logExtDbg("evhHANDLER::MainLoop()");

    // If a message is given and it the command is not empty
    if ((msg != NULL) && (strlen(msg->GetCommand()) != 0))
    {
        // Build event key
        evhCMD_KEY key;
        key.SetCommand(msg->GetCommand());

        // Run attached callback
        if (Run(key, *msg) == FAILURE)
        {
            return FAILURE;
        }

        return SUCCESS;
    }
    // Else enter in main loop
    else
    {
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
                if (_msgManager.Receive(msg, 0) == FAILURE)
                {
                    return FAILURE;
                }

                // Build event key
                ((evhCMD_KEY *)key)->SetCommand(msg.GetCommand());

                // Run attached callbacks
                if (Run(*(evhCMD_KEY *)key, msg) == FAILURE)
                {
                    return FAILURE;
                }

                // If EXIT command received
                if (strcmp(msg.GetCommand(), "EXIT") == 0)
                {
                    return SUCCESS;
                }
            }        
            // Else if a I/O stream received 
            else if (key->GetType() == evhTYPE_IOSTREAM)
            {
                // Run attached callback
                if (Run(*(evhIOSTREAM_KEY *)key, 
                        ((evhIOSTREAM_KEY *)key)->GetSd()) == FAILURE)
                {
                    return FAILURE;
                }
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

/**
 * Executes all the callbacks attached to the command.
 *
 * Executes all the callbacks in the list which have been attached to the
 * command (see AddCallback()) identified by the given \em key. They are
 * executed in the same order they where inserted in the list.
 * The received messsage is passed as argument to the callaback.
 * 
 * If a callback returns with the evhCB_DELETE bit set, it deletes the
 * callback.
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method returns
 * immediately, i.e. the remaining callbacks in the list are not executed.
 *           
 * \return SUCCESS or FAILURE (see above).
 */
mcsCOMPL_STAT evhHANDLER::Run(const evhCMD_KEY &key, msgMESSAGE &msg)
{
    logExtDbg("evhHANDLER::Run()");

    // Number of callback attached to the received command 
    mcsINT32 counter=0;

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->Match(key))
        {
            counter++;

            // Execute callback of the list
            if (((*iter).second)->Run(msg) == FAILURE)
            {
                return FAILURE;
            } 
        }
        // End if
    }
    // End for

    // Purge events where there is no more callback
    iter = _eventList.begin();
    while (iter != _eventList.end())
    {
        // If the callback list is empty 
        if (((*iter).second)->IsEmpty() == mcsTRUE)
        {
            // Remove it, and restart at the beginning of the list
            _eventList.erase(iter);
            iter = _eventList.begin();
        }
        else
        {
            iter++;
        }
    }

    // If there is no callback attached to this command
    if (counter == 0)
    {
        errAdd(evhERR_CMD_UNKNOWN, msg.GetCommand());
        if (_msgManager.SendReply(msg, mcsTRUE)== FAILURE)
        {
            return FAILURE;
        } 
    }

    return SUCCESS;
}

/**
 * Executes all the callbacks attached to the I/O.
 *
 * Executes all the callbacks in the list which have been attached to the
 * I/O stream (see AddCallback()) identified by the given \em key. They are
 * executed in the same order they where inserted in the list.
 * The I/O descriptor is passed as argument to the callaback.
 * 
 * If a callback returns with the evhCB_DELETE bit set, it deletes the
 * callback.
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method returns
 * immediately, i.e. the remaining callbacks in the list are not executed.
 *           
 * \return SUCCESS or FAILURE (see above).
 */
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
 * a new message is received or a stream descriptor (see evhIOSTREAM_KEY) is
 * ready.
 *
 * \return key corresponding to the received event or NULL if an error occurs.
 */
evhKEY *evhHANDLER::Select()
{
    logExtDbg("evhHANDLER::Select()");

    fd_set   readMask;
    
    mcsINT32 msgQueueSd = -1;
    mcsINT32 nbOfSds = 0;
    mcsINT32 sds[evhMAX_NO_OF_SDS];
    mcsINT32 maxSd = 0;

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
    if ((_msgManager.IsConnected() == mcsFALSE) && (nbOfSds == 0))
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
    if (_msgManager.IsConnected() == mcsTRUE)
    {
        // Get the socket descriptor for the message queue
        msgQueueSd = _msgManager.GetMsgQueue();

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
    if ((_msgManager.IsConnected() == mcsTRUE) && 
        (FD_ISSET(msgQueueSd , &readMask)))
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

/**
 */
mcsCOMPL_STAT evhHANDLER::GetHelp(msgMESSAGE &msg)
{
    // Command instance
    evhHELP_CMD helpCmd(msg.GetCommand(), msg.GetBody());
    
    // Parse command parameters
    if (helpCmd.Parse() == FAILURE)
    {
        return FAILURE;
    }
    
    // Clear message body
    msg.ClearBody();
    
    // If a command name is given
    if (helpCmd.IsDefinedCommand() == mcsTRUE)
    {
        // Get the command name 
        char *command;
        if (helpCmd.GetCommand(&command)== FAILURE)
        {
            return FAILURE;
        }
        
        // Look for this command 
        mcsLOGICAL found=mcsFALSE;
        std::list<std::pair<evhKEY *, evhCALLBACK_LIST *> >::iterator iter;
        for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
        {
            char *registerCommand;
            if (((*iter).first)->GetType() == evhTYPE_COMMAND)
            {
                registerCommand = ((evhCMD_KEY *)((*iter).first))->GetCommand();
                if (strcmp(registerCommand, command) == 0)
                {
                    char *cdf;
                    cdf = ((evhCMD_KEY *)((*iter).first))->GetCdf();
                    if (strlen(cdf) != 0)
                    {
                        string desc;
                        cmdCOMMAND cmd(command, "", cdf);
                        if (cmd.GetDescription(desc) == FAILURE)
                        {
                            return FAILURE;
                        }
                        msg.AppendToBody(desc.c_str(), strlen(desc.c_str()));
                        msg.AppendToBody("\n");
                    }
                    else
                    {
                        msg.AppendToBody(command, strlen(command));
                        msg.AppendToBody(" - no help available\n");
                    }
                    found = mcsTRUE;
                }
            }
        }
        if (found == mcsFALSE)
        {
            msg.AppendToBody(command, strlen(command));
            msg.AppendToBody(" - not registered\n");
        }
    }
    else 
    {
        // For each registered event
        std::list<std::pair<evhKEY *, evhCALLBACK_LIST *> >::iterator iter;
        for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
        {
            if (((*iter).first)->GetType() == evhTYPE_COMMAND)
            {
                char *command;
                char *cdf;
                command = ((evhCMD_KEY *)((*iter).first))->GetCommand();
                cdf = ((evhCMD_KEY *)((*iter).first))->GetCdf();
                printf("command = %s\n", command); 
                if (strlen(cdf) != 0)
                {
                    string desc;
                    cmdCOMMAND cmd(command, "", cdf);
                    if (cmd.GetShortDescription(desc) == FAILURE)
                    {
                        return FAILURE;
                    }
                    msg.AppendToBody(desc.c_str(), strlen(desc.c_str()));
                    msg.AppendToBody("\n", 1);
                }
                else
                {
                    msg.AppendToBody(command, strlen(command));
                    msg.AppendToBody(" - no help available\n", 21);
                }
            }
        }
        msg.AppendToBody("\0", 1);
    }
    
    // End for
    return SUCCESS;
}

/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhHANDLER.cpp,v 1.11 2006-05-11 13:04:18 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/12/14 23:09:51  gzins
 * Fixed infinite loop bug when execution of command given in command line failed
 *
 * Revision 1.9  2005/05/19 15:15:20  gzins
 * Added handling of message queue when looking for messages
 *
 * Revision 1.8  2005/02/09 16:28:28  lafrasse
 * Reflected function renaming in 'msg' (GetMsgQueue became GetSocketDescriptor)
 *
 * Revision 1.7  2005/02/03 06:57:01  gzins
 * Updated HandleHelpCmd to format returned reply when short description is requested
 *
 * Revision 1.6  2005/01/26 18:27:22  gzins
 * Handled timeout for callback related to command reply.
 *
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
 * gzins     07-Jan-2005  Changed SUCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *                        Implemented AddCallback() and Run() for command reply
 *                        Updated MainLoop to support cammnd reply event
 *                        Added evhMainHandler global variable
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of the evhHANDLER class
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: evhHANDLER.cpp,v 1.11 2006-05-11 13:04:18 mella Exp $";
/* 
 * System Headers 
 */
#include <iostream>
#include <list>
using namespace std;
#include <time.h>
#include <sys/time.h>
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
 * Gloabl variables
 */
evhHANDLER *evhMainHandler = NULL;

/*
 * Class constructor
 */
evhHANDLER::evhHANDLER() : _msgEvent(evhTYPE_MESSAGE)
{
    if (evhMainHandler == NULL)
    {
        evhMainHandler = this;
    }
}

/*
 * Class destructor
 */
evhHANDLER::~evhHANDLER()
{
    if (evhMainHandler == this)
    {
        evhMainHandler = NULL;
    }
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
 * \return mcsSUCCESS, or mcsFAILURE if an error occurs. 
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
        if (cbList->AddAtTail(newCallback) == mcsFAILURE)
        {
            return mcsFAILURE;
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
        _eventList.push_back(pair<evhKEY *,
                             evhCALLBACK_LIST *>(newKey, newCbList));
    }
    // End if

    return mcsSUCCESS;
}

/**
 * Add a callback for command reply.
 *
 * It adds the specified callback for the event defined by the given command
 * reply key.
 *
 * \return mcsSUCCESS, or mcsFAILURE if an error occurs. 
 *
 * \sa evhCMD_KEY
 */
mcsCOMPL_STAT evhHANDLER::AddCallback(const evhCMD_REPLY_KEY &key,
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
        if (cbList->AddAtTail(newCallback) == mcsFAILURE)
        {
            return mcsFAILURE;
        } 
    }
    // Else
    else
    {
        // Add it
        evhCMD_REPLY_KEY *newKey      = new evhCMD_REPLY_KEY(key);
        evhCMD_CALLBACK  *newCallback = new evhCMD_CALLBACK(callback);
        evhCALLBACK_LIST *newCbList   = new evhCALLBACK_LIST();
        newCbList->AddAtTail(newCallback);
        _eventList.push_back(pair<evhKEY *,
                             evhCALLBACK_LIST *>(newKey, newCbList));
    }
    // End if

    return mcsSUCCESS;
}

/**
 * Add a callback for I/O stream.
 *
 * It adds the specified callback for the event defined by the given I/O
 * stream key.
 *
 * \return mcsSUCCESS, or mcsFAILURE if an error occurs.
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
        if (cbList->AddAtTail(newCallback) == mcsFAILURE)
        {
            return mcsFAILURE;
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
        _eventList.push_back(pair<evhKEY *,
                             evhCALLBACK_LIST *>(newKey, newCbList));
    }
    // End if
    return mcsSUCCESS;
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
 *     In this case MainLoop() returns mcsFAILURE.
 *   - an internal error occurred in the handling of events.
 *     In this case MainLoop() returns mcsFAILURE
 *
 * If the callback returns with the DELETE flag up, it is always deleted, no
 * regard of the other flags. If the callback returns with the mcsFAILURE and/or
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
 * \return mcsSUCCESS or mcsFAILURE (see above).
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
        if (Run(key, *msg) == mcsFAILURE)
        {
            errCloseStack();
            exit(EXIT_FAILURE);
        }

        return mcsSUCCESS;
    }
    // Else enter in main loop
    else
    {
        // For ever
        for(;;)
        {
            evhKEY *key;

            // Wait for event
            // If message queue is not empty
            if (_msgManager.GetNbQueuedMessages() != 0)
            {
                // Handle queued message 
                key = &_msgEvent;
            }
            else
            {
                // Wait for new event
                key = Select();
            }

            // If an error occured
            if (key == NULL)
            {
                // Stop loop
                return mcsFAILURE;
            }
            // Else if a timeout is expired. Select() returns a 'command reply'
            // key when a timeout, associated to a command, expired.
            else if (key->GetType() == evhTYPE_COMMAND_REPLY)
            {
                // Add error 
                errAdd(evhERR_TIMEOUT_EXPIRED,
                       ((evhCMD_REPLY_KEY *)key)->GetCommand());
                
                // Build message with 
                msgMESSAGE msg;
                msg.SetCommand(((evhCMD_REPLY_KEY *)key)->GetCommand());
                msg.SetCommandId(((evhCMD_REPLY_KEY *)key)->GetCommandId());
                msg.SetType(msgTYPE_ERROR_REPLY);
                
                // Run attached callbacks
                if (Run(*(evhCMD_REPLY_KEY *)key, msg) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // Else if a message (command or reply) received 
            else if (key->GetType() == evhTYPE_MESSAGE)
            {
                // Read message
                msgMESSAGE msg;
                // From queue, if there are queued messages
                if (_msgManager.GetNbQueuedMessages() != 0)
                {
                    if (_msgManager.GetNextQueuedMessage(msg) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
                // or from network otherwise
                else
                {
                    if (_msgManager.Receive(msg, 0) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }

                // If message is a command
                if (msg.GetType() == msgTYPE_COMMAND)
                {
                    // Build event key
                    evhCMD_KEY cmdEvent(msg.GetCommand());

                    // Run attached callbacks
                    if (Run(cmdEvent, msg) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }

                    // If EXIT command received
                    if (strcmp(msg.GetCommand(), "EXIT") == 0)
                    {
                        return mcsSUCCESS;
                    }
                }
                // else (reply)
                else
                {
                    // Build event key
                    evhCMD_REPLY_KEY cmdReplyEvent(msg.GetCommand(),
                                                   msg.GetCommandId());
                    // Run attached callbacks
                    if (Run(cmdReplyEvent, msg) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
            }        
            // Else if a I/O stream received 
            else if (key->GetType() == evhTYPE_IOSTREAM)
            {
                // Run attached callback
                if (Run(*(evhIOSTREAM_KEY *)key, 
                        ((evhIOSTREAM_KEY *)key)->GetSd()) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Handle HELP command.
 * 
 * It retrieves the short description of the commands supported by the
 * application, or the description of a given command. It recognizes the
 * following command parameter:
 *   \li command \em &lt;command&gt;  specifies the command name to get detailed
 *        help on
 *
 * \return mcsSUCCESS, or mcsFAILURE if an error occurs.
 */
mcsCOMPL_STAT evhHANDLER::HandeHelpCmd(msgMESSAGE &msg)
{
    // Command instance
    evhHELP_CMD helpCmd(msg.GetCommand(), msg.GetBody());
    
    // Parse command parameters
    if (helpCmd.Parse() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Clear message body
    msg.ClearBody();
    
    // If a command name is given
    if (helpCmd.IsDefinedCommand() == mcsTRUE)
    {
        // Get the command name 
        char *command;
        if (helpCmd.GetCommand(&command)== mcsFAILURE)
        {
            return mcsFAILURE;
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
                    cmdCOMMAND cmd(command, "", cdf);
                    string desc;
                    if (cmd.GetDescription(desc) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    msg.AppendStringToBody(desc.c_str());
                    msg.AppendStringToBody("\n");
                    found = mcsTRUE;
                }
            }
        }
        if (found == mcsFALSE)
        {
            msg.AppendStringToBody(command);
            msg.AppendStringToBody(" - not registered\n");
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
                mcsSTRING256 cmdShortDescription;
                command = ((evhCMD_KEY *)((*iter).first))->GetCommand();
                cdf = ((evhCMD_KEY *)((*iter).first))->GetCdf();
                string desc;
                cmdCOMMAND cmd(command, "", cdf);
                if (cmd.GetShortDescription(desc) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                sprintf(cmdShortDescription, "%10s - %s\n", 
                        command, desc.c_str());
                msg.AppendStringToBody(cmdShortDescription);
            }
        }
    }
    
    // End for
    return mcsSUCCESS;
}

/*
 * Protected methods
 */

/**
 * Wait for events.
 * 
 * This method performs an asynchroneous wait for events. It returns whenever
 * a new message is received or a stream descriptor (see evhIOSTREAM_KEY) is
 * ready. It returns a key corresponding to the received event:
 * \li evhTYPE_MESSAGE event key if a message has been received (either command
 * or reply)
 * \li evhTYPE_IOSTREAM event key if an I/O stream is ready for reading
 * \li evhTYPE_COMMAND_REPLY event key if a timeout is expired
 *
 * \return key corresponding to the received event or NULL if an error occurs.
 */
evhKEY *evhHANDLER::Select()
{
    logExtDbg("evhHANDLER::Select()");

    fd_set   readMask, refReadMask;
    
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
    FD_ZERO(&refReadMask);
    // Message queue
    if (_msgManager.IsConnected() == mcsTRUE)
    {
        // Get the socket descriptor for the message queue
        msgQueueSd = _msgManager.GetSocketDescriptor();

        FD_SET(msgQueueSd, &refReadMask);
        maxSd = msgQueueSd + 1;
    }

    // I/O streams
    for (int sd=0; sd < nbOfSds; sd++)
    {
        FD_SET(sds[sd], &refReadMask);
        maxSd = mcsMAX(maxSd, sds[sd] + 1);
    }

     
    // Wait for message
    int status;
    do
    {
        // Accuracy for timer events; i.e. frequency at which the event handler
        // look for timer events. 
        struct timeval timeout ;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100 ms
        readMask = refReadMask;
        status = select(maxSd, &readMask, NULL, NULL, &timeout);
        
        // If no event is arrived 
        if (status == 0)
        {
            // Check for timeout
            evhCMD_REPLY_KEY *timeOutReplyKey;
            timeOutReplyKey = CheckForTimeout();
            if (timeOutReplyKey != NULL)
            {
                return timeOutReplyKey;
            }
        }
    } while (status == 0);

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
        (FD_ISSET(msgQueueSd, &readMask)))
    {
        logTest("Message received...");
        return &(_msgEvent);
    }
    // Else
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
    // End if

    return NULL;
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
 * Executes all the callbacks in the list (see evhCALLBACK_LIST::Run() method).
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method send reply
 * to sender process or (if it is an internal message) close the error stack,
 * and then returns immediately, i.e. the remaining callbacks in the list are
 * not executed.
 *           
 * \return mcsSUCCESS or mcsFAILURE (see above).
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
            if (((*iter).second)->Run(msg) == mcsFAILURE)
            {
                return mcsFAILURE;
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
        if (_msgManager.SendReply(msg, mcsTRUE) == mcsFAILURE)
        {
            return mcsFAILURE;
        } 
    }

    return mcsSUCCESS;
}

/**
 * Executes all the callbacks attached to the command reply.
 *
 * Executes all the callbacks in the list (see evhCALLBACK_LIST::Run() method).
 * 
 * \return mcsSUCCESS or mcsFAILURE.
 */
mcsCOMPL_STAT evhHANDLER::Run(const evhCMD_REPLY_KEY &key, msgMESSAGE &msg)
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
            if (((*iter).second)->Run(msg) == mcsFAILURE)
            {
                return mcsFAILURE;
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

    // If there is no callback attached to this reply
    if (counter == 0)
    {
        logWarning("Received undesired command reply for %s command.",
                   msg.GetCommand());
    }

    return mcsSUCCESS;
}

/**
 * Executes all the callbacks attached to the I/O.
 *
 * Executes all the callbacks in the list (see evhCALLBACK_LIST::Run() method).
 *           
 * \return mcsSUCCESS or mcsFAILURE.
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
            // Execute the callback 
            if (((*iter).second)->Run(fd) == mcsFAILURE)
            {
                return mcsFAILURE;
            } 
        }
        // End if
    }
    // End for
    return mcsSUCCESS;
}

/**
 * Check expiration date of timeout.
 *
 * It scans the register event list, and check, for the command reply events, if
 * the timeout is expired. If yes, it returns the pointer to this event. If
 * there is no expired timeout, NULL is returned. 
 * 
 * \return pointer to event whose timeout is expired, or NULL if no timeout
 * expired.
 */
evhCMD_REPLY_KEY *evhHANDLER::CheckForTimeout()
{
    // For each registered event
    std::list<std::pair<evhKEY *, evhCALLBACK_LIST *> >::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If it is a command reply event
        if (((*iter).first)->GetType() == evhTYPE_COMMAND_REPLY)
        {
            // It a timeout has been specified
            if (((evhCMD_REPLY_KEY *)((*iter).first))->GetTimeout() !=
                msgWAIT_FOREVER)
            {
                // Get the expiration date
                struct timeval expDate;
                ((evhCMD_REPLY_KEY *)((*iter).first))->GetTimeoutExpDate(&expDate);
                // Get the system time
                struct timeval  time;
                gettimeofday(&time, NULL);

                // Compare the expiration date and system time
                if ((time.tv_sec > expDate.tv_sec) ||
                    ((time.tv_sec == expDate.tv_sec) &&
                     (time.tv_usec >= expDate.tv_usec)))
                {
                    return  ((evhCMD_REPLY_KEY *)((*iter).first));
                }
            }
        }
    }
    return NULL;
}

/*___oOo___*/

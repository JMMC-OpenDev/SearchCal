/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhINTERFACE.cpp,v 1.5 2005-02-15 13:39:03 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/01/29 06:47:26  gzins
 * Fixed wrong prototype of Forward method
 *
 * Revision 1.3  2005/01/27 17:52:57  gzins
 * Implemented command reply callback: ReplyCB()
 *
 * Revision 1.2  2005/01/26 18:18:08  gzins
 * Added timeout when attaching callback for command reply.
 *
 * gzins     06-Jan-2005  Created
 *
 ******************************************************************************/

/**
 * \file
 * evhINTERFACE class definition.
 */

static char *rcsId="@(#) $Id: evhINTERFACE.cpp,v 1.5 2005-02-15 13:39:03 gzins Exp $"; 
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
#include "evhHANDLER.h"
#include "evhINTERFACE.h"
#include "evhCMD_REPLY_KEY.h"
#include "evhPrivate.h"
#include "evhErrors.h"

/**
 * Class constructor
 */
evhINTERFACE::evhINTERFACE(const char *name, const char *procName, 
                           const mcsINT32 timeout)
{
    _name = name;
    strncpy(_procName, procName, sizeof(mcsPROCNAME));
    _timeout = timeout;
    _replyCb = NULL;
}

/**
 * Class destructor
 */
evhINTERFACE::~evhINTERFACE()
{
    if (_replyCb != NULL)
    {
        delete (_replyCb);
    }
}

/*
 * Public methods
 */
/**
 * Send a command message to a process.
 *
 * Send the \<command\> to the interface destination process and wait for the
 * reply. The command parameters (if any) has to be given in \<parameters\>.
 * This method returns an error if the timeout is expired during the waiting of
 * message reply, or if an error reply is received. The possible value of
 * timeout are:
 *   \li -2              : used default timeout defined in constructor,
 *   \li msgWAIT_FOREVER : wait forever for message reply
 *   \li msgNO_WAIT      : do not wait for message reply
 *   \li \<n\>             : wait 'n' seconds for message reply
 *
 * The reply is stored into the internal buffer and can be retrieved with
 * GetLastReply().
 *
 * \param command command name
 * \param parameters parameter list stored in a string
 * \param timeout time-out for waiting reply
 *
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT evhINTERFACE::Send(const char *command,
                                 const char *parameters,  
                                 mcsINT32   timeout)
{
    logExtDbg("evhINTERFACE::Send()");

    mcsINT32   cmdId;
    
    // Clear message used to get reply
    _msg.ClearBody();

    // Send the message
    cmdId = _msgManager.SendCommand(command, _procName, parameters);
    if (cmdId == -1)
    {
        return mcsFAILURE;
    }

    // If timeout is not given, use default one 
    if (timeout == -2)
    {
        timeout = _timeout;
    }

    // Do
    do
    {
        // If timeout is expired
        if (_msgManager.Receive(_msg, timeout) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // End if

        // If an error reply is received
        if (_msg.GetType() == msgTYPE_ERROR_REPLY)
        {
            // Retrieve error from message
            errUnpackStack(_msg.GetBody(), _msg.GetBodySize());
            errAdd (evhERR_REPLY, _name.c_str(), command);
            return mcsFAILURE;
        }
        // End if
    } while (_msg.IsLastReply() == mcsFALSE);
    // while the last reply is not received

    return mcsSUCCESS;
}

/**
 * Send asynchronously a command message to a process.
 *
 * Send the \<command\> to the interface destination process and install the
 * given \<callback\> for the reply. The command parameters (if any) has to be
 * given in \<parameters\>.
 * The possible value of timeout are:
 *   \li -2              : used default timeout defined with SetTimeout(),
 *   \li msgWAIT_FOREVER : wait forever for message reply
 *   \li msgNO_WAIT      : do not wait for message reply
 *   \li \<n\>             : wait 'n' seconds for message reply
 *
 * The reply message is passed to the method defined by the callback.
 *
 * \param command command name
 * \param parameters parameter list stored in a string
 * \param callback callback to be executed when reply is received
 * \param timeout time-out for waiting reply
 *
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT evhINTERFACE::Forward(const char *command,
                                    const char *parameters,  
                                    evhCMD_CALLBACK &callback,
                                    mcsINT32   timeout)
{
    logExtDbg("evhINTERFACE::Forward()");

    mcsINT32   cmdId;
    
    // This class rely on the standard event handler
    if (evhMainHandler == NULL)
    {
        errAdd (evhERR_NO_HANDLER_INSTANCE);
        return mcsFAILURE;
    }

    // Send the message
    cmdId = _msgManager.SendCommand(command, _procName, parameters);
    if (cmdId == -1)
    {
        return mcsFAILURE;
    }
    else
    {
        // If no timeout given, use the default one
        if (timeout == -2)
        {
            timeout = _timeout;
        }

        // Save user reply callback
        if (_replyCb != NULL)
        {
            delete (_replyCb);
        }
        _replyCb = new evhCMD_CALLBACK(callback);

        // Attach callback for reply
        evhCMD_REPLY_KEY key(command, cmdId, timeout);
        evhCMD_CALLBACK cmdReplyCB
            (this, (evhCMD_CB_METHOD)&evhINTERFACE::ReplyCB);
        if (evhMainHandler->AddCallback(key, cmdReplyCB) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * Callback attached to the command reply.
 * 
 * It is the callback which is attached to the reply of the command sent by
 * Forward() method. When reply is received, it unpack error message in error
 * stack if an erro occured during command execution, and then executes the
 * callback specified by in Forword().
 *
 * \param msg received message reply.
 *
 * \return return value of user callback. 
 */
evhCB_COMPL_STAT evhINTERFACE::ReplyCB(msgMESSAGE &msg, void*)
{
    logExtDbg("evhINTERFACE::ReplyCB");

    // If an error reply is received
    if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Unpack error stack
        errUnpackStack(msg.GetBody(), msg.GetBodySize());
    }
    
    // Then execute user callback 
    return (_replyCb->Run(msg));
}

/**
 * Get the last received reply 
 *
 * \return pointer the last received reply  
 */
const char *evhINTERFACE::GetLastReply(void)
{
    return (_msg.GetBody());
}

/*___oOo___*/

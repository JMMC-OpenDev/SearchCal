/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.cpp,v 1.8 2004-12-03 17:05:50 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for _socket,
*                        and refined comments
* lafrasse  22-Nov-2004  Use msgSOCKET_CLIENT instead of system socket calls.
* lafrasse  24-Nov-2004  Comment refinments, and includes cleaning
* gzins     29-Nov-2004  Fixed bug in Connect method
* lafrasse  01-Dec-2004  Comment refinments
* gzins     03-Dec-2004  Removed msgManagerHost param from Connect
*                        Minor changes in documentation 
* lafrasse  03-Dec-2004  Added mcs environment name management
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class definition.
 */

static char *rcsId="@(#) $Id: msgMANAGER_IF.cpp,v 1.8 2004-12-03 17:05:50 lafrasse Exp $"; 
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
#include "msgPrivate.h"
#include "msgErrors.h"
#include "msgMANAGER_IF.h"
#include "msgMCS_ENV.h"
#include "msgSOCKET_CLIENT.h"


/*
 * Static Class Members Initialization 
 */
msgSOCKET_CLIENT msgMANAGER_IF::_socket;


/*
 * Class constructor
 */
msgMANAGER_IF::msgMANAGER_IF()
{
}

/*
 * Class destructor
 */
msgMANAGER_IF::~msgMANAGER_IF()
{
}


/*
 * Public methods
 */

/**
 * Establish the connection with the MCS message service.
 *
 * The server host name is found via the $MCSENV environment variable, and the
 * mcsEnvList file (located in $MCSROOT/etc/mcsEnvList).
 *
 * \param procName the local processus name
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Connect (const mcsPROCNAME  procName)
{
    logExtDbg("msgMANAGER_IF::Connect()");

    // If a connection is already open...
    if (IsConnected() == mcsTRUE)
    {
        errAdd(msgERR_PROC_ALREADY_CONNECTED);
        return FAILURE;
    }

    // Initialize MCS services
    if (mcsInit(procName) == FAILURE)
    {
        errAdd(msgERR_MCSINIT);
        return FAILURE;
    }

    // Get the msgManager host name and port number
    msgMCS_ENV mcsEnv;
    logTest("'msgManager' server host name is '%s', listening on port '%d'",
            mcsEnv.GetHostName(), mcsEnv.GetPortNumber());

    // Establish the connection, retry otherwise...
    mcsINT32 nbRetry = 2;
    for(;;) 
    {
        // Connect to msgManager
        _socket.Open(mcsEnv.GetHostName(), mcsEnv.GetPortNumber());
        if (_socket.IsConnected() == mcsFALSE)
        {
            // If no more retry possible
            if (--nbRetry <= 0)
            { 
                errAdd(msgERR_CONNECT, strerror(errno));
                return FAILURE; 
            }
            else
            {
                logWarning("Cannot connect to 'msgManager'. Trying again...");
                sleep(1);
                _socket.Close();
            }
        }
        else // Else if the connection is established, leave the retry loop
        {
            break;
        }
    }

    // Register with msgManager
    if (SendCommand(msgREGISTER_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }
    
    // Wait for its answer
    msgMESSAGE registerAnswer;
    if (Receive(registerAnswer, 1000) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }

    // If the reply is an ERROR...
    if (registerAnswer.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Put the received errors in the MCS error stack
        if (errUnpackStack(registerAnswer.GetBodyPtr(),
                           registerAnswer.GetBodySize())
            == FAILURE)
        {
            return FAILURE;
        }

        _socket.Close();
        return FAILURE;
    }
    
    logExtDbg("Connection to 'msgManager' established");

    return SUCCESS;
}
 
/**
 * Return weither the connection to msgManger is up and running, or not.
 *
 * \return an MCS logical (TRUE if the connection is up and running, or FALSE
 * otherwise)
 */
mcsLOGICAL msgMANAGER_IF::IsConnected(void)
{
    logExtDbg("msgMANAGER_IF::IsConnected()");

    return (_socket.IsConnected());
}
 
/**
 * Send a command message to a process.
 *
 * Send the \<command\>  to the \<destProc\> named process. The command
 * parameters (if any) has to be given in \<paramList\>. The parameter list
 * length can be specified using \<paramLen\>, if it is not given then the
 * length of the parameter list string is used.
 *
 * \param command command name
 * \param destProc remote process name
 * \param paramList parameter list stored in a string
 * \param paramLen length of the parameter list string
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::SendCommand(const char        *command,
                                         const mcsPROCNAME  destProc,
                                         const char        *paramList,  
                                         const mcsINT32     paramLen)
{
    logExtDbg("msgMANAGER_IF::SendCommand()");

    msgMESSAGE msg;

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return FAILURE;
    }

    // Build the message header
    msg.SetSender(mcsGetProcName());
    msg.SetRecipient(destProc);
    msg.SetType(msgTYPE_COMMAND);
    msg.SetCommand(command);
 
    // Build the message body
    if (msg.SetBody((char*)paramList, paramLen) == FAILURE)
    {
        return FAILURE;
    }

    // Send the message
    return (_socket.Send(msg));
}

/**
 * Send a reply message.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current message is the last one or
 * not
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::SendReply           (msgMESSAGE        &msg,
                                                  mcsLOGICAL         lastReply)
{
    logExtDbg("msgMANAGER_IF::SendReply()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return FAILURE;
    }

    // Build the reply message header
    msg.SetLastReplyFlag(lastReply);
    // If there is no error in the MCS error stack
    if (errStackIsEmpty() == mcsTRUE)
    {
        // Set message type to REPLY
        msg.SetType(msgTYPE_REPLY);
    }
    else
    {
        // Put the MCS error stack data in the message body
        char errStackContent[msgBODYMAXLEN];
        if (errPackStack(errStackContent, msgBODYMAXLEN) == FAILURE)
        {
            return FAILURE;
        }

        // Store the message body size
        msg.SetBody(errStackContent, msgBODYMAXLEN);

        // Set message type to ERROR_REPLY
        msg.SetType(msgTYPE_ERROR_REPLY);

        // Empty MCS error stack
        errResetStack();
    }

    logTest("Sending '%s' answer : %s", msg.GetCommand(), msg.GetBodyPtr());
    return _socket.Send(msg);
}

/**
 * Receive a message.
 *
 * Wait for a message receive from 'msgManager'.\n
 * The \<timeoutInMs\> can have specific values : msgWAIT_FOREVER or msgNO_WAIT.
 *
 * \param msg an already allocated message structure pointer
 * \param timeoutInMs a time out value in milliseconds
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Receive     (msgMESSAGE        &msg,
                                          const mcsINT32     timeoutInMs)
{
    logExtDbg("msgMANAGER_IF::Receive()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return FAILURE;
    }

    // Return weither a message was received or not
    return (_socket.Receive(msg, timeoutInMs));
}

/**
 * Close the connection with the MCS message service.
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Disconnect(void)
{
    logExtDbg("msgMANAGER_IF::Disconnect()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return FAILURE;
    }

    // Send a 'close command' message to msgManager
    if (SendCommand(msgCLOSE_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }
    
    // Wait for its answer
    msgMESSAGE msg;
    if (Receive(msg, 1000) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }

    // If an error occured while deconnecting...
    if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Put the received error in the local MCS error stack
        errUnpackStack(msg.GetBodyPtr(), msg.GetBodySize());
        _socket.Close();
        return FAILURE;
    }
    
    // Close the socket
    _socket.Close();

    logExtDbg("Connection to 'msgManager' closed");

    return SUCCESS;
}
 
/**
 * Return the socket descriptor for the message queue
 *
 * This allows a process to monitor its message queue using the UNIX function
 * select().
 *
 * \warning
 * The file descriptor returned must NOT be read or manipulated in any way
 * (e.g. close()) by the process. Otherwise the monitoring system will lose
 * syncronization with the message manager.
 *
 * \return the socket descriptor of the communication link with msgManager
 */
mcsINT32 msgMANAGER_IF::GetMsgQueue()
{
    logExtDbg("msgMANAGER_IF::GetMsgQueue()");

    return _socket.GetDescriptor();
}
 

/*___oOo___*/

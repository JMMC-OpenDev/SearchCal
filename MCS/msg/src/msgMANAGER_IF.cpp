/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMANAGER_IF.cpp,v 1.19 2005-01-26 08:42:22 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * lafrasse  18-Nov-2004  Created
 * lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for
 *                        _socket, and refined comments
 * lafrasse  22-Nov-2004  Use msgSOCKET_CLIENT instead of system socket calls.
 * lafrasse  24-Nov-2004  Comment refinments, and includes cleaning
 * gzins     29-Nov-2004  Fixed bug in Connect method
 * lafrasse  01-Dec-2004  Comment refinments
 * gzins     03-Dec-2004  Removed msgManagerHost param from Connect
 *                        Minor changes in documentation 
 * lafrasse  03-Dec-2004  Added mcs environment name management
 * gzins     05-Dec-2004  Updated according to new msgMCS_ENV class API
 * gzins     06-Dec-2004  Renamed msgMCS_ENV to msgMCS_ENVS
 * gzins     07-Dec-2004  Updated according to the new msgMCS_ENVS class
 *                        Fixed bug related to error handling in Connect
 * gzins     08-Dec-2004  Replaced msgMCS_ENVS with envLIST
 * gzins     09-Dec-2004  Fixed bug related to default port number;
 *                        msgMANAGER_PORT_NUMBER used instead of the one given
 *                        by envLIST class
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer (no more msgMAXLEN)
 * gzins     15-Dec-2004  Used new command name definition (with _NAME)
 * gzins     22-Dec-2004  Replaced GetBodyPtr by GetBody 
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *                        Updated SendCommand to return command Id
 *
 ******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class definition.
 */

static char *rcsId="@(#) $Id: msgMANAGER_IF.cpp,v 1.19 2005-01-26 08:42:22 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <errno.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "env.h"
#include "misc.h"


/*
 * Local Headers 
 */
#include "msgPrivate.h"
#include "msgErrors.h"
#include "msgMANAGER_IF.h"
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
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Connect (const mcsPROCNAME  procName)
{
    logExtDbg("msgMANAGER_IF::Connect()");

    // If a connection is already open...
    if (IsConnected() == mcsTRUE)
    {
        errAdd(msgERR_PROC_ALREADY_CONNECTED);
        return mcsFAILURE;
    }

    // Initialize MCS services
    if (mcsInit(procName) == mcsFAILURE)
    {
        errAdd(msgERR_MCSINIT);
        return mcsFAILURE;
    }

    // Get the local host name
    mcsSTRING256 localHostName;
    if (miscGetHostName(localHostName, sizeof(mcsSTRING256)) != mcsSUCCESS)
    {
        return mcsFAILURE;
    }

    // Get the host name on which environment is running
    envLIST envList;
    const char *envHostName;
    envHostName = envList.GetHostName();
    if (envHostName == NULL)
    {
        return mcsFAILURE;
    }
    // Check the environment is local
    if (strcmp(localHostName, envHostName) != 0)
    {
        errAdd (msgERR_REMOTE_ENV, mcsGetEnvName(), envHostName);
        return mcsFAILURE;
    }
   
    // If an environment is defined
    mcsINT32 envPortNumber;
    if (strcmp (mcsGetEnvName(), mcsUNKNOWN_ENV) != 0)
    {
        // Get the host name on which environment is running
        const char *envHostName;
        envHostName = envList.GetHostName();
        
        // Check environment if defined
        if (envHostName == NULL)
        {
            return mcsFAILURE;
        }

        // Check the MCS environment is a local environment; i.e runing on the
        // local host.
        if (strcmp(localHostName, envHostName) != 0)
        {
            // errAdd (msgERR_REMOTE_ENV, mcsGetEnvName(), envHostName);
            return mcsFAILURE;
        }

        // Get msgManager connection port number
        envPortNumber = envList.GetPortNumber();
        
        // Check environment if defined
        if (envPortNumber == -1)
        {
            return mcsFAILURE;
        }
    }
    // Else
    else
    {
        // Use default port number
        envPortNumber = envList.GetPortNumber(); /* Definir */
    }
    // End if

    logTest("Connection to message manager in '%s' environment is '%d'",
            mcsGetEnvName(), envPortNumber);

    // Establish the connection, retry otherwise...
    mcsINT32 nbRetry = 2;
    for(;;) 
    {
        // Connect to msgManager
        if (_socket.Open(localHostName, envPortNumber) == mcsFAILURE)
        {
            // If no more retry possible
            if (--nbRetry <= 0)
            { 
                return mcsFAILURE; 
            }
            else
            {
                logWarning("Cannot connect to 'msgManager'. Trying again...");
                sleep(1);
                errResetStack();
                _socket.Close();
            }
        }
        else // Else if the connection is established, leave the retry loop
        {
            break;
        }
    }

    // Register with msgManager
    if (SendCommand(msgREGISTER_CMD_NAME, "msgManager", "") == mcsFAILURE)
    {
        _socket.Close();
        return mcsFAILURE;
    }
    
    // Wait for its answer
    msgMESSAGE registerAnswer;
    if (Receive(registerAnswer, 1000) == mcsFAILURE)
    {
        _socket.Close();
        return mcsFAILURE;
    }

    // If the reply is an ERROR...
    if (registerAnswer.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Put the received errors in the MCS error stack
        if (errUnpackStack(registerAnswer.GetBody(),
                           registerAnswer.GetBodySize())
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        _socket.Close();
        return mcsFAILURE;
    }
    
    logExtDbg("Connection to 'msgManager' established");

    return mcsSUCCESS;
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
 * \return command Id on successful completion or -1 if an error occurs
 */
mcsINT32 msgMANAGER_IF::SendCommand(const char        *command,
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
        return -1;
    }

    // Build the message header
    msg.SetSender(mcsGetProcName());
    msg.SetRecipient(destProc);
    msg.SetType(msgTYPE_COMMAND);
    msg.SetCommand(command);
 
    // Build the message body
    if (paramList != NULL)
    {
        if (msg.SetBody((char*)paramList, paramLen) == mcsFAILURE)
        {
            return -1;
        }
    }

    // Send the message
    if (_socket.Send(msg) == mcsFAILURE)
    {
        return -1;
    }

    // Return Id of the sent message
    return (msg.GetCommandId());
}

/**
 * Send a reply message.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current message is the last one or
 * not
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::SendReply           (msgMESSAGE        &msg,
                                                  mcsLOGICAL         lastReply)
{
    logExtDbg("msgMANAGER_IF::SendReply()");
    char *replyType;

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return mcsFAILURE;
    }

    // Build the reply message header
    msg.SetLastReplyFlag(lastReply);
    // If there is no error in the MCS error stack
    if (errStackIsEmpty() == mcsTRUE)
    {
        // Set message type to REPLY
        msg.SetType(msgTYPE_REPLY);
        replyType = "reply";
    }
    else
    {
        // Put the MCS error stack data in the message body
        char errStackContent[errSTACK_SIZE * errMSG_MAX_LEN];
        if (errPackStack(errStackContent, sizeof(errStackContent)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Store the message body size
        msg.SetBody(errStackContent, strlen(errStackContent) + 1);

        // Set message type to ERROR_REPLY
        msg.SetType(msgTYPE_ERROR_REPLY);
        replyType = "error reply";

        // Empty MCS error stack
        errResetStack();
    }

    logTest("Sending '%s' %s : %s", msg.GetCommand(), replyType, msg.GetBody());
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
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Receive     (msgMESSAGE        &msg,
                                          const mcsINT32     timeoutInMs)
{
    logExtDbg("msgMANAGER_IF::Receive()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return mcsFAILURE;
    }

    // Return weither a message was received or not
    return (_socket.Receive(msg, timeoutInMs));
}

/**
 * Close the connection with the MCS message service.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Disconnect(void)
{
    logExtDbg("msgMANAGER_IF::Disconnect()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_PROC_NOT_CONNECTED);
        return mcsFAILURE;
    }

    // Send a 'close command' message to msgManager
    if (SendCommand(msgCLOSE_CMD_NAME, "msgManager", "") == mcsFAILURE)
    {
        _socket.Close();
        return mcsFAILURE;
    }
    
    // Wait for its answer
    msgMESSAGE msg;
    if (Receive(msg, 1000) == mcsFAILURE)
    {
        _socket.Close();
        return mcsFAILURE;
    }

    // If an error occured while deconnecting...
    if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Put the received error in the local MCS error stack
        errUnpackStack(msg.GetBody(), msg.GetBodySize());
        _socket.Close();
        return mcsFAILURE;
    }
    
    // Close the socket
    _socket.Close();

    logExtDbg("Connection to 'msgManager' closed");

    return mcsSUCCESS;
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

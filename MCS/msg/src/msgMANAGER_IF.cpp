/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.cpp,v 1.5 2004-11-29 15:28:47 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for _socket,
*                        and refined comments
* lafrasse  22-Nov-2004  Use msgSOCKET_CLIENT instead of system socket calls.
* lafrasse  24-Nov-2004  Comment refinments, and includes cleaning
* gzins     29-Nov-2004  Fixed bug in Connect method
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class definition.
 */

static char *rcsId="@(#) $Id: msgMANAGER_IF.cpp,v 1.5 2004-11-29 15:28:47 scetre Exp $"; 
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
 * Try to establish the connection with the communication server.
 *
 * The server host name is (in order) :
 * \li the msgManagerHost parameter if its value is \em not NULL
 * \li the MSG_MANAGER_HOST environment variable value, if it is defined
 * \li the local host name
 *
 * \param procName the local processus name
 * \param msgManagerHost the server host name, or NULL
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Connect   (const mcsPROCNAME  procName,
                                        const char*        msgManagerHost)
{
    logExtDbg("msgMANAGER_IF::Connect()");

    mcsSTRING256       hostName;
    msgMESSAGE         msg;
    mcsINT32           nbRetry = 0;
    mcsINT32           status  = 0;

    // If a connection is already open...
    if (IsConnected() == mcsTRUE)
    {
        // Raise an error
        errAdd(msgERR_PROC_ALREADY_CONNECTED);

        // Return an error code
        return FAILURE;
    }

    // Try to initialize MCS services
    if (mcsInit(procName) == FAILURE)
    {
        errAdd(msgERR_MCSINIT);
        return FAILURE;
    }

    // Try to get a valid host name
    // If the server host name is given by parameter...
    if (msgManagerHost != NULL)
    {
        strcpy(hostName, msgManagerHost);
    }
    else
    {
        // If the MSG_MANAGER_HOST Env. Var. is not defined...
        if (miscGetEnvVarValue("MSG_MANAGER_HOST", hostName,sizeof(mcsBYTES256))
            == FAILURE)
        {
            // Try to get the local host name
            memset(hostName, '\0', sizeof(hostName));
            if (miscGetHostName(hostName, sizeof(hostName)) == FAILURE)
            {
                return FAILURE;
            }
            errResetStack();
        }
    }
    logTest("'msgManager' server host name is '%s'", hostName );

    // Try to establish the connection, retry otherwise...
    nbRetry = 2;
    do 
    {
        // Try to connect to msgManager
        _socket.Open(hostName, msgMANAGER_PORT_NUMBER);
        if (_socket.IsConnected() == mcsFALSE)
        {
            if (--nbRetry <= 0 )
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
    } while (status == -1);


    // Try to register with msgManager
    if (SendCommand(msgREGISTER_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }
    
    // Wait for its answer
    if (Receive(msg, 1000) == FAILURE)
    {
        _socket.Close();
        return FAILURE;
    }

    // If the reply as ERROR...
    if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Try to put the received errors in the MCS error stack
        if (errUnpackStack(msg.GetBodyPtr(), msg.GetBodySize())
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
 * Try to send a command message to the communication server.
 *
 * Send the \<command\> named message with all the parameters stored in
 * \<buffer\>  to the \<destProc\> named process. If \<buflen\> equals 0, then
 * strlen() is used to get \<buffer\> size.
 *
 * \param command a command name
 * \param destProc a remote process name
 * \param buffer a parameter list stored in a string
 * \param bufLen the length of the parameter list string, or 0 to use strlen()
 * internally
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::SendCommand         (const char        *command,
                                                  const mcsPROCNAME  destProc,
                                                  const char        *buffer,  
                                                  const mcsINT32     bufLen)
{
    logExtDbg("msgMANAGER_IF::SendCommand()");

    msgMESSAGE msg;

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        // Raise an error
        errAdd(msgERR_PROC_NOT_CONNECTED);

        // Return an error code 
        return FAILURE;
    }

    // Build the message header
    msg.SetSender(mcsGetProcName());
    msg.SetRecipient(destProc);
    msg.SetType(msgTYPE_COMMAND);
    msg.SetCommand(command);
 
    // Try to build the message body
    if (msg.SetBody((char*)buffer, bufLen) == FAILURE)
    {
        return FAILURE;
    }

    // Try to send the message
    return (_socket.Send(msg));
}

/**
 * Try to send a reply message to the communication server.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current messag is the last one or not
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
        // Raise an error
        errAdd(msgERR_PROC_NOT_CONNECTED);

        // Return an error code
        return FAILURE;
    }

    /* Build the reply message header */
    msg.SetLastReplyFlag(lastReply);
    /* If there is no error in the MCS error stack */
    if (errStackIsEmpty() == mcsTRUE)
    {
        /* Set message type to REPLY */
        msg.SetType(msgTYPE_REPLY);
    }
    else
    {
        char errStackContent[msgBODYMAXLEN];
        /* Try to put the MCS error stack data in the message body */
        if (errPackStack(errStackContent, msgBODYMAXLEN) == FAILURE)
        {
            return FAILURE;
        }

        /* Store the message body size in network byte order */
        msg.SetBody(errStackContent, msgBODYMAXLEN);

        /* Set message type to ERROR_REPLY */
        msg.SetType(msgTYPE_ERROR_REPLY);

        /* Empty MCS error stack */
        errResetStack();
    }

    logTest("Sending '%s' answer : %s", msg.GetCommand(), msg.GetBodyPtr());
    return _socket.Send(msg);
}

/**
 * Try to receive a message from the communication server.
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
        // Raise an error
        errAdd(msgERR_PROC_NOT_CONNECTED);

        // Return an error code
        return FAILURE;
    }

    // Return weither a message was received or not
    return (_socket.Receive(msg, timeoutInMs));
}

/**
 * Try to close the connection with the communication server.
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Disconnect(void)
{
    logExtDbg("msgMANAGER_IF::Disconnect()");

    // If no connection is already open...
    if (IsConnected() == mcsFALSE)
    {
        // Raise an error
        errAdd(msgERR_PROC_NOT_CONNECTED);

        // Return an error code
        return FAILURE;
    }

    // Try to send a 'close command' message to msgManager
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
        return (FAILURE);
    }
    
    // Close the socket
    _socket.Close();

    logExtDbg("Connection to 'msgManager' closed");

    return SUCCESS;
}
 
/**
 * Get the socket descriptor for the message queue
 *
 * Returns the socket descriptor of the communication link with message
 * manager. This allows a process to monitor its message queue using the UNIX
 * function select().
 *
 * \warning
 * The file descriptor returned must NOT be read or manipulated in any way
 * (e.g. close()) by the process. Otherwise the monitoring system will lose
 * syncronization with the message manager.
 */
mcsINT32 msgMANAGER_IF::GetMsgQueue()
{
    logExtDbg("msgMANAGER_IF::GetMsgQueue()");

    return _socket.GetDescriptor();
}
 
/*___oOo___*/

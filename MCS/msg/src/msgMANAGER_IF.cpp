/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.cpp,v 1.2 2004-11-19 23:55:17 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for _socket,
*                        and refined comments
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class definition.
 */

static char *rcsId="@(#) $Id: msgMANAGER_IF.cpp,v 1.2 2004-11-19 23:55:17 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
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
#include "msgMANAGER_IF.h"
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/*
 * Class constructor
 */
msgMANAGER_IF::msgMANAGER_IF()
{
    _socket = -1;
}

/*
 * Class destructor
 */
msgMANAGER_IF::~msgMANAGER_IF()
{
    // If the connection to the msgManager process is still up
    if (IsConnected() == mcsTRUE)
    {
        // Close this connection
        Disconnect();
    }
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

    struct hostent    *remoteHostEnt;
    mcsSTRING256        hostName;
    
    int                sd;
    struct sockaddr_in addr;
    mcsINT32           nbRetry;
    mcsINT32           status;
    msgMESSAGE         msg;

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

    // Get msgManager server data
    logTest("'msgManager' server host name is '%s'", hostName );
    remoteHostEnt = gethostbyname(hostName);
    if (remoteHostEnt == (struct hostent *) NULL)
    {
        errAdd(msgERR_GETHOSTBYNAME, strerror(errno));
        return FAILURE;
    }

    // Try to establish the connection, retry otherwise...
    nbRetry = 2;
    do 
    {
        // Create the socket
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd == -1)
        { 
            errAdd(msgERR_SOCKET, strerror(errno));
            return FAILURE; 
        }

        // Initialize sockaddr_in
        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_port = htons(msgMANAGER_PORT_NUMBER);
        memcpy(&(addr.sin_addr), remoteHostEnt->h_addr,remoteHostEnt->h_length);
        addr.sin_family = AF_INET;

        // Try to connect to msgManager
        status = connect(sd , (struct sockaddr *)&addr, sizeof(addr));
        if (status == -1)
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
                close(sd);
            }
        }
    } while (status == -1);


    // Store the established connection socket
    _socket = sd;

    // Try to register with msgManager
    if (SendCommand(msgREGISTER_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        close(_socket);
        _socket = -1;
        return FAILURE;
    }
    
    // Wait for its answer
    if (Receive(msg, 1000) == FAILURE)
    {
        close(_socket);
        _socket = -1;
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
        close(_socket);
        _socket = -1;       
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
mcsLOGICAL msgMANAGER_IF::IsConnected()
{
    logExtDbg("msgMANAGER_IF::IsConnected()");

    return ((_socket != -1)?mcsTRUE:mcsFALSE);
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
    return (msgSendTo(_socket, msg.GetMessageRaw()));
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

    // Try to send the reply message
    return (msgSendReplyTo(_socket, msg.GetMessageRaw(), lastReply));
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
    return (msgReceiveFrom(_socket, msg.GetMessageRaw(), timeoutInMs));
}

/**
 * Try to close the connection with the communication server.
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER_IF::Disconnect()
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
        close(_socket);
        _socket = -1;
        return FAILURE;
    }
    
    // Wait for its answer
    msgMESSAGE msg;
    if (Receive(msg, 1000) == FAILURE)
    {
        close(_socket);
        _socket = -1;
        return FAILURE;
    }

    // If an error occured while deconnecting...
    if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        // Put the received error in the local MCS error stack
        errUnpackStack(msg.GetBodyPtr(), msg.GetBodySize());
        close(_socket);
        _socket = -1;       
        return (FAILURE);
    }
    
    // Close the socket
    close(_socket);
    _socket = -1;       

    logExtDbg("Connection to 'msgManager' closed");

    return SUCCESS;
}
 

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

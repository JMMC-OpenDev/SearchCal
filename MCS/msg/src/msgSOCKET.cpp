/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Interface class providing all the low-level API to handle IPv4 network
 * communication.
 *
 * \sa msgSOCKET
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgSOCKET.cpp,v 1.23 2006-05-11 13:04:56 mella Exp $";

/* 
 * System Headers 
 */
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msgSOCKET.h"
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"

/**
 * Class constructor
 */
msgSOCKET::msgSOCKET()
{
    // Reset the socket structure
    memset(&_address, 0, sizeof(_address));
    _descriptor = -1;
}

/**
 * Copy constructor
 */
msgSOCKET::msgSOCKET(const msgSOCKET &socket)
{
    _descriptor = socket._descriptor;
    memcpy(&_address, &socket._address, sizeof(sockaddr_in));
}

/**
 * Class destructor
 */
msgSOCKET::~msgSOCKET()
{
    logExtDbg("msgSOCKET::~msgSOCKET(%d)", _descriptor); 
    // If the socket is still connected, close it
    if (IsConnected() == mcsTRUE)
    {
        Close();
    }
}


/*
 * Public methods
 */

/**
 * Create the socket.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Create(void)
{
    logExtDbg("msgSOCKET::Create()");

    // Is the socket is already connected
    if (IsConnected() == mcsTRUE)
    {
        errAdd(msgERR_PROC_ALREADY_CONNECTED);
        return mcsFAILURE;
    }

    // Instantiate a new IPv4/TCP socket
    _descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Check that the socket has been succesfully created
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET, strerror(errno));
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Get the socket number.
 *
 * \return the socket number
 */
mcsINT32 msgSOCKET::GetDescriptor(void)
{
    logExtDbg("msgSOCKET::GetDescriptor()");

    // return the socket number
    return _descriptor;
}

/**
 * Return wether the socket is already connected or not.
 *
 * \return mcsTRUE if the socket is already connected, mcsFALSE otherwise
 */
mcsLOGICAL msgSOCKET::IsConnected(void)
{
    logExtDbg("msgSOCKET::IsConnected()");

    // If the socket has not already been created
    if (_descriptor == -1)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Bind the socket to a specific port number.
 *
 * \param port the port number to which the socket should be bound
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Bind(const mcsUINT16 port)
{
    logExtDbg("msgSOCKET::Bind()");
    
    // Check that the socket is connected
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return mcsFAILURE;
    }

    // Define socket configuration
    _address.sin_family      = AF_INET;      // TCP/IP connection
    _address.sin_port        = htons(port);  // Desired port number
    _address.sin_addr.s_addr = INADDR_ANY;   // Host network address

    // Tell the kernel that even if this port is busy (in the TIME_WAIT state),
    // go ahead and reuse it anyway.
    int opt=1;
    if (setsockopt(_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, 
                   sizeof (opt)) == -1)
    {
        errAdd(msgERR_BIND, strerror(errno));
        Close();
        return mcsFAILURE;
    }

    // Bind the socket to the given port number
    if (bind(_descriptor, (struct sockaddr*)&_address, sizeof(_address)) == -1)
    {
        errAdd(msgERR_BIND, strerror(errno));
        Close();
        return mcsFAILURE;
    }

    // Verify the new socket validity
    socklen_t addressLength = sizeof(_address);
    if (getsockname(_descriptor, (struct sockaddr*)&_address, &addressLength)
        == -1)
    {
        errAdd(msgERR_GETSOCKNAME, strerror(errno));
        Close();
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Start listening on the socket.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Listen(void)
{
    logExtDbg("msgSOCKET::Listen()");

    // Check that the socket is connected
    if ( IsConnected() == mcsFALSE )
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return mcsFAILURE;
    }

    // Start listening
    int listen_return = listen(_descriptor, MAXCONNECTIONS);
    if (listen_return == -1)
    {
        errAdd(msgERR_LISTEN, strerror(errno));
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Wait until a new connection is made on the socket, and then accept it.
 *
 * \param socket the newly created socket to the connected process
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Accept(msgSOCKET &socket) const
{
    logExtDbg("msgSOCKET:::Accept()");

    // Wait until a new connection is received on the socket
    int addr_length    = sizeof(_address);
    socket._descriptor = accept(_descriptor, (sockaddr *)&_address,
                                (socklen_t*)&addr_length);

    // If the new socket is not functionnal
    if (socket.IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Connect the socket to a given remote host name and port number.
 *
 * \param host the remote machine host name or IPv4 address
 * \param port the remote server port number
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Connect(const std::string host,
                                 const mcsUINT16   port)
{
    logExtDbg("msgSOCKET::Connect()");

    // Check that the socket is connected
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return mcsFAILURE;
    }

    _address.sin_family = AF_INET;
    // Set the remote server port number to connect to
    _address.sin_port   = htons(port);

    // Convert the host name string in a network address structure
    inet_pton(AF_INET, host.c_str(), &_address.sin_addr);
    if (errno == EAFNOSUPPORT) 
    {
        return mcsFAILURE;
    }

    // Connect to the remote server
    int status;
    status = connect(_descriptor, (sockaddr*) &_address, sizeof(_address));
    if (status != 0)
    {
        errAdd(msgERR_CONNECT, strerror(errno));
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Send a string through the socket.
 *
 * \param string the string to be sent
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Send(const std::string string) const
{
    logExtDbg("msgSOCKET::Send()");

    // Send the given string
    int status;
    status = send(_descriptor, string.c_str(), string.size(), MSG_NOSIGNAL);
    if ( status == -1 )
    {
        errAdd(msgERR_CONNECT, strerror(errno));
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Waits until the socket received some data, then give it back as a string.
 *
 * \param string a string object that will be overwritten with the received
 * data
 * @param timeoutInMs the number of milliseconds before a timeout occurs
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Receive(std::string& string,
                                 mcsINT32 timeoutInMs)
{
    logExtDbg("msgSOCKET::Receive()");

    struct timeval timeout ;
    fd_set         readMask ;
    int status;
    
    char buf[MAXRECV+1];
    memset(buf, 0, MAXRECV+1);

    string = "";

    // Compute the timeout value
    if (timeoutInMs == msgNO_WAIT)
    {
        timeout.tv_sec  = 0 ;
        timeout.tv_usec = 0 ;
    }
    else if (timeoutInMs != msgWAIT_FOREVER )
    {
        timeout.tv_sec  = (timeoutInMs / 1000);
        timeout.tv_usec = (timeoutInMs % 1000);
    }

    FD_ZERO(&readMask);
    FD_SET(_descriptor, &readMask);

    // If a timeout is defined
    if (timeoutInMs != msgWAIT_FOREVER)
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL, &timeout);
    }
    else
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL,
                        (struct timeval *)NULL);
    }

    // If the timeout expired...
    if (status == 0)
    {
        errAdd(msgERR_TIMEOUT_EXPIRED);
        return mcsFAILURE;
    }

    // If an error occured during select()
    if (status == -1)
    {
        errAdd(msgERR_SELECT, strerror(errno));
        return mcsFAILURE;
    }
    
    // if status non equal to 0
    while(status != 0)
    {
        // reinit the reveive buffer
        memset(buf, 0, MAXRECV+1);  
        // reveive in it the data
        status = recv(_descriptor, buf, MAXRECV, 0 );
        // if an error occur return error
        if (status == -1)
        {
            errAdd(msgERR_RECV, strerror(errno));
            return mcsFAILURE;
        }
        // else put it on the complete resulting string
        else
        {
            string += buf;
        }
    }
    return mcsSUCCESS;
}

/**
 * Send a msgMESSAGE object content through the socket.
 *
 * \param msg the message to be sent
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Send(msgMESSAGE &msg)
{
    logExtDbg("msgSOCKET::Send()");

    // Log info
    if (msg.GetType() == msgTYPE_COMMAND)
    {
        if (msg.GetBodySize() > 80)
        {
            logInfo("Sent '%s %.80s...' to '%s'", 
                    msg.GetCommand(), msg.GetBody(), msg.GetRecipient());
        }
        else
        {
            logInfo("Sent '%s %s' to '%s'", 
                    msg.GetCommand(), msg.GetBody(), msg.GetRecipient());
        }                                       
    }
    else if (msg.GetType() == msgTYPE_REPLY)
    {
        if (msg.GetBodySize() > 80)
        {
            logInfo("Sent reply '%.80s...' to '%s'", 
                    msg.GetBody(), msg.GetSender());
        }
        else
        {
            logInfo("Sent reply '%s' to '%s'", 
                    msg.GetBody(), msg.GetSender());
        }    
    }
    else if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        logInfo("Sent error reply to '%s'; '%s' command failed", 
                 msg.GetSender(), msg.GetCommand());
    }

    // Set message id
    if (msg.GetCommandId() == -1)
    {
        // REM : A safer way to get a true unique ID is to use 'libuuid' (see
        // `man uuidgen`), but it seems to be not available by default under
        // BSD/Mac OS X, so it's not used for the moment (maybe after Mac OS X
        // 10.4 release ;)

        // Get the system time
        struct timeval  time; 
        gettimeofday(&time, NULL);

        // Command Id is the time of the day in msec
        mcsINT32 commandId;
        commandId = (time.tv_sec%86400) * 1000 + time.tv_usec/1000;
        msg.SetCommandId(commandId);
    }

    // Send the message header
    mcsINT32 nbBytesSent;
    nbBytesSent = send(_descriptor, &msg._header, msgHEADERLEN, 0);
    mcsINT32 msgLength = msgHEADERLEN;

    // If the body exists, sent it
    if (msg.GetBodySize() != 0)
    {
        nbBytesSent += send(_descriptor, msg.GetBody(), msg.GetBodySize(), 0);
        msgLength   += msg.GetBodySize();
    }

    // Verify that every bytes were sent
    if (nbBytesSent != msgLength)
    {
        // If no byte at all were sent...
        if (nbBytesSent == -1)
        {
            errAdd(msgERR_SEND, strerror(errno));
        }
        else
        {
            errAdd(msgERR_PARTIAL_SEND, nbBytesSent, msgLength,strerror(errno));
        }

        // Return an error code
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Wait until a msgMESSAGE object content is received through the socket.
 *
 * \param msg the message object that will be overwritten with the received
 * contnent
 * \param timeoutInMs the number of milliseconds before a timeout occurs
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET::Receive(msgMESSAGE         &msg,
                                 mcsINT32           timeoutInMs)
{
    struct timeval timeout ;
    fd_set         readMask ;
    mcsINT32       status, nbBytesToRead, nbBytesRead;

    logExtDbg("msgSOCKET::Receive()");

    // Compute the timeout value
    if (timeoutInMs == msgNO_WAIT)
    {
        timeout.tv_sec  = 0 ;
        timeout.tv_usec = 0 ;
    }
    else if (timeoutInMs != msgWAIT_FOREVER )
    {
        timeout.tv_sec  = (timeoutInMs / 1000);
        timeout.tv_usec = (timeoutInMs % 1000);
    }

    FD_ZERO(&readMask);
    FD_SET(_descriptor, &readMask);

    // If a timeout is defined
    if (timeoutInMs != msgWAIT_FOREVER)
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL, &timeout);
    }
    else
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL,
                        (struct timeval *)NULL);
    }

    // If the timeout expired...
    if (status == 0)
    {
        errAdd(msgERR_TIMEOUT_EXPIRED);
        return mcsFAILURE;
    }

    // If an error occured during select()
    if (status == -1)
    {
        errAdd(msgERR_SELECT, strerror(errno));
        return mcsFAILURE;
    }

    // If the connection with the remote processus was lost...
    ioctl(_descriptor, FIONREAD, (unsigned long *)&nbBytesToRead);
    if (nbBytesToRead == 0)
    {
        // Close socket and exit...
        close(_descriptor);
        errAdd(msgERR_BROKEN_PIPE);
        errCloseStack(); 
        exit(EXIT_FAILURE);
    }
    else
    {
        // Read the message header
        nbBytesRead = recv(_descriptor, &msg._header, msgHEADERLEN,
                           MSG_WAITALL);
        if (nbBytesRead != msgHEADERLEN)
        {
            errAdd(msgERR_PARTIAL_HDR_RECV, nbBytesRead, msgHEADERLEN);
            return (mcsFAILURE);
        }

        // Read the message body if it exists
        mcsINT32 bodySize;
        bodySize = msg.GetBodySize();
        if (bodySize == -1)
        {
            return mcsFAILURE;
        }

        // If there is a body
        if (bodySize != 0)
        {
            // Allocate some memory for the coming message
            if (msg.AllocateBody(bodySize) == mcsFAILURE)
            {
                return (mcsFAILURE);
            }

            // Get the body from the socket and write it inside msgMESSAGE
            nbBytesRead = recv(_descriptor, miscDynBufGetBuffer(&msg._body), 
                               bodySize, MSG_WAITALL);
            if (nbBytesRead != bodySize)
            {
                errAdd(msgERR_PARTIAL_BODY_RECV, nbBytesRead, bodySize);
                return (mcsFAILURE); 
            }
        }

        // Set the sender id (i.e. socket descriptor if not yet set)
        if (msg.GetSenderId() == -1)
        {
            msg.SetSenderId(_descriptor);
        }
    } 

    // Log info
    if (msg.GetType() == msgTYPE_COMMAND)
    {
        if (msg.GetBodySize() > 80)
        {
            logInfo("Received '%s %.80s...' from '%s'", 
                    msg.GetCommand(), msg.GetBody(), msg.GetSender());
        }
        else
        {
            logInfo("Received '%s %s' from '%s'", 
                    msg.GetCommand(), msg.GetBody(), msg.GetSender());
        }     
    }
    else if (msg.GetType() == msgTYPE_REPLY)
    {
        if (msg.GetBodySize() > 80)
        {
            logInfo("Received reply '%.80s...' from '%s'", 
                    msg.GetBody(), msg.GetRecipient());
        }
        else
        {
            logInfo("Received reply '%s' from '%s'", 
                    msg.GetBody(), msg.GetRecipient());
        }    
    }
    else if (msg.GetType() == msgTYPE_ERROR_REPLY)
    {
        logInfo("Received error reply from '%s'; '%s' command failed", 
                 msg.GetRecipient(), msg.GetCommand());
    }

    return mcsSUCCESS;
}

/**
 * Close the socket.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgSOCKET::Close(void)
{
    logExtDbg("msgSOCKET::Close()");
    
    // Shutdown read/write operation through the socket
    shutdown(_descriptor, SHUT_RDWR);

    // Close the socket
    close(_descriptor);
    _descriptor = -1;

    return mcsSUCCESS;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

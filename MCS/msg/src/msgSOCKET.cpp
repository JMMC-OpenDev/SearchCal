/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.cpp,v 1.8 2004-12-07 07:50:59 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    19-Nov-2004  Created
* lafrasse  23-Nov-2004  Comment refinments, and includes cleaning
* lafrasse  25-Nov-2004  Added error management code
* gzins     29-Nov-2004  Fixed wrong returned value in IsConnected method
*                        Do not read body if body size is 0 in Receive()
* lafrasse  03-Dec-2004  Changed port number type from mcsINT32 to mcsUINT16
* gzins     06-Dec-2004  Implemented copy constructor
* gzins     06-Dec-2004  Removed copy constructor
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET.cpp,v 1.8 2004-12-07 07:50:59 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <errno.h>
#include <sys/ioctl.h>

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
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Create(void)
{
    logExtDbg("msgSOCKET::Create()");

    // Is the socket is already connected
    if (IsConnected() == mcsTRUE)
    {
        errAdd(msgERR_PROC_ALREADY_CONNECTED);
        return FAILURE;
    }

    // Instantiate a new IPv4/TCP socket
    _descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Check that the socket has been succesfully created
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
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
 * Return weither the socket is already connected or not.
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
 * Bind the socket to a specific port.
 *
 * \param port the port to which the socket should be bind
 *
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Bind(const mcsUINT16 port)
{
    logExtDbg("msgSOCKET::Bind()");
    
    // Check that the socket is connected
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return FAILURE;
    }

    // Define socket configuration
    _address.sin_family      = AF_INET;      // TCP/IP connection
    _address.sin_port        = htons(port);  // Desired port number
    _address.sin_addr.s_addr = INADDR_ANY;   // Host network address

    // Try to bind the socket to the given port number
    if (bind(_descriptor, (struct sockaddr*)&_address, sizeof(_address)) == -1)
    {
        errAdd(msgERR_BIND, strerror(errno));
        Close();
        return FAILURE;
    }

    // Verify the new socket validity
    socklen_t addressLength = sizeof(_address);
    if (getsockname(_descriptor, (struct sockaddr*)&_address, &addressLength)
        == -1)
    {
        errAdd(msgERR_GETSOCKNAME, strerror(errno));
        Close();
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Start listening on the socket.
 *
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Listen(void)
{
    logExtDbg("msgSOCKET::Listen()");

    // Check that the socket is connected
    if ( IsConnected() == mcsFALSE )
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return FAILURE;
    }

    // Start listening
    int listen_return = listen(_descriptor, MAXCONNECTIONS);
    if (listen_return == -1)
    {
        errAdd(msgERR_LISTEN, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Wait until a new connection is made on the socket, and then accept it.
 *
 * \param socket the newly created socket to the connected process
 *
 * @return SUCCESS on successfull completion, FAILURE otherwise
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
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Connect the socket to a given remote host name and port number.
 *
 * \param host the remote machine host name or IPv4 address
 * \param port the remote server  port number
 *
 * @return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Connect(const std::string host,
                                 const mcsUINT16   port)
{
    logExtDbg("msgSOCKET::Connect()");

    // Check that the socket is connected
    if (IsConnected() == mcsFALSE)
    {
        errAdd(msgERR_SOCKET_NOT_CONNECTED);
        return FAILURE;
    }

    _address.sin_family = AF_INET;
    // Set the remote server port number to connect to
    _address.sin_port   = htons(port);

    // Try to convert the host name string in a network address structure
    inet_pton(AF_INET, host.c_str(), &_address.sin_addr);
    if (errno == EAFNOSUPPORT) 
    {
        return FAILURE;
    }

    // Try to connect to the remote server
    int status = connect(_descriptor, (sockaddr*) &_address, sizeof(_address));
    if (status != 0)
    {
        errAdd(msgERR_CONNECT, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Send a string on the socket.
 *
 * \param string the string to be sended to the connected process
 *
 * @return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Send(const std::string string) const
{
    logExtDbg("msgSOCKET::Send()");

    // Try to send the given string
    int status = send(_descriptor, string.c_str(), string.size(), MSG_NOSIGNAL);
    if ( status == -1 )
    {
        errAdd(msgERR_CONNECT, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Waits until the socket received some data, then given back as a string.
 *
 * \param string a string object that will be overwritten with the incomming
 * data
 *
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Receive(std::string& string) const
{
    logExtDbg("msgSOCKET::Receive()");

    char buf[MAXRECV+1];
    memset(buf, 0, MAXRECV+1);

    string = "";

    // Wait untill some data are received
    int status = recv(_descriptor, buf, MAXRECV, 0 );
    if (status == -1)
    {
        errAdd(msgERR_RECV, strerror(errno));
        return FAILURE;
    }
    else
    {
        // If nothing was received
        if (status == 0)
        {
            errAdd(msgERR_RECV, strerror(errno));
            return FAILURE;
        }
        else
        {
            string = buf;
            return SUCCESS;
        }
    }
}

/**
 * Send a message on the socket.
 *
 * \param msg the message to send
 *
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Send(msgMESSAGE &msg)
{
    logExtDbg("msgSOCKET::Send()");

    // Try to send the message
    mcsINT32 msgLength   = msgHEADERLEN + msg.GetBodySize();
    mcsINT32 nbBytesSent = send(_descriptor, msg.GetMessagePtr(), msgLength, 0);

    // If some sent bytes were lost...
    if (nbBytesSent != msgLength)
    {
        // If no byte at all were sent...
        if (nbBytesSent == 0)
        {
            errAdd(msgERR_SEND, strerror(errno));
        }
        else
        {
            errAdd(msgERR_PARTIAL_SEND, nbBytesSent, msgLength,strerror(errno));
        }

        // Return an error code
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * Wait until a message is received on the socket.
 *
 * \param msg the message object that will be overwritten with the received one
 * \param timeoutInMs the number of milliseconds before a timeout occur
 *
 * \return SUCCESS on successfull completion, FAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET::Receive(msgMESSAGE         &msg,
                                 mcsINT32           timeoutInMs)
{
    struct timeval timeout ;
    fd_set         readMask ;
    mcsINT32       status, nbBytesToRead, nbBytesRead;

    logExtDbg("msgReceiveFrom()");

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
        errAdd(msgERR_TIMEOUT_EXPIRED, "No specific error message !!!");
        return FAILURE;
    }

    // If an error occured during select()
    if (status == -1)
    {
        errAdd(msgERR_SELECT, strerror(errno));
        return FAILURE;
    }

    // If the connection with the remote processus was lost...
    ioctl(_descriptor, FIONREAD, (unsigned long *)&nbBytesToRead);
    if (nbBytesToRead == 0)
    {
        errAdd(msgERR_BROKEN_PIPE);
        return FAILURE;
    }
    else
    {
        // Read the message header
        nbBytesRead = recv(_descriptor, (char*)msg.GetHeaderPtr(),
                           msgHEADERLEN, 0);
        if (nbBytesRead != msgHEADERLEN)
        {
            errAdd(msgERR_PARTIAL_HDR_RECV, nbBytesRead, msgHEADERLEN);
            return (FAILURE);
        }

        // Read the message body if it exists
        memset(msg.GetBodyPtr(), '\0', (msgBODYMAXLEN + 1));
        if (msg.GetBodySize() != 0)
        {
            nbBytesRead = recv(_descriptor, msg.GetBodyPtr(), msg.GetBodySize(), 0);
            if (nbBytesRead != msg.GetBodySize())
            {
                errAdd(msgERR_PARTIAL_BODY_RECV, nbBytesRead, msg.GetBodySize());
                return (FAILURE); 
            }
        }
    } 

    return SUCCESS;
}

/**
 * Close the socket.
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgSOCKET::Close(void)
{
    logExtDbg("msgSOCKET::Close()");
    
    // Shutdown read/write operation through the socket
    shutdown(_descriptor, SHUT_RDWR);

    // Close the socket
    close(_descriptor);
    _descriptor = -1;

    return SUCCESS;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

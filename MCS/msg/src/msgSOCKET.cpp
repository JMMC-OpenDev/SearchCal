/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.cpp,v 1.2 2004-11-23 08:25:25 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    19-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET.cpp,v 1.2 2004-11-23 08:25:25 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
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
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"

/*
 * Class constructor
 */
msgSOCKET::msgSOCKET()
{
    memset ( &_address, 0, sizeof ( _address ) );
}

/*
 * Class destructor
 */
msgSOCKET::~msgSOCKET()
{
    if ( IsConnected() == SUCCESS )
    {
        Close();
    }
}

/*
 * Public methods
 */

/**
 * Get the socket number
 *
 * \return the socket number
 */
mcsINT32 msgSOCKET::GetDescriptor()
{
    logExtDbg("msgSOCKET::GetDescriptor()");
    // return the socket number
    return _descriptor;
}

/**
 * Close the socket
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgSOCKET::Close()
{
    logExtDbg("msgSOCKET::Close()");
    
    // shutdown and close
    (void) shutdown (_descriptor, 2);
    (void) close (_descriptor);

    return SUCCESS;
}


/**
 * Create the socket 
 *
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Create()
{
    logExtDbg("msgSOCKET::Create()");
    _descriptor = socket ( AF_INET, SOCK_STREAM, 0 );

    // Check that the socket is connected
    if ( IsConnected() == FAILURE )
    {
        return FAILURE;
    }

    // TIME_WAIT - argh
    int on = 1;
    if (setsockopt(_descriptor, SOL_SOCKET, SO_REUSEADDR,
                   (const char*)&on, sizeof(on)) == -1 )
    {
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * Bind the socket on a specific port 
 *
 * @param port the port where the method bind te socket 
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Bind(const mcsINT32 port)
{
    logExtDbg("msgSOCKET::Bind()");
    
    // Check that the socket is connected
    if ( IsConnected() == FAILURE )
    {
        return FAILURE;
    }

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons ( port );

    int bind_return = bind ( _descriptor,
                             ( struct sockaddr * ) &_address,
                             sizeof ( _address ) );
    // if bind failed return FAILURE
    if ( bind_return == -1 )
    {
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief Listen the socket
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Listen()
{
    logExtDbg("msgSOCKET::Listen()");
    // Check that the socket is connected
    if ( IsConnected() == FAILURE )
    {
        return FAILURE;
    }
    int listen_return = listen ( _descriptor, MAXCONNECTIONS );

    if ( listen_return == -1 )
    {
        return FAILURE;
    }
    return SUCCESS;
}

/**
 * @brief Accept a socket
 *
 * @param socket  
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Accept(msgSOCKET &socket) const
{
    logExtDbg("msgSOCKET:::Accept()");
    int addr_length = sizeof ( _address );
    socket._descriptor  = (accept(_descriptor, (sockaddr *)&_address,
                                   ( socklen_t * ) &addr_length ) );

    if ( socket.GetDescriptor() <= 0 )
    {
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * @brief connect a socket
 *
 * @param host  
 * @param port
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Connect(const std::string host,
                                 const mcsINT32 port)
{
    logExtDbg("msgSOCKET::Connect()");
    // Check that the socket is connected
    if ( IsConnected() == FAILURE )
    {
        return FAILURE;
    }

    _address.sin_family = AF_INET;
    _address.sin_port = htons ( port );

    int status = inet_pton ( AF_INET, host.c_str(), &_address.sin_addr );

    if ( errno == EAFNOSUPPORT ) 
    {
        return FAILURE;
    }

    status = ::connect(_descriptor, (sockaddr*) &_address, sizeof(_address));

    if ( status == 0 )
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

/**
 * @brief Send a string on the socket
 *
 * @param string  
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Send(const std::string string) const
{
    logExtDbg("msgSOCKET::Send()");

    int status = ::send ( _descriptor, string.c_str(), string.size(), MSG_NOSIGNAL );
    if ( status == -1 )
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
}

/**
 * @brief receive a string on a socket
 *
 * @param string
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Receive(std::string& string) const
{
    logExtDbg("msgSOCKET::Receive()");
    char buf [ MAXRECV + 1 ];

    string = "";

    memset ( buf, 0, MAXRECV + 1 );

    int status = ::recv ( _descriptor, buf, MAXRECV, 0 );

    if ( status == -1 )
    {
        return FAILURE;
    }
    else if ( status == 0 )
    {
        return FAILURE;
    }
    else
    {
        string = buf;
        return SUCCESS;
    }
}

/**
 * Say if the socket is connected or not 
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::IsConnected()
{
    logExtDbg("msgSOCKET::IsConnected()");
    if (_descriptor==-1)
    {
        return FAILURE;
    }
    return SUCCESS;
}

/**
 * Send a message on the socket 
 *
 * \param msg the message to send
 *
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Send(msgMESSAGE &msg)
{
    mcsINT32   msgLen;
    mcsINT32   nbBytesSent;

    logExtDbg("msgSOCKET::Send()");

    // Try to send the message
    msgLen = sizeof(msgHEADER) + msg.GetBodySize();
    nbBytesSent = send(_descriptor, (char *)&msg, msgLen, 0);
    // If some sent bytes were lost...
    if (nbBytesSent != msgLen)
    {
        // If no byte at all were sent...
        if (nbBytesSent == 0)
        {
            errAdd(msgERR_SEND, strerror(errno));
        }
        else
        {
            errAdd(msgERR_PARTIAL_SEND, nbBytesSent, msgLen, strerror(errno));
        }

        //Return an errror code
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * Receive message 
 *
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 **/
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

    if (timeoutInMs != msgWAIT_FOREVER)
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL, &timeout);
    }
    else
    {
        status = select(_descriptor + 1, &readMask, NULL, NULL, (struct timeval *)NULL);
    }

    // If the timeout expired... 
    if (status == 0)
    {
        /* Raise an error */
        errAdd(msgERR_TIMEOUT_EXPIRED, "No specific error message !!!");

        // Return an error code
        return FAILURE;
    }

    /* If an error occured during select() */
    if (status == -1)
    {
        /* Raise an error */
        errAdd(msgERR_SELECT, strerror(errno));

        // Return an error code 
        return FAILURE;
    }

    /* If the connection with the remote processus was lost... */
    ioctl(_descriptor, FIONREAD, (unsigned long *)&nbBytesToRead);
    if (nbBytesToRead == 0)
    {
        // Raise an error 
        errAdd(msgERR_BROKEN_PIPE);

        // Return an error code 
        return FAILURE;
    }
    else
    {
        // Read the message header
        nbBytesRead = recv(_descriptor, (char *)msg.GetHeaderPtr(), sizeof(msgHEADER), 0);
        if (nbBytesRead != sizeof(msgHEADER))
        {
            errAdd(msgERR_PARTIAL_HDR_RECV,nbBytesRead, sizeof(msgHEADER));
            return (FAILURE);
        }

        // Read the message body if it exists
        memset(msg.GetBodyPtr(), '\0', (msgBODYMAXLEN + 1));
        if (msg.GetBodySize() != 0 )
        {
            nbBytesRead = recv(_descriptor, msg.GetBodyPtr(), msg.GetBodySize(), 0);
            if (nbBytesRead != msg.GetBodySize())
            {
                errAdd(msgERR_PARTIAL_BODY_RECV, nbBytesRead,
                       msg.GetBodySize());
                return (FAILURE); 
            }
        }
    } 

    return SUCCESS;
}
/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

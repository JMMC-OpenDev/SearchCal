#ifndef msgSOCKET_H
#define msgSOCKET_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.h,v 1.3 2004-11-22 15:24:05 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    19-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Class declaration
 */

/**
 * Object wrapper around system socket.
 * 
 * The msgSOCKET object provide method to create, close, send and received
 * data on a socket in order to allow network communication.
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa http://www.linuxgazette.com/issue74/tougher.html
 * 
 * \todo write code example 
 */
#include "msgMESSAGE.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>


const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET();

    // Brief description of the destructor
    virtual ~msgSOCKET();
    
    virtual mcsINT32 GetDescriptor(void);
   
    // Initialisation
    virtual mcsCOMPL_STAT Open(unsigned short     *portNumberPt,
                               int                socketType);
    virtual mcsCOMPL_STAT Close(void);
    
    virtual mcsCOMPL_STAT Create(void);
    virtual mcsCOMPL_STAT Bind(const mcsINT32 port);
    virtual mcsCOMPL_STAT Listen(void);
    virtual mcsCOMPL_STAT Accept(msgSOCKET &socket) const;
    
    // Client initialization
    virtual mcsCOMPL_STAT Connect(const std::string host,
                                  const mcsINT32 port);

    // Data Transmission
    // String
    virtual mcsCOMPL_STAT Send(const std::string s) const;
    virtual mcsCOMPL_STAT Receive(std::string&) const;
    // msgMESSAGE
    virtual mcsCOMPL_STAT Send(msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Receive(msgMESSAGE &msg,
                                  mcsINT32 timeoutInMs);

    virtual mcsCOMPL_STAT IsValid(void);
    
    protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgSOCKET(const msgSOCKET&);
     msgSOCKET& operator=(const msgSOCKET&);

    mcsINT32 _descriptor;
    sockaddr_in _address;
};




#endif /*!msgSOCKET_H*/

/*___oOo___*/

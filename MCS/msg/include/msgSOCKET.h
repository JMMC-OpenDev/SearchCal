#ifndef msgSOCKET_H
#define msgSOCKET_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.h,v 1.9 2004-12-06 07:02:06 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    19-Nov-2004  Created
* lafrasse  23-Nov-2004  Comment refinments, and includes cleaning
* lafrasse  03-Dec-2004  Changed port number type from mcsINT32 to mcsUINT16
* gzins     06-Dec-2004  Declared copy constructor as public method
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
 * System Headers 
 */
#include <string>
#include <arpa/inet.h>


/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgMESSAGE.h"


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
 * \todo write code example 
 */
const int MAXHOSTNAME    = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV        = 500;

class msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET();
    msgSOCKET(const msgSOCKET &socket);

    // Brief description of the destructor
    virtual ~msgSOCKET();

    // Common initialisation
    virtual mcsCOMPL_STAT Create       (void);

    // Accessors
    virtual mcsINT32      GetDescriptor(void);
    virtual mcsLOGICAL    IsConnected  (void);

    // Server initialisation
    virtual mcsCOMPL_STAT Bind         (const mcsUINT16 port);
    virtual mcsCOMPL_STAT Listen       (void);
    virtual mcsCOMPL_STAT Accept       (msgSOCKET &socket) const;

    // Client initialization
    virtual mcsCOMPL_STAT Connect      (const std::string host,
                                        const mcsUINT16   port);

    // String-related Transmission
    virtual mcsCOMPL_STAT Send         (const std::string  string) const;
    virtual mcsCOMPL_STAT Receive      (      std::string& string) const;

    // msgMESSAGE-related Transmission
    virtual mcsCOMPL_STAT Send         (msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Receive      (msgMESSAGE &msg, mcsINT32 timeoutInMs);

    virtual mcsCOMPL_STAT Close        (void);

protected:
    
private:
    // Declaration of assignment operator as private methods, in order to it
    // them from the users.
     msgSOCKET& operator=(const msgSOCKET&);

    mcsINT32    _descriptor;  // Socket Descriptor
    sockaddr_in _address;     // Socket Data Structure
};

#endif /*!msgSOCKET_H*/

/*___oOo___*/

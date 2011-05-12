#ifndef msgSOCKET_H
#define msgSOCKET_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgSOCKET class 
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
class msgMESSAGE;


/*
 * Constant declaration
 */

/**
 * Message waiting constant, specifying to not wait on a recieve
 */
#define msgNO_WAIT               0   
/**
 * Message waiting constant, specifying to wait forever on a recieve
 */
#define msgWAIT_FOREVER         -1

const int MAXCONNECTIONS = 5;
const int MAXRECV        = 500;


/*
 * Class declaration
 */

/**
 * Class wrapping IPv4 BSD socket.
 * 
 * Provides methods to :
 * \li create a socket;
 * \li send data through a socket;
 * \li received data through a socket;
 * \li close a socket.
 */
class msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET();

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
    virtual mcsCOMPL_STAT Receive      (std::string& string,
                                        mcsINT32 timeoutInMs = msgNO_WAIT);

    // msgMESSAGE-related Transmission
    virtual mcsCOMPL_STAT Send         (msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Receive      (msgMESSAGE &msg,
                                        mcsINT32 timeoutInMs = msgNO_WAIT);

    virtual mcsCOMPL_STAT Close        (void);

protected:
    
private:
    // Declaration of copy const and assignment operator as private methods,
    // in order to it them from the users.
    msgSOCKET(const msgSOCKET &socket);
    msgSOCKET& operator=(const msgSOCKET&);

    mcsINT32    _descriptor;  // Socket Descriptor
    sockaddr_in _address;     // Socket Data Structure
};

#endif /*!msgSOCKET_H*/

/*___oOo___*/

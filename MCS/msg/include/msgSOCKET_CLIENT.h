#ifndef msgSOCKET_CLIENT_H
#define msgSOCKET_CLIENT_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_CLIENT.h,v 1.8 2004-12-07 07:41:59 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
* lafrasse  23-Nov-2004  Comment refinments, and includes cleaning
* lafrasse  03-Dec-2004  Changed port number type from mcsINT32 to mcsUINT16
* gzins     06-Dec-2004  Declared copy constructor as public method
* gzins     06-Dec-2004  Declared copy constructor as private method
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET_CLIENT class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgSOCKET.h"


/*
 * Class declaration
 */

/**
 * Client-side specialized object wrapper around system socket.
 * 
 * \sa msgTestSocketClient.cpp for a code example
 */
class msgSOCKET_CLIENT : public msgSOCKET
{
public:
    // Brief description of the constructor
    msgSOCKET_CLIENT();

    // Brief description of the destructor
    virtual ~msgSOCKET_CLIENT();

    // Open a client socket to a given server port number on a given host
    virtual mcsCOMPL_STAT Open(std::string host, mcsUINT16 port);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgSOCKET_CLIENT(const msgSOCKET_CLIENT&);
    msgSOCKET_CLIENT& operator=(const msgSOCKET_CLIENT&);
};

#endif /*!msgSOCKET_CLIENT_H*/

/*___oOo___*/

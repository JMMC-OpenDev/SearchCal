#ifndef msgSOCKET_CLIENT_H
#define msgSOCKET_CLIENT_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_CLIENT.h,v 1.4 2004-11-26 13:11:28 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
* lafrasse  23-Nov-2004  Comment refinments, and includes cleaning
*
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
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 * 
 * \sa http://www.linuxgazette.com/issue74/tougher.html
 * \sa msgSOCKET.cpp
 * 
 * \todo write code example
 */
class msgSOCKET_CLIENT : public msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET_CLIENT();

    // Brief description of the destructor
    virtual ~msgSOCKET_CLIENT();

    // Open a client socket to a given server port number on a given host
    virtual mcsCOMPL_STAT Open(std::string host, mcsINT32 port);

protected:

    
private:
     // Declaration of copy constructor and assignment operator as private
     // methods, in order to hide them from the users.
     msgSOCKET_CLIENT(const msgSOCKET_CLIENT&);
     msgSOCKET_CLIENT& operator=(const msgSOCKET_CLIENT&);
};




#endif /*!msgSOCKET_CLIENT_H*/

/*___oOo___*/

#ifndef msgSOCKET_CLIENT_H
#define msgSOCKET_CLIENT_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_CLIENT.h,v 1.3 2004-11-22 16:35:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
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
#include "msgSOCKET.h"
class msgSOCKET_CLIENT : public msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET_CLIENT();

    // Brief description of the destructor
    virtual ~msgSOCKET_CLIENT();

     virtual mcsCOMPL_STAT Open(std::string host,
                                mcsINT32 port);

protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgSOCKET_CLIENT(const msgSOCKET_CLIENT&);
     msgSOCKET_CLIENT& operator=(const msgSOCKET_CLIENT&);


};




#endif /*!msgSOCKET_CLIENT_H*/

/*___oOo___*/

#ifndef msgSOCKET_SERVER_H
#define msgSOCKET_SERVER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_SERVER.h,v 1.6 2004-12-01 12:54:39 lafrasse Exp $"
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
 * msgSOCKET_SERVER class declaration.
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
#include "msgSOCKET_SERVER.h"


/*
 * Class declaration
 */

/**
 * Server-side specialized object wrapper around system socket.
 *  
 * \sa msgTestSocketServer.cpp for a code example
 */
class msgSOCKET_SERVER : public msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET_SERVER();

    // Brief description of the destructor
    virtual ~msgSOCKET_SERVER();

    // Open a server socket on the given port number
    virtual mcsCOMPL_STAT Open(mcsINT32 port);

protected:

    
private:
     // Declaration of copy constructor and assignment operator as private
     // methods, in order to hide them from the users.
     msgSOCKET_SERVER(const msgSOCKET_SERVER&);
     msgSOCKET_SERVER& operator=(const msgSOCKET_SERVER&);
};


#endif /*!msgSOCKET_SERVER_H*/

/*___oOo___*/

#ifndef msgSOCKET_H
#define msgSOCKET_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.h,v 1.2 2004-11-22 14:46:00 scetre Exp $"
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
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa modcppMain.C
 * 
 * \bug OPTIONAL. Bugs list if it exists.
 * \bug For example, description of the first bug
 * \bug For example, description of the second bug
 * 
 * \todo OPTIONAL. Things to forsee list, if needed. For example, 
 * \todo add other methods, dealing with operations.
 * 
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
    virtual mcsCOMPL_STAT SetDescriptor(mcsINT32 descrip);
    
    // Server Initialisation
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

    virtual mcsCOMPL_STAT IsValid(void);

    virtual mcsCOMPL_STAT Open(unsigned short     *portNumberPt,
                               int                socketType);
    virtual mcsCOMPL_STAT Close(void);
    // msgMESSAGE
    virtual mcsCOMPL_STAT Send(msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Receive(msgMESSAGE &msg,
                                  mcsINT32 timeoutInMs);
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

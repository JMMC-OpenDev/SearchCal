#ifndef msgMANAGER_IF_H
#define msgMANAGER_IF_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.h,v 1.6 2004-12-03 08:47:33 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for _socket,
*                        and added the class description comment
* lafrasse  22-Nov-2004  Use msgSOCKET_CLIENT instead of system socket calls.
* lafrasse  01-Dec-2004  Comment refinments
* gzins     03-Dec-2004  Removed msgManagerHost param from Connect
*                        Gave default value to paramList and paramLen
*                        arguments of SendCommand
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class declaration.
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
#include "msgSOCKET_CLIENT.h"
#include "msgMESSAGE.h"
#include "msgErrors.h"


/*
 * Class declaration
 */

/**
 * msgMANAGER_IF is an interface class that allow to easily communicate with an
 * msgManager process.
 *
 * The main functionnalities are :
 * \li (dis)connection to an msgManager process;
 * \li transmission of messages (in the firm of msgMESSAGE objects) to it;
 * \li reception of messages from it;
 *
 * \sa msgSendCommand.cpp for a complete usage example.
 * 
 * \todo use msgSOCKET instead of the msg C function calls.
 * 
 */
class msgMANAGER_IF
{
public:
    msgMANAGER_IF();
    virtual ~msgMANAGER_IF();

    virtual mcsCOMPL_STAT Connect     (const mcsPROCNAME  procName);

    virtual mcsCOMPL_STAT SendCommand (const char        *command,
                                       const mcsPROCNAME  destProc,
                                       const char        *paramList=NULL,  
                                       mcsINT32           paramLen=0);
    virtual mcsCOMPL_STAT SendReply   (msgMESSAGE        &msg,
                                       mcsLOGICAL         lastReply);

    virtual mcsCOMPL_STAT Receive     (msgMESSAGE        &msg,
                                       const mcsINT32     timeoutInMs);

    virtual mcsLOGICAL    IsConnected (void);

    virtual mcsCOMPL_STAT Disconnect  (void);

    virtual mcsINT32      GetMsgQueue (void);
protected:

private:
    static msgSOCKET_CLIENT _socket; /* The network connection used to
                                      * communicate with the msgManager process
                                      */

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER_IF& operator=(const msgMANAGER_IF&);
    msgMANAGER_IF (const msgMANAGER_IF&);
};


#endif /*!msgMANAGER_IF_H*/

/*___oOo___*/

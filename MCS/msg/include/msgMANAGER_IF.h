#ifndef msgMANAGER_IF_H
#define msgMANAGER_IF_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.h,v 1.3 2004-11-22 14:31:04 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for _socket,
*                        and added the class description comment
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
#include "msgMESSAGE.h"


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

    virtual mcsCOMPL_STAT Connect     (const mcsPROCNAME  procName,
                                       const char        *msgManagerHost);

    virtual mcsCOMPL_STAT SendCommand (const char        *command,
                                       const mcsPROCNAME  destProc,
                                       const char        *buffer,  
                                       mcsINT32           bufLen);
    virtual mcsCOMPL_STAT SendReply   (msgMESSAGE        &msg,
                                       mcsLOGICAL         lastReply);

    virtual mcsCOMPL_STAT Receive     (msgMESSAGE        &msg,
                                       const mcsINT32     timeoutInMs);

    virtual mcsLOGICAL    IsConnected (void);

    virtual mcsCOMPL_STAT Disconnect  (void);

    virtual mcsINT32      GetMsgQueue (void);
protected:

private:
    static int _socket;

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER_IF& operator=(const msgMANAGER_IF&);
    msgMANAGER_IF (const msgMANAGER_IF&);
};


#endif /*!msgMANAGER_IF_H*/

/*___oOo___*/

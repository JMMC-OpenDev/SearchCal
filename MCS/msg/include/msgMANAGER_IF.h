#ifndef msgMANAGER_IF_H
#define msgMANAGER_IF_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMANAGER_IF.h,v 1.14 2005-05-19 15:04:58 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2005/02/14 07:59:01  gzins
 * Minor documentation changes
 *
 * Revision 1.12  2005/02/09 16:42:26  lafrasse
 * Added msgMESSAGE_FILTER class to manage message queues
 *
 * Revision 1.11  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.10  2005/01/29 19:58:17  gzins
 * Added únique' parameter to Connect()
 *
 * Revision 1.9  2005/01/24 15:39:54  gzins
 * Added CVS logs as modification history
 *
 * gzins     07-Jan-2005  Updated SendCommand prototype to return command Id
 * gzins     03-Dec-2004  Removed msgManagerHost param from Connect
 *                        Gave default value to paramList and paramLen
 *                        arguments of SendCommand
 * lafrasse  01-Dec-2004  Comment refinments
 * lafrasse  22-Nov-2004  Use msgSOCKET_CLIENT instead of system socket calls.
 * lafrasse  19-Nov-2004  Changed the class member name msgManagerSd for
 *                        _socket, and added the class description comment
 * lafrasse  18-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of msgMANAGER_IF class 
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System Headers 
 */
#include <queue>


/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgSOCKET_CLIENT.h"
#include "msgMESSAGE.h"
#include "msgMESSAGE_FILTER.h"
#include "msgErrors.h"


/*
 * Class declaration
 */

/**
 * msgMANAGER_IF is an interface class that allow to easily communicate with
 * remote processes through \<msgManager\>.
 *
 * The main functionnalities are :
 * \li (dis)connection to the \<msgManager\> process;
 * \li transmission of messages (in the form of msgMESSAGE objects) to remote
 * processes;
 * \li reception of messages from remote processes.
 *
 * \sa msgSendCommand.cpp code as a complete usage example.
 */
class msgMANAGER_IF
{
public:
    msgMANAGER_IF();
    virtual ~msgMANAGER_IF();

    // Connection/disconnection
    virtual mcsCOMPL_STAT Connect     (const mcsPROCNAME procName,
                                       const mcsLOGICAL unique = mcsFALSE);
    virtual mcsLOGICAL    IsConnected (void) const;
    virtual mcsCOMPL_STAT Disconnect  (void);
                       
    // Message sending
    virtual mcsINT32      SendCommand (const char*       command,
                                       const mcsPROCNAME destProc,
                                       const char*       paramList = NULL,  
                                       const mcsINT32    paramLen = 0);
    virtual mcsCOMPL_STAT SendReply   (msgMESSAGE&      msg,
                                       const mcsLOGICAL lastReply);

                       
    // Message receiving
    virtual mcsCOMPL_STAT Receive (msgMESSAGE& msg,
                                   mcsINT32    timeoutInMs);
    virtual mcsCOMPL_STAT Receive (msgMESSAGE&              msg,
                                   mcsINT32                 timeoutInMs,
                                   const msgMESSAGE_FILTER& filter);
                                                
    // Message queue
    virtual mcsUINT32     GetNbQueuedMessages      (void) const;
    virtual mcsCOMPL_STAT GetNextQueuedMessage  (msgMESSAGE& msg);

    // Socket descriptor
    virtual mcsINT32      GetSocketDescriptor   (void) const;

protected:

private:
    static msgSOCKET_CLIENT  _socket;             /* The network connection used
                                                   * to communicate with
                                                   * msgManager process
                                                   */

    static std::queue<msgMESSAGE> _messageQueue;  /* The FIFO stack used to hold
                                                   * waiting messages
                                                   */

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER_IF& operator=(const msgMANAGER_IF&);
    msgMANAGER_IF (const msgMANAGER_IF&);
};


#endif /*!msgMANAGER_IF_H*/

/*___oOo___*/

#ifndef msgMANAGER_H
#define msgMANAGER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMANAGER.h,v 1.8 2005-02-14 07:59:01 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/02/09 16:36:13  lafrasse
 * minor indentation refinments
 *
 * Revision 1.6  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.5  2005/01/29 09:56:46  gzins
 * Updated to notify client when a server is exiting abnormally
 *
 * Revision 1.4  2005/01/26 08:47:18  gzins
 * Added PrepareReply to fix bug related to wrong message type when sending reply to sender.
 *
 * Revision 1.3  2005/01/24 15:39:54  gzins
 * Added CVS logs as modification history
 *
 * gzins     15-Dec-2004  Set mcsTRUE as default value of lastReply parameter
 *                        of SendReply method
 * gzins     06-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of msgMANAGER class 
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/* 
 * System Headers 
 */
#include <list>

/*
 * MCS headers
 */
#include "mcs.h"

/*
 * Local headers
 */
#include "msgMESSAGE.h"
#include "msgSOCKET_SERVER.h"
#include "msgPROCESS_LIST.h"

/*
 * Class declaration
 */

/**
 * MCS \<msgManager\> message manager process main class.
 * 
 * This msgMANAGER class is the communication server core, allowing messages
 * exchange between processes. Each process connected to this server can send
 * messages to (and received messages from) all the other connected processes.
 */
class msgMANAGER
{
public:
    // Class constructor
    msgMANAGER();

    // Class destructor
    virtual ~msgMANAGER();

     // Initialization
    virtual mcsCOMPL_STAT Init(int argc, char *argv[]);

    // Main loop
    virtual mcsCOMPL_STAT MainLoop();
    
protected:
    // Command-line parameters
    virtual mcsCOMPL_STAT ParseOptions(mcsINT32 argc, char *argv[]);
    virtual mcsCOMPL_STAT Usage(void);

    // Get SW version
    virtual const char   *GetSwVersion(void);

    // Accept and register a new connection
    virtual mcsCOMPL_STAT AcceptConnection();

    // Send command/reply
    virtual mcsCOMPL_STAT Forward     (msgMESSAGE &msg);
    virtual mcsCOMPL_STAT PrepareReply(msgMESSAGE &msg,
                                       mcsLOGICAL lastReply=mcsTRUE);
    virtual mcsCOMPL_STAT SendReply   (msgMESSAGE &msg,
                                       msgPROCESS *sender=NULL);

    // Internal commands
    virtual mcsCOMPL_STAT HandleCmd   (msgMESSAGE &msg);
    
    // Management of processes which are waiting for reply
    virtual mcsCOMPL_STAT ReleaseWaitingProcess  (mcsINT32 procDescriptor);
    virtual mcsCOMPL_STAT RemoveProcessWaitingFor(mcsINT32 commandId);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER(const msgMANAGER&);
    msgMANAGER& operator=(const msgMANAGER&);

    // Connection socket
    msgSOCKET_SERVER _connectionSocket;

    // List of connected processes
    msgPROCESS_LIST  _processList;

    // List of waited command replies
    list< pair<int, msgMESSAGE> > _waitedCmdReplyList;
    
};

#endif /*!msgMANAGER_H*/

/*___oOo___*/

#ifndef msgMANAGER_H
#define msgMANAGER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

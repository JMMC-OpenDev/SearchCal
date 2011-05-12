#ifndef msgMANAGER_IF_H
#define msgMANAGER_IF_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

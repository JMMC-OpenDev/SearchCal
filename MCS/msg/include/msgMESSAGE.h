#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE.h,v 1.17 2005-01-24 15:39:54 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    17-Nov-2004  Created
 * lafrasse  19-Nov-2004  Changed all method name first letter to upper case,
 *                        and re-commented
 * lafrasse  22-Nov-2004  Added void type for functions without parameters
 * lafrasse  23-Nov-2004  Moved isInternal from msgMESSAGE_RAW to _isInternal in
 *                        msgMESSAGE, added SetLastReplyFlag method
 * scetre    30-Nov-2004  Set message body size to 32000
 * lafrasse  01-Dec-2004  Comment refinments
 * gzins     06-Dec-2004  Updated to be only C++
 * gzins     08-Dec-2004  Added senderId and messageId, with associated methods
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer, and removed unused API
 * gzins     15-Dec-2004  Added _NAME to command name definitions
 * gzins     15-Dec-2004  Removed msgDEBUG_CMD_NAME definition (defined in
 *                        msgDEBUG_CMD.h)
 * gzins     22-Dec-2004  Renamed GetBodyPtr to GetBody
 *                        Removed GetHeaderPtr
 *                        Declared AllocateBody as private
 *                        Renamed isInternal to IsInternal
 *                        Added ClearBody and AppendToBody
 *                        Declared msgSOCKET::Send and msgSOCKET::Receive as
 *                        friend
 * gzins     07-Jan-2005  Changed messageId to commandId
 *
 ******************************************************************************/

/**
 * \file
 * msgMESSAGE class declaration.
 */

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "misc.h"

#include "msgSOCKET.h"
/* 
 * Constants definition
 */

/**
 * Message waiting constants
 */
#define msgNO_WAIT               0   
#define msgWAIT_FOREVER         -1

/**
 * Standard command names
 */
#define msgPING_CMD_NAME         "PING"
#define msgEXIT_CMD_NAME         "EXIT"
#define msgVERSION_CMD_NAME      "VERSION"

/* 
 * Macro definition
 */

/**
 * Message header size
 */
#define msgHEADERLEN             (sizeof(msgHEADER))

/**
 * Maximum message body size
 */
#define msgBODYMAXLEN            (msgMAXLEN - msgHEADERLEN - 1)

/*
 * Enumeration type definition
 */

/**
 * Message types enumeration
 */
typedef enum
{
    msgTYPE_COMMAND = 1,         /**< Describe command messages. */
    msgTYPE_REPLY,               /**< Describe reply messages. */
    msgTYPE_ERROR_REPLY          /**< Describe error reply messages. */
} msgTYPE;

/* 
 * Structure type definition
 */

/**
 * Message header structure
 */
typedef struct
{
    mcsPROCNAME sender;          /**< Sender processus name */
    mcsENVNAME  senderEnv;       /**< Sender environnement */
    mcsSTRING8  senderId;        /**< Sender Id */
    mcsPROCNAME recipient;       /**< Receiver processus name  */
    mcsENVNAME  recipientEnv;    /**< Receiver environnement */
    msgTYPE     type;            /**< Message type */
    mcsCMD      command;         /**< Command name */
    mcsSTRING16 commandId;       /**< Command Id */
    mcsLOGICAL  lastReply;       /**< TRUE if it is the last answer */
    mcsBYTES32  timeStamp;       /**< Message date */
    mcsSTRING8  msgBodySize;     /**< Message body size */
} msgHEADER;

/*
 * System Headers 
 */
#include <iostream>

/*
 * Class declaration
 */

/**
 * Class handling MCS messages.
 *  
 * The msgMESSAGE class contains all methods necessary for handling messages
 * (commands and replies) which are exchanged between processes through the
 * MCS message service. A message is defined by : 
 *   - \em sender \n
 *     is the MCS registering name of the process which send the message
 *   - \em sender environment \n
 *     is the environment name in which the sender is running
 *   - \em recipient \n
 *     is the MCS registering name of the process to which the message is
 *     intended
 *   - \em recipient environment \n
 *     is the environment name in which the recipient is running
 *   - \em type \n
 *     is the message type (see msgTYPE)
 *   - \em command \n
 *     is the name of the command
 *   - \em params \n
 *     is the command parameters
 *
 * The class msgMANAGER_IF can be used for reception  and sending of MCS
 * messages.
 *
 * \sa msgMANAGER_IF
 */

class msgMESSAGE
{

public:
    // Constructor
    msgMESSAGE                               (const mcsLOGICAL isInternalMsg
                                              = mcsFALSE);
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMESSAGE(msgMESSAGE&);
    msgMESSAGE&              operator=(msgMESSAGE&);

    // Destructor
    virtual ~msgMESSAGE                      (void);
    
    // Accessors
    virtual char*            GetSender       (void);
    virtual mcsCOMPL_STAT    SetSender       (const char     *buffer);

    virtual char*            GetSenderEnv    (void);
    virtual mcsCOMPL_STAT    SetSenderEnv    (const char     *senderEnv);

    virtual mcsINT32         GetSenderId     (void);
    virtual mcsCOMPL_STAT    SetSenderId     (const mcsINT32 id);

    virtual char*            GetRecipient    (void);
    virtual mcsCOMPL_STAT    SetRecipient    (const char     *recipient);

    virtual char*            GetRecipientEnv (void);
    virtual mcsCOMPL_STAT    SetRecipientEnv (const char     *recipientEnv);

    virtual msgTYPE          GetType         (void);
    virtual mcsCOMPL_STAT    SetType         (const msgTYPE   type);

    virtual mcsINT32         GetCommandId    (void);
    virtual mcsCOMPL_STAT    SetCommandId    (const mcsINT32 id);

    virtual char*            GetCommand      (void);
    virtual mcsCOMPL_STAT    SetCommand      (const char     *command);

    virtual mcsLOGICAL       IsLastReply     (void);
    virtual mcsCOMPL_STAT    SetLastReplyFlag(mcsLOGICAL      flag);

    virtual mcsLOGICAL       IsInternal      (void);

    virtual char*            GetBody         (void);
    virtual mcsINT32         GetBodySize     (void);
    virtual mcsCOMPL_STAT    ClearBody       (void);
    virtual mcsCOMPL_STAT    SetBody         (const char *buffer,
                                              mcsUINT32  bufLen=0);
    virtual mcsCOMPL_STAT    AppendToBody    (const char *buffer,
                                              mcsUINT32  bufLen=0);

    virtual void             Display         (void);

    friend mcsCOMPL_STAT     msgSOCKET::Send(msgMESSAGE &msg);
    friend mcsCOMPL_STAT     msgSOCKET::Receive(msgMESSAGE &msg,
                                                mcsINT32   timeoutInMs);

protected:

    
private:
    virtual mcsCOMPL_STAT    AllocateBody    (const mcsUINT32 bufLen);

    // The members
    msgHEADER   _header;     // The complete message header
    miscDYN_BUF _body;       // A convenient pointer to the _message body

    mcsLOGICAL  _isInternal; // A flag to say weither the message is of
                             // internal process use or not (see evh module)
};

#endif /*!msgMESSAGE_H*/
/*___oOo___*/

#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.h,v 1.12 2004-12-15 09:57:54 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed all method name first letter to upper case, and
*                        re-commented
* lafrasse  22-Nov-2004  Added void type for functions without parameters
* lafrasse  23-Nov-2004  Moved isInternal from msgMESSAGE_RAW to _isInternal in
*                        msgMESSAGE, added SetLastReplyFlag method
* scetre    30-Nov-2004  Set message body size to 32000
* lafrasse  01-Dec-2004  Comment refinments
* gzins     06-Dec-2004  Updated to be only C++
* gzins     08-Dec-2004  Added senderId and messageId, with associated methods
* gzins     15-Dec-2004  Added _NAME to command name definitions
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class declaration.
 */

/*
 * MCS Headers 
 */
#include "mcs.h"

/* 
 * Constants definition
 */

/**
 * Message waiting constants
 */
#define msgNO_WAIT               0   
#define msgWAIT_FOREVER         -1

/**
 * Maximum message size
 */
#define msgMAXLEN                32000

/**
 * Standard command names
 */
#define msgDEBUG_CMD_NAME        "DEBUG"
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
    mcsSTRING16 messageId;       /**< Message Id */
    msgTYPE     type;            /**< Message type */
    mcsCMD      command;         /**< Command name */
    mcsLOGICAL  lastReply;       /**< TRUE if it is the last answer */
    mcsBYTES32  timeStamp;       /**< Message date */
    mcsSTRING8  msgBodySize;     /**< Message body size */
} msgHEADER;

/**
 * Complete message structure
 */
typedef struct
{
    msgHEADER  header;
    char       body[msgMAXLEN - sizeof(msgHEADER)];

} msgMESSAGE_RAW;

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

    virtual mcsINT32         GetMessageId    (void);
    virtual mcsCOMPL_STAT    SetMessageId    (const mcsINT32 id);

    virtual char*            GetCommand      (void);
    virtual mcsCOMPL_STAT    SetCommand      (const char     *command);

    virtual mcsLOGICAL       IsLastReply     (void);
    virtual mcsCOMPL_STAT    SetLastReplyFlag(mcsLOGICAL      flag);

    virtual mcsLOGICAL       isInternal      (void);

    virtual msgHEADER*       GetHeaderPtr    (void);

    virtual char*            GetBodyPtr      (void);
    virtual mcsINT32         GetBodySize     (void);
    virtual mcsCOMPL_STAT    SetBody         (const char     *buffer,
                                              const mcsINT32  bufLen=0);

    virtual msgMESSAGE_RAW*  GetMessagePtr   (void);
    virtual mcsCOMPL_STAT    SetMessage      (const msgMESSAGE_RAW* message);

    virtual void             Display         (void);

protected:

    
private:
     // The members
     msgMESSAGE_RAW _message;    // The complete message structure
     msgHEADER*     _header;     // A convenient pointer to the _message header
     char*          _body;       // A convenient pointer to the _message body

     mcsLOGICAL     _isInternal; /* A flag to say weither the message is of
                                  * internal process use or not (see evh module)
                                  */

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgMESSAGE(const msgMESSAGE&);
     msgMESSAGE& operator=(const msgMESSAGE&);
};

#endif /*!msgMESSAGE_H*/

/*___oOo___*/

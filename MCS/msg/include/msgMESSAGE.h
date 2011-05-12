#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgMESSAGE class 
 */


/*
 * System Headers 
 */
#include <iostream>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "misc.h"

/*
 * Local Headers 
 */
#include "msgSOCKET.h"


/* 
 * Constants definition
 */


/**
 * Standard command, used to test wether a remote process is available or not
 */
#define msgPING_CMD_NAME         "PING"
/**
 * Standard command, used to shut down a remote process
 */
#define msgEXIT_CMD_NAME         "EXIT"
/**
 * Standard command, used to get a remote process version number
 */
#define msgVERSION_CMD_NAME      "VERSION"


/* 
 * Macro definition
 */

/**
 * Message header size
 */
#define msgHEADERLEN             (sizeof(msgHEADER))


/*
 * Enumeration type definition
 */

/**
 * Message types enumeration
 */
typedef enum
{
    msgTYPE_COMMAND = 1,         /**< Characterize command messages. */
    msgTYPE_REPLY,               /**< Characterize reply messages. */
    msgTYPE_ERROR_REPLY          /**< Characterize error reply messages. */
} msgTYPE;


/* 
 * Structure type definition
 */

/**
 * MCS message network header structure.
 *
 * This structure holds all the data used by the 'Msg' library and  the
 * \<msgManager\> process to route and deliver messages amongst all the remote
 * processes.
 *
 * \sa msgTYPE, 'Cmd' module
 */
typedef struct
{
    mcsPROCNAME sender;          /**< the MCS registering name of the process
                                   *  that wants to send the message
                                   */
    mcsENVNAME  senderEnv;       /**< the MCS environment name in which the
                                   *  \b sender is running
                                   */
    mcsSTRING8  senderId;        /**< the \b sender identifier */

    mcsPROCNAME recipient;       /**< the MCS registering name of the process to
                                   *  which the message is intended
                                   */
    mcsENVNAME  recipientEnv;    /**< the MCS environment name in which the
                                   *  \b recipient is running
                                   */

    msgTYPE     type;            /**< the message type (picked amongs those
                                   *   defined in the msgTYPE enumeration)
                                   */

    mcsCMD      command;         /**< the message command name (see the 'Cmd'
                                   *  module for more information about commands
                                   */
    mcsSTRING16 commandId;       /**< the message command identifier */

    mcsINT8     lastReply;       /**< the flag specifying wether the message is 
                                   *  a 'last reply' one or not (equals 'T' if 
                                   *  it is the last answer of a dialog)
                                   */

    mcsBYTES32  timeStamp;       /**< the message date */

    mcsSTRING8  msgBodySize;     /**< the message payload size */

} msgHEADER;


/*
 * Class declaration
 */

/**
 * MCS messages container class, holding the message header and data.
 *  
 * The msgMESSAGE class provides all the necessary methods for accessing and
 * storing messages data that are exchanged between processes through the
 * \<msgManager\>.\n\n
 * Each message object holds an msgHEADER structure, followed by a miscDYN_BUF
 * to store the dynamically sized message payload.
 *
 * The msgMANAGER_IF class can be used as a facility to sending and receiving
 * msgMessage objects.
 *
 * \sa msgHEADER, miscDYN_BUF, msgMANAGER_IF
 */

class msgMESSAGE
{

public:
    // Constructor
    msgMESSAGE (const mcsLOGICAL isInternalMsg = mcsFALSE);

    // Copy constructor and assignment operator 
    msgMESSAGE(const msgMESSAGE&);
    msgMESSAGE& operator=(const msgMESSAGE&);

    // Destructor
    virtual ~msgMESSAGE                      (void);
    
    // Accessors
    virtual const char      *GetSender       (void) const;
    virtual mcsCOMPL_STAT    SetSender       (const char     *sender);

    virtual const char      *GetSenderEnv    (void) const;
    virtual mcsCOMPL_STAT    SetSenderEnv    (const char     *senderEnv);

    virtual mcsINT32         GetSenderId     (void) const;
    virtual mcsCOMPL_STAT    SetSenderId     (const mcsINT32 identifier);

    virtual const char      *GetRecipient    (void) const;
    virtual mcsCOMPL_STAT    SetRecipient    (const char     *recipient);

    virtual const char      *GetRecipientEnv (void) const;
    virtual mcsCOMPL_STAT    SetRecipientEnv (const char     *recipientEnv);

    virtual msgTYPE          GetType         (void) const;
    virtual mcsCOMPL_STAT    SetType         (const msgTYPE   type);

    virtual mcsINT32         GetCommandId    (void) const;
    virtual mcsCOMPL_STAT    SetCommandId    (const mcsINT32 identifier);

    virtual const char      *GetCommand      (void) const;
    virtual mcsCOMPL_STAT    SetCommand      (const char     *command);

    virtual mcsLOGICAL       IsLastReply     (void) const;
    virtual mcsCOMPL_STAT    SetLastReplyFlag(mcsLOGICAL      flag);

    virtual mcsLOGICAL       IsInternal      (void) const; 

    virtual const char      *GetBody         (void) const;
    virtual mcsINT32         GetBodySize     (void) const;
    virtual mcsCOMPL_STAT    ClearBody       (void);
    virtual mcsCOMPL_STAT    SetBody         (const char *buffer,
                                              mcsUINT32  bufLen=0);
    virtual mcsCOMPL_STAT    SetBodyArgs(const char *format, ...);
    virtual mcsCOMPL_STAT    AppendToBody    (const char *buffer,
                                              mcsUINT32  bufLen=0);
    virtual mcsCOMPL_STAT    AppendStringToBody(const char *str); 

    friend  std::ostream&    operator<<      (       std::ostream& stream,
                                               const msgMESSAGE&   message);

    friend mcsCOMPL_STAT     msgSOCKET::Send(msgMESSAGE &msg);
    friend mcsCOMPL_STAT     msgSOCKET::Receive(msgMESSAGE &msg,
                                                mcsINT32   timeoutInMs);

protected:

    
private:
    virtual mcsCOMPL_STAT    AllocateBody    (const mcsUINT32 bufLen);

    // The members
    msgHEADER   _header;     // The complete message header
    miscDYN_BUF _body;       // A convenient pointer to the _message body

    mcsLOGICAL  _isInternal; // A flag to say wether the message is of
                             // internal process use or not (see evh module)
};

#endif /*!msgMESSAGE_H*/
/*___oOo___*/

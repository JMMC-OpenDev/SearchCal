#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE.h,v 1.25 2005-02-14 07:59:01 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.24  2005/02/09 16:34:12  lafrasse
 * Changed method prototypes to use as much 'const' parameters as possible
 *
 * Revision 1.23  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.22  2005/02/03 06:51:42  gzins
 * Defined IsInternal method as constant
 *
 * Revision 1.21  2005/01/29 19:54:46  gzins
 * Added AppendStringToBody method
 *
 * Revision 1.20  2005/01/29 10:05:06  gzins
 * Changed msgMESSAGE.lastReply type from mcsLOGICAL to mcsSTRING8
 *
 * Revision 1.19  2005/01/29 07:17:59  gzins
 * Fixed wrong message body initialization in constructor
 *
 * Revision 1.18  2005/01/28 23:50:00  gzins
 * Defined GetBody and GetBodySize as constant method
 *
 * Revision 1.17  2005/01/24 15:39:54  gzins
 * Added CVS logs as modification history
 *
 * gzins     07-Jan-2005  Changed messageId to commandId
 * gzins     22-Dec-2004  Renamed GetBodyPtr to GetBody
 *                        Removed GetHeaderPtr
 *                        Declared AllocateBody as private
 *                        Renamed isInternal to IsInternal
 *                        Added ClearBody and AppendToBody
 *                        Declared msgSOCKET::Send and msgSOCKET::Receive as
 *                        friend
 * gzins     15-Dec-2004  Removed msgDEBUG_CMD_NAME definition (defined in
 *                        msgDEBUG_CMD.h)
 * gzins     15-Dec-2004  Added _NAME to command name definitions
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer, and removed unused API
 * gzins     08-Dec-2004  Added senderId and messageId, with associated methods
 * gzins     06-Dec-2004  Updated to be only C++
 * lafrasse  01-Dec-2004  Comment refinments
 * scetre    30-Nov-2004  Set message body size to 32000
 * lafrasse  23-Nov-2004  Moved isInternal from msgMESSAGE_RAW to _isInternal in
 *                        msgMESSAGE, added SetLastReplyFlag method
 * lafrasse  22-Nov-2004  Added void type for functions without parameters
 * lafrasse  19-Nov-2004  Changed all method name first letter to upper case,
 *                        and re-commented
 * scetre    17-Nov-2004  Created
 *
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
 * Message waiting constant, specifying to not wait on a recieve
 */
#define msgNO_WAIT               0   
/**
 * Message waiting constant, specifying to wait forever on a recieve
 */
#define msgWAIT_FOREVER         -1

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

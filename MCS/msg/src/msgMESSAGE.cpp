/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.cpp,v 1.1 2004-11-19 17:19:45 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class definition.
 */

static char *rcsId="@(#) $Id: msgMESSAGE.cpp,v 1.1 2004-11-19 17:19:45 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>
#include <netinet/in.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msg.h"
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"

/*
 * Class constructor
 */
msgMESSAGE::msgMESSAGE(const mcsLOGICAL isInternal)
{
    // The message is considered extern by default
    _message.header.isInternal = isInternal;
}



/*
 * Class destructor
 */
msgMESSAGE::~msgMESSAGE()
{
}

/*
 * Public methods
 */
/**
 * Return the address of the internal msgMESSAGE_RAW structure.
 *
 * \return the address of the internal msgMESSAGE_RAW structure
 */
msgMESSAGE_RAW*  msgMESSAGE::GetMessageRaw ()
{
    logExtDbg("msgMESSAGE::GetMessageRaw()");
    /* Return the message internal structure */
    return &_message;
}

/**
 * Return the address of the message sender processus name.
 *
 * \return the address of the message sender processus name
 */
char *msgMESSAGE::GetSender()
{
    logExtDbg("msgMESSAGE::GetSender()");
    /* Return the message sender processus name address */
    return _message.header.sender;
}

/**
 * Return the address of the message sender processus environnement.
 *
 * \return the address of the message sender processus environnement
 */
char *msgMESSAGE::GetSenderEnv()
{
    logExtDbg("msgMESSAGE::GetSenderEnv()");
    /* Return the message sender processus name environnement */
    return _message.header.senderEnv;
}

/**
 * Return the address of the message receiver processus name.
 *
 * \return the address of the message receiver processus name
 */
char *msgMESSAGE::GetRecipient()
{
    logExtDbg("msgMESSAGE::GetRecipient()");
    /* Return the message receiver processus name address */
    return _message.header.recipient;
}

/**
 * Return the address of the message recipient processus environnement.
 *
 * \return the address of the message recipient processus environnement
 */
char *msgMESSAGE::GetRecipientEnv()
{
    logExtDbg("msgMESSAGE::GetRecipientEnv()");
    /* Return the message recipient processus name environnement */
    return _message.header.recipientEnv;
}

/**
 * Return the identifier value of the message.
 *
 * \return the identifier value of the message
 */
char *msgMESSAGE::GetIdentifier()
{
    logExtDbg("msgMESSAGE::GetIdentifier()");
    /* Return the identificator value of the message */
    return _message.header.identifier;
}

/**
 * Return the message type.
 *
 * \return the message type
 */
int msgMESSAGE::GetType()
{
    logExtDbg("msgMESSAGE::GetType()");
    /* Return the message type */
    return _message.header.type;
}

/**
 * Return the address of the message command name.
 *
 * \return the address of the message command name
 */
char *msgMESSAGE::GetCommand()
{
    logExtDbg("msgMESSAGE::GetCommand()");
    /* Return the message command name address */
    return _message.header.command;
}

/**
 * Return weither it is the last message or not.
 *
 * \return mcsTRUE or mcsFALSE
 */
mcsLOGICAL msgMESSAGE::IsLastReply()
{
    logExtDbg("msgMESSAGE::IsLastReply()");
    /* Return weither it is the last message or not */
    return _message.header.lastReply;
}

/**
 * Return weither it is an external message or not.
 *
 * \return mcsTRUE or mcsFALSE
 */
mcsLOGICAL msgMESSAGE::IsInternal()
{
    logExtDbg("msgMESSAGE::IsInternal()");
    /* Return weither it is an external message or not */
    return _message.header.isInternal;
}


/**
 * Return the address of the message body.
 *
 * \return the address of the message body
 */
char* msgMESSAGE::GetBodyPtr()
{
    logExtDbg("msgMESSAGE::GetBodyPtr()");
    /* Return the message body address */
    return _message.body;
}

/**
 * Return the address of the message header.
 *
 * \return the address of the message header
 */
char* msgMESSAGE::GetHeaderPtr()
{
    /* Return the message body address */
    return ((char*)(&_message.header));
}

/**
 * Return the message body size.
 *
 * \return the message body size
 */
mcsINT32 msgMESSAGE::GetBodySize()
{
    logExtDbg("msgMESSAGE::GetBodySize()");
    mcsINT32 msgBodySize;
    /* Return the message body size in local host byte order */
    sscanf(_message.header.msgBodySize, "%d", &msgBodySize);
    return msgBodySize;
}

/**
 * Copy \<bufLen\> bytes of \<buffer\> in the message body.
 *
 * If \<bufLen\> equal 0, strlen() is used to get \<buffer\> length to be
 * copied in.
 * If \<buffer\> equal NULL, the message body is resetted with '\\0'.
 *
 * \param buffer the byte buffer to be copied in
 * \param bufLen the size to be copied in
 *
 * \return FAILURE if the to-be-copied-in byte number is greater than the
 * message body maximum size, SUCCESS otherwise
 */
mcsCOMPL_STAT msgMESSAGE::SetBody(const char *buffer,
                                  const mcsINT32 bufLen)
{
    logExtDbg("msgMESSAGE::SetBody()");
    
    /* Reset the message body with '\0' */
    memset(_message.body, '\0', sizeof(_message.body));

    /* If there is nothing to copy in... */
    mcsINT32 trueBufLen = 0;
    if (buffer != NULL)
    {
        /* If no to-be-copied-in byte number is given... */
        if (bufLen == 0)
        {
            /* Get the given buffer total length */
            trueBufLen = strlen(buffer);
        }
        else
        {
            trueBufLen = bufLen;
        }
    }

    /* If the to-be-copied byte number is greater than the message body
     * maximum size...
     */
    if (trueBufLen > (mcsINT32)msgBODYMAXLEN)
    {
        /* Raise an error */
        errAdd(msgERR_BUFFER_TOO_BIG, trueBufLen, msgBODYMAXLEN);

        /* Return an error code */
        return FAILURE;
    }
    
    /* Store the new message body size, in network byte order */
    sprintf(_message.header.msgBodySize, "%d", bufLen);
    /* Fill the message body with the given length and buffer content */
    strncpy(_message.body, buffer, trueBufLen);
    
    return SUCCESS;
}

/**
 * Affect Sender name to the message
 *
 * \param sender the sender name to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSender(const char *sender)
{
    logExtDbg("msgMESSAGE::SetSender()");
    strcpy(_message.header.sender, sender);
    return SUCCESS;
}

/**
 * Affect Sender name environnement to the message
 *
 * \param senderEnv the sender environnement name to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSenderEnv(const char *senderEnv)
{
    logExtDbg("msgMESSAGE::SetSenderEnv()");
    strcpy(_message.header.senderEnv, senderEnv);
    return SUCCESS;
}

/**
 * Affect recipient name to the message
 *
 * \param recipient the recipient name to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipient(const char *recipient)
{
    logExtDbg("msgMESSAGE::SetRecipient()");
    strcpy(_message.header.recipient, recipient);
    return SUCCESS;
}

/**
 * Affect Recipient environnement name to the message
 *
 * \param recipientEnv the recipient environnement name to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipientEnv(const char *recipientEnv)
{
    logExtDbg("msgMESSAGE::SetRecipientEnv()");
    strcpy(_message.header.recipientEnv, recipientEnv);
    return SUCCESS;
}

/**
 * Affect identifier value to the message
 *
 * \param identifier the identifier value to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetIdentifier(const char *identifier)
{
    logExtDbg("msgMESSAGE::SetIdentifier()");
    strcpy(_message.header.identifier, identifier);
    return SUCCESS;
}

/**
 * Affect the type of the message
 *
 * \param type the type to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetType(const mcsUINT8 type)
{
    logExtDbg("msgMESSAGE::SetType()");
    _message.header.type = type;
    return SUCCESS;
}

/**
 * Affect command name to the message
 *
 * \param command the command name to affect
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetCommand(const char *command)
{
    logExtDbg("msgMESSAGE::SetCommand()");
    // Copy command value in the command parameter of the header message
    strcpy(_message.header.command, command);
    return SUCCESS;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

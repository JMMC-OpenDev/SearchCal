/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.cpp,v 1.3 2004-11-22 14:57:05 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
* lafrasse  19-Nov-2004  Re-commented, and replaced all the srtcpy by some
*                        strNcpy in order to avoid segmentation faults as far
*                        as possible
* lafrasse  22-Nov-2004  Added void type for functions without parameters
*
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class definition.
 */

static char *rcsId="@(#) $Id: msgMESSAGE.cpp,v 1.3 2004-11-22 14:57:05 lafrasse Exp $"; 
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
    // Reset all the message structure to 0
    memset(&_message, 0, sizeof(_message));

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
 * Return the sender processus name.
 *
 * \return the address of the message sender processus name
 */
char* msgMESSAGE::GetSender(void)
{
    logExtDbg("msgMESSAGE::GetSender()");

    // Return the sender processus name
    return _message.header.sender;
}

/**
 * Copy the given sender name into the message.
 *
 * \param sender the sender name to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSender(const char *sender)
{
    logExtDbg("msgMESSAGE::SetSender()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.sender, sender, sizeof(_message.header.sender));

    return SUCCESS;
}

/**
 * Return the message sender environnement name.
 *
 * \return the address of the message sender processus environnement name
 */
char* msgMESSAGE::GetSenderEnv(void)
{
    logExtDbg("msgMESSAGE::GetSenderEnv()");

    // Return the message sender environnement name
    return _message.header.senderEnv;
}

/**
 * Copy the given sender environnement name into the message.
 *
 * \param senderEnv the sender environnement name to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSenderEnv(const char *senderEnv)
{
    logExtDbg("msgMESSAGE::SetSenderEnv()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.senderEnv, senderEnv,
            sizeof(_message.header.senderEnv));

    return SUCCESS;
}

/**
 * Return the message receiver processus name.
 *
 * \return the address of the message receiver processus name
 */
char* msgMESSAGE::GetRecipient(void)
{
    logExtDbg("msgMESSAGE::GetRecipient()");

    // Return the message receiver processus name
    return _message.header.recipient;
}

/**
 * Copy the given recipient name into the message.
 *
 * \param recipient the recipient name to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipient(const char *recipient)
{
    logExtDbg("msgMESSAGE::SetRecipient()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.recipient, recipient,
            sizeof(_message.header.recipient));

    return SUCCESS;
}

/**
 * Return the recipient environnement name.
 *
 * \return the address of the message recipient processus environnement
 */
char* msgMESSAGE::GetRecipientEnv(void)
{
    logExtDbg("msgMESSAGE::GetRecipientEnv()");

    // Return the recipient environnement name
    return _message.header.recipientEnv;
}

/**
 * Copy given the recipient environnement name into the message.
 *
 * \param recipientEnv the recipient environnement name to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipientEnv(const char *recipientEnv)
{
    logExtDbg("msgMESSAGE::SetRecipientEnv()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.recipientEnv, recipientEnv,
            sizeof(_message.header.recipientEnv));

    return SUCCESS;
}

/**
 * Return the message type.
 * 
 * \sa msgTYPE, the enumeration that list all the possible message types
 *
 * \return the message type
 */
msgTYPE msgMESSAGE::GetType(void)
{
    logExtDbg("msgMESSAGE::GetType()");

    // Return the message type
    return _message.header.type;
}

/**
 * Set the type of the message.
 *
 * \param type the type of th message
 * 
 * \sa msgTYPE, the enumeration that list all the possible message types
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetType(const msgTYPE type)
{
    logExtDbg("msgMESSAGE::SetType()");

    // Copy the given value in the message header associated field
    _message.header.type = type;

    return SUCCESS;
}

/**
 * Return the message identifier value.
 *
 * \return the identifier value of the message
 */
char* msgMESSAGE::GetIdentifier(void)
{
    logExtDbg("msgMESSAGE::GetIdentifier()");

    // Return the message identifier value
    return _message.header.identifier;
}

/**
 * Copy the given identifier value into the message.
 *
 * \param identifier the identifier value to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetIdentifier(const char *identifier)
{
    logExtDbg("msgMESSAGE::SetIdentifier()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.identifier, identifier,
            sizeof(_message.header.identifier));

    return SUCCESS;
}

/**
 * Return the message command name.
 *
 * \return the address of the message command name
 */
char* msgMESSAGE::GetCommand(void)
{
    logExtDbg("msgMESSAGE::GetCommand()");

    // Return the message command name
    return _message.header.command;
}

/**
 * Copy the given command name into the message.
 *
 * \param command the command name to be copied in
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetCommand(const char *command)
{
    logExtDbg("msgMESSAGE::SetCommand()");

    // Copy the given value in the message header associated field
    strncpy(_message.header.command, command, sizeof(_message.header.command));

    return SUCCESS;
}

/**
 * Return weither the current message is the last one or not.
 *
 * \return mcsTRUE if the message is the last one, mcsFALSE othewise
 */
mcsLOGICAL msgMESSAGE::IsLastReply(void)
{
    logExtDbg("msgMESSAGE::IsLastReply()");

    // Return weither the current message is the last one or not
    return _message.header.lastReply;
}

/**
 * Return weither the current message is an internal one or not.
 *
 * \return mcsTRUE if the message is internal, mcsFALSE otherwise
 */
mcsLOGICAL msgMESSAGE::IsInternal(void)
{
    logExtDbg("msgMESSAGE::IsInternal()");

    // Return weither the current message is an internal one or not
    return _message.header.isInternal;
}

/**
 * Return a pointer to the message header.
 *
 * \return the address of the message header
 */
msgHEADER* msgMESSAGE::GetHeaderPtr(void)
{
    logExtDbg("msgMESSAGE::GetHeaderPtr()");

    // Return a pointer to the message header
    return &_message.header;
}

/**
 * Return a pointer to the message body.
 *
 * \return the address of the message body
 */
char* msgMESSAGE::GetBodyPtr(void)
{
    logExtDbg("msgMESSAGE::GetBodyPtr()");

    // Return a pointer to the message body
    return _message.body;
}

/**
 * Return the message body size.
 *
 * \return the message body size
 */
mcsINT32 msgMESSAGE::GetBodySize(void)
{
    logExtDbg("msgMESSAGE::GetBodySize()");

    // Return the message body size in the localhost byte order
    mcsINT32 msgBodySize = 0;
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
    
    // If there is nothing to copy in...
    mcsINT32 trueBufLen = 0;
    if (buffer != NULL)
    {
        // If no to-be-copied-in byte number is given...
        if (bufLen == 0)
        {
            // Get the given buffer total length
            trueBufLen = strlen(buffer);
        }
        else
        {
            trueBufLen = bufLen;
        }
    }

    // If the to-be-copied byte number is greater than the message body
    // maximum size...
    if (trueBufLen > (mcsINT32)msgBODYMAXLEN)
    {
        // Raise an error
        errAdd(msgERR_BUFFER_TOO_BIG, trueBufLen, msgBODYMAXLEN);

        // Return an error code
        return FAILURE;
    }
    
    // Store the new message body size, in network byte order
    sprintf(_message.header.msgBodySize, "%d", bufLen);
    // Fill the message body with the given length and buffer content
    strncpy(_message.body, buffer, trueBufLen);
    
    return SUCCESS;
}

/**
 * Return a pointer to the message internal msgMESSAGE_RAW structure.
 *
 * \return the address of the internal msgMESSAGE_RAW structure
 */
msgMESSAGE_RAW*  msgMESSAGE::GetMessageRaw(void)
{
    logExtDbg("msgMESSAGE::GetMessageRaw()");

    // Return a pointer to the message internal structure
    return &_message;
}

/*___oOo___*/

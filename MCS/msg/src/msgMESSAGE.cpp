/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.cpp,v 1.8 2004-12-07 07:47:23 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
* lafrasse  19-Nov-2004  Re-commented, and replaced all the srtcpy by some
*                        strNcpy in order to avoid segmentation faults as far
*                        as possible
* lafrasse  22-Nov-2004  Added void type for functions without parameters
* lafrasse  23-Nov-2004  Moved isInternal from msgMESSAGE_RAW to _isInternal in
*                        msgMESSAGE, added SetLastReplyFlag method
* lafrasse  01-Dec-2004  Added error management code, comment refinments, and
*                        includes cleaning
* gzins     03-Dec-2004  Improved parameter check in SetBody method
* gzins     07-Dec-2004  Removed invalid parameters from Display documentation 
*
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class definition.
 */

static char *rcsId="@(#) $Id: msgMESSAGE.cpp,v 1.8 2004-12-07 07:47:23 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/*
 * Class constructor
 */
msgMESSAGE::msgMESSAGE(const mcsLOGICAL isInternalMsg)
{
    // Reset all the message structure to 0
    memset(&_message, 0, msgMAXLEN);

    // Initializing short-cuts
    _header = &(_message.header);
    _body   =   _message.body;

    // The message is considered extern by default
    _isInternal = isInternalMsg;
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
    return _header->sender;
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
    strncpy(_header->sender, sender, sizeof(_header->sender));

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
    return _header->senderEnv;
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
    strncpy(_header->senderEnv, senderEnv, sizeof(_header->senderEnv));

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
    return _header->recipient;
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
    strncpy(_header->recipient, recipient, sizeof(_header->recipient));

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
    return _header->recipientEnv;
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
    strncpy(_header->recipientEnv, recipientEnv, sizeof(_header->recipientEnv));

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
    return _header->type;
}

/**
 * Set the type of the message.
 *
 * \param type the type of the message
 * 
 * \sa msgTYPE, the enumeration that list all the possible message types
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetType(const msgTYPE type)
{
    logExtDbg("msgMESSAGE::SetType()");

    // Copy the given value in the message header associated field
    _header->type = type;

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
    return _header->identifier;
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
    strncpy(_header->identifier, identifier, sizeof(_header->identifier));

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
    return _header->command;
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
    strncpy(_header->command, command, sizeof(_header->command));

    return SUCCESS;
}

/**
 * Return weither the current message is the last one or not.
 *
 * \return mcsTRUE if the message is the last one, mcsFALSE othewise
 */
mcsLOGICAL msgMESSAGE::IsLastReply(void)
{
    logExtDbg("msgMESSAGE::GetLastReplyFlag()");

    // Return weither the current message is the last one or not
    return _header->lastReply;
}

/**
 * Set weither the current message is the last one or not.
 *
 * \param flag mcsTRUE if the message is the last one, mcsFALSE othewise
 * 
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetLastReplyFlag(mcsLOGICAL flag)
{
    logExtDbg("msgMESSAGE::SetLastReplyFlag()");

    // Copy the given value in the message header associated field
    _header->lastReply = flag;

    return SUCCESS;
}

/**
 * Return weither the current message is an internal one or not.
 *
 * \return mcsTRUE if the message is internal, mcsFALSE otherwise
 */
mcsLOGICAL msgMESSAGE::isInternal(void)
{
    logExtDbg("msgMESSAGE::GetInternalFlag()");

    // Return weither the current message is an internal one or not
    return _isInternal;
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
    return _header;
}

/**
 * Overwrite the current message header by the given one.
 *
 * \param header the address of an msgHEADER to replace the internal one
 *
 * \sa msgHEADER, the message header structure
 *
 * \return FAILURE if the given header pointer is NULL, SUCCESS otherwise
 */
mcsCOMPL_STAT msgMESSAGE::SetHeader(const msgHEADER* header)
{
    logExtDbg("msgMESSAGE::SetHeader()");

    // Test the header parameter vailidty
    if (header == NULL)
    {
        return FAILURE;
    }

    // Copy the given value in the message header
    memcpy(_header, header, msgHEADERLEN);

    return SUCCESS;
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
    return _body;
}

/**
 * Return the message body size.
 *
 * \return the message body size, or -1 if an error occured
 */
mcsINT32 msgMESSAGE::GetBodySize(void)
{
    logExtDbg("msgMESSAGE::GetBodySize()");

    // Get the message body size in the localhost byte order
    mcsINT32 msgBodySize = 0;
    mcsINT32 readFieldNumber = sscanf(_header->msgBodySize, "%d", &msgBodySize);

    // Verify the body size was well read
    if (readFieldNumber != 1)
    {
        return -1;
    }

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
mcsCOMPL_STAT msgMESSAGE::SetBody(const char  *buffer,
                                  mcsINT32     bufLen)
{
    logExtDbg("msgMESSAGE::SetBody()");
    
    // If there is nothing to copy in...
    if (buffer == NULL)
    {
        // Force buffer length to 0
        bufLen = 0;
    }
    // Else
    else
    {
        // If to-be-copied-in byte number is not given...
        if (bufLen == 0)
        {
            // Get the given buffer total length
            bufLen = strlen(buffer);
        }
    }
    // End if

    // If the to-be-copied byte number is greater than the message body
    // maximum size...
    if (bufLen > (mcsINT32)msgBODYMAXLEN)
    {
        // Raise an error
        errAdd(msgERR_BUFFER_TOO_BIG, bufLen, msgBODYMAXLEN);

        // Return an error code
        return FAILURE;
    }
    
    // Store the new message body size, in network byte order
    sprintf(_header->msgBodySize, "%d", bufLen);
    
    // Fill the message body with the given length and buffer content
    if (buffer != NULL)
    {
        memcpy(_body, buffer, bufLen);
    }

    return SUCCESS;
}

/**
 * Return a pointer to the message internal msgMESSAGE_RAW structure.
 *
 * \return the address of the internal msgMESSAGE_RAW structure
 */
msgMESSAGE_RAW* msgMESSAGE::GetMessagePtr(void)
{
    logExtDbg("msgMESSAGE::GetMessagePtr()");

    // Return a pointer to the message internal structure
    return &_message;
}

/**
 * Copy the given content in the message.
 *
 * \param message the content be copied in
 *
 * \return FAILURE if the given message pointer is NULL, SUCCESS otherwise
 */
mcsCOMPL_STAT msgMESSAGE::SetMessage(const msgMESSAGE_RAW* message)
{
    logExtDbg("msgMESSAGE::SetMessage()");
    
    // If there is nothing to copy in...
    if (message == NULL)
    {
        return FAILURE;
    }

    // Overwrite the message with the given content
    memcpy(&_message, message, msgMAXLEN);
    
    return SUCCESS;
}

/**
 * Write the msgMESSAGE content on output stream.
 *
 * \return the address of the internal msgMESSAGE_RAW structure
 */
//ostream& operator<<(ostream &o, const msgMESSAGE &msg)
void  msgMESSAGE::Display(void)
{
    cout << "msgMESSAGE ="                                     << endl
         << "{"                                                << endl
         << "\tmsgHEADER ="                                    << endl
         << "\t{"                                              << endl
         << "\t\tsender       = '" << GetSender()       << "'" << endl
         << "\t\tsenderEnv    = '" << GetSenderEnv()    << "'" << endl
         << "\t\trecipient    = '" << GetRecipient()    << "'" << endl
         << "\t\trecipientEnv = '" << GetRecipientEnv() << "'" << endl
         << "\t\ttype         = '" << GetType()         << "'" << endl
         << "\t\tidentifier   = '" << GetIdentifier()   << "'" << endl
         << "\t\tcommand      = '" << GetCommand()      << "'" << endl
         << "\t\tlastReply    = '" << IsLastReply()     << "'" << endl
         << "\t}"                                              << endl
         << "\t" << "body = '"     << GetBodyPtr()      << "'" << endl
         << "}";
}

/*___oOo___*/

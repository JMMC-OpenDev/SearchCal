/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE.cpp,v 1.20 2005-01-29 19:54:46 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2005/01/29 10:05:06  gzins
 * Changed msgMESSAGE.lastReply type from mcsLOGICAL to mcsSTRING8
 *
 * Revision 1.18  2005/01/29 07:17:59  gzins
 * Fixed wrong message body initialization in constructor
 *
 * Revision 1.17  2005/01/28 23:49:44  gzins
 * Defined GetBody and GetBodySize as constant method
 *
 * Revision 1.16  2005/01/24 15:46:10  gzins
 * Fixed wrong method name in logExtDbg() for ClearBody()
 *
 * Revision 1.15  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
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
 * gzins     08-Dec-2004  Implemented methods for SendId and MessageId 
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer, and removed unused API
 * gzins     20-Dec-2004  Fixed bug in GetBody which returned a wrong pointer
 *                        when body was empty
 * gzins     22-Dec-2004  Renamed GetBodyPtr to GetBody
 *                        Removed GetHeaderPtr
 *                        Declared AllocateBody as private
 *                        Renamed isInternal to IsInternal
 *                        Added ClearBody and AppendToBody
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *                        Implemented copy constructor and assignment operator
 *                        Changed messageId to commandId
 *
 ******************************************************************************/

/**
 * \file
 * msgMESSAGE class definition.
 */

static char *rcsId="@(#) $Id: msgMESSAGE.cpp,v 1.20 2005-01-29 19:54:46 gzins Exp $"; 
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
    // Reset all the header structure to 0
    memset(&_header, 0, sizeof(_header));

    // Initializing the body
    ClearBody(); 

    // Message is considered extern by default
    _isInternal = isInternalMsg;

    // Reset Ids
    SetSenderId(-1);
    SetCommandId(-1);
}

/**
 * Copy constructor
 */
msgMESSAGE::msgMESSAGE(const msgMESSAGE& msg)
{
    *this = msg;
}

/**
 * Assignment operator
 */
msgMESSAGE &msgMESSAGE::operator=(const msgMESSAGE& msg)
{
    memcpy (&_header, &msg._header, sizeof(msgHEADER));
    if (msg.GetBodySize() != 0)
    {
        SetBody(msg.GetBody(), msg.GetBodySize());
    }
    else
    {
        ClearBody();
    }
    _isInternal = msg._isInternal;
    return *this;
}

/*
 * Class destructor
 */
msgMESSAGE::~msgMESSAGE()
{
    // Destroy the body Dynamic Buffer
    miscDynBufDestroy(&_body);
}

/*
 * Public methods
 */

/**
 * Return the sender process name.
 *
 * \return the address of the message sender process name
 */
char* msgMESSAGE::GetSender(void)
{
    logExtDbg("msgMESSAGE::GetSender()");

    // Return the sender process name
    return _header.sender;
}

/**
 * Copy the given sender name into the message.
 *
 * \param sender the sender name to be copied in
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSender(const char *sender)
{
    logExtDbg("msgMESSAGE::SetSender()");

    // Copy the given value in the message header associated field
    strncpy(_header.sender, sender, sizeof(_header.sender));

    return mcsSUCCESS;
}

/**
 * Return the message sender environnement name.
 *
 * \return the address of the message sender process environnement name
 */
char* msgMESSAGE::GetSenderEnv(void)
{
    logExtDbg("msgMESSAGE::GetSenderEnv()");

    // Return the message sender environnement name
    return _header.senderEnv;
}

/**
 * Copy the given sender environnement name into the message.
 *
 * \param senderEnv the sender environnement name to be copied in
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSenderEnv(const char *senderEnv)
{
    logExtDbg("msgMESSAGE::SetSenderEnv()");

    // Copy the given value in the message header associated field
    strncpy(_header.senderEnv, senderEnv, sizeof(_header.senderEnv));

    return mcsSUCCESS;
}

/**
 * Return the sender process id.
 *
 * \return the id of the message sender process name
 */
mcsINT32 msgMESSAGE::GetSenderId(void)
{
    logExtDbg("msgMESSAGE::GetSenderId()");

    // Get the sender id 
    mcsINT32 id = -1;
    sscanf(_header.senderId, "%d", &id);
    
    // Return sender id 
    return id;
}

/**
 * Set the id of the sender process.
 *
 * \param id the sender id 
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSenderId(mcsINT32 id)
{
    logExtDbg("msgMESSAGE::SetSenderId()");

    // Set the sender id
    sprintf(_header.senderId, "%d", id);

    return mcsSUCCESS;
}

/**
 * Return the message receiver process name.
 *
 * \return the address of the message receiver process name
 */
char* msgMESSAGE::GetRecipient(void)
{
    logExtDbg("msgMESSAGE::GetRecipient()");

    // Return the message receiver process name
    return _header.recipient;
}

/**
 * Copy the given recipient name into the message.
 *
 * \param recipient the recipient name to be copied in
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipient(const char *recipient)
{
    logExtDbg("msgMESSAGE::SetRecipient()");

    // Copy the given value in the message header associated field
    strncpy(_header.recipient, recipient, sizeof(_header.recipient));

    return mcsSUCCESS;
}

/**
 * Return the recipient environnement name.
 *
 * \return the address of the message recipient process environnement
 */
char* msgMESSAGE::GetRecipientEnv(void)
{
    logExtDbg("msgMESSAGE::GetRecipientEnv()");

    // Return the recipient environnement name
    return _header.recipientEnv;
}

/**
 * Copy given the recipient environnement name into the message.
 *
 * \param recipientEnv the recipient environnement name to be copied in
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetRecipientEnv(const char *recipientEnv)
{
    logExtDbg("msgMESSAGE::SetRecipientEnv()");

    // Copy the given value in the message header associated field
    strncpy(_header.recipientEnv, recipientEnv, sizeof(_header.recipientEnv));

    return mcsSUCCESS;
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
    return _header.type;
}

/**
 * Set the type of the message.
 *
 * \param type the type of the message
 * 
 * \sa msgTYPE, the enumeration that list all the possible message types
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetType(const msgTYPE type)
{
    logExtDbg("msgMESSAGE::SetType()");

    // Copy the given value in the message header associated field
    _header.type = type;

    return mcsSUCCESS;
}

/**
 * Return the message identifier value.
 *
 * \return the identifier value of the message
 */
mcsINT32 msgMESSAGE::GetCommandId(void)
{
    logExtDbg("msgMESSAGE::GetCommandId()");

    // Get the sender id 
    mcsINT32 id = -1;
    sscanf(_header.commandId, "%d", &id);
    
    // Return sender id 
    return id;
    // Return the message identifier value
}

/**
 * Set the message Id.
 *
 * \param id message Id 
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetCommandId(const mcsINT32 id)
{
    logExtDbg("msgMESSAGE::SetIdentifier()");

    // Set the message id
    sprintf(_header.commandId, "%d", id);

    return mcsSUCCESS;
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
    return _header.command;
}

/**
 * Copy the given command name into the message.
 *
 * \param command the command name to be copied in
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetCommand(const char *command)
{
    logExtDbg("msgMESSAGE::SetCommand()");

    // Copy the given value in the message header associated field
    strncpy(_header.command, command, sizeof(_header.command));

    return mcsSUCCESS;
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
    return ((_header.lastReply == 'T') ? mcsTRUE : mcsFALSE);
}

/**
 * Set weither the current message is the last one or not.
 *
 * \param flag mcsTRUE if the message is the last one, mcsFALSE othewise
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetLastReplyFlag(mcsLOGICAL flag)
{
    logExtDbg("msgMESSAGE::SetLastReplyFlag()");

    // Copy the given value in the message header associated field
    _header.lastReply = (flag == mcsTRUE) ? 'T' : 'F';

    return mcsSUCCESS;
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
    return _isInternal;
}

/**
 * Return a pointer to the message body.
 *
 * \return the address of the message body, or NULL if an error occured
 */
char* msgMESSAGE::GetBody(void) const
{
    logExtDbg("msgMESSAGE::GetBody()");

    // If message body is empty
    if (GetBodySize() == 0)
    {
        // Return an empty string
        return ("");
    }
    // Else
    else
    {
        // Return a pointer to the message body
        return miscDynBufGetBuffer(&_body);
    }
    // End if
}

/**
 * Return the message body size.
 *
 * \return the message body size, or -1 if an error occured
 */
mcsINT32 msgMESSAGE::GetBodySize(void) const
{
    logExtDbg("msgMESSAGE::GetBodySize()");

    // Get the message body size in the localhost byte order
    mcsINT32 msgBodySize = 0;
    mcsINT32 ndReadFields = sscanf(_header.msgBodySize, "%d", &msgBodySize);

    // Verify the body size was well read
    if (ndReadFields != 1)
    {
        return -1;
    }

    return msgBodySize;

}

/**
 * Clear the message body.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT msgMESSAGE::ClearBody(void)
{
    logExtDbg("msgMESSAGE::ClearBody()");


    // Empty the body buffer
    if (miscDynBufReset(&_body) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Reset the body size
    sprintf(_header.msgBodySize, "%d", 0);

    return mcsSUCCESS;
}

/**
 * Copy \<bufLen\> bytes of \<buffer\> in the message body.
 *
 * If \<bufLen\> equal 0, strlen() is used to get \<buffer\> length to be
 * copied in.
 *
 * \param buffer buffer to be copied in
 * \param bufLen buffer size
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li msgERR_NULL_PARAM
 */
mcsCOMPL_STAT msgMESSAGE::SetBody(const char *buffer,
                                  mcsUINT32  bufLen)
{
    logExtDbg("msgMESSAGE::SetBody()");
    
    // If received a NULL pointer
    if (buffer == NULL)
    {
        errAdd(msgERR_NULL_PARAM, "buffer");
        return mcsFAILURE;
    }
    
    // If to-be-copied-in byte number is not given...
    if (bufLen <= 0)
    {
        // Get the given buffer total length
        bufLen = strlen(buffer) + 1;
    }

    // Fill the body buffer with the given length and content
    if (bufLen > 0)
    {  
        // Reset the body buffer and allocate sufficient memory
        if (AllocateBody(bufLen) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        if (miscDynBufAppendBytes(&_body, (char*)buffer, bufLen) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        // Empty the body buffer
        if (miscDynBufReset(&_body) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Store the new body size in the header
    sprintf(_header.msgBodySize, "%d", bufLen);
    
    return mcsSUCCESS;
}

/**
 * Append \<bufLen\> bytes of \<buffer\> to the message body.
 *
 * If \<bufLen\> equal 0, strlen() is used to get \<buffer\> length to be
 * appended.
 *
 * \param buffer buffer to be appended to
 * \param bufLen buffer size 
 *
 * \return mcsFAILURE if the to-be-copied-in byte number is greater than the
 * message body maximum size, mcsSUCCESS otherwise
 */
mcsCOMPL_STAT msgMESSAGE::AppendToBody(const char *buffer,
                                       mcsUINT32  bufLen)
{
    logExtDbg("msgMESSAGE::AppendToBody()");
    
    // If received a NULL pointer
    if (buffer == NULL)
    {
        errAdd(msgERR_NULL_PARAM, "buffer");
        return mcsFAILURE;
    }
    
    // If to-be-appended byte number is not given...
    if (bufLen <= 0)
    {
        // Get the given buffer total length
        bufLen = strlen(buffer) + 1;
    }

    // Fill the body buffer with the given length and content
    if (bufLen > 0)
    {  
        if (miscDynBufAppendBytes(&_body, (char*)buffer, bufLen) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Store the new body size in the header
    mcsUINT32 bodySize;
    miscDynBufGetNbStoredBytes(&_body, &bodySize);
    sprintf(_header.msgBodySize, "%d", bodySize);
    
    return mcsSUCCESS;
}

/**
 * Append \<str\> to the message body.
 *
 * \param str string to be appended to
 *
 * \return mcsSUCCESS on successful completion, and mcsFAILURE otherwise.
 */
mcsCOMPL_STAT msgMESSAGE::AppendStringToBody(const char *str)
{
    logExtDbg("msgMESSAGE::AppendStringToBody()");
    
    // If received a NULL pointer
    if (str == NULL)
    {
        errAdd(msgERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }
    
    // Append string to the dynamic buffer 
    if (miscDynBufAppendString(&_body, (char*)str) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Store the new body size in the header
    mcsUINT32 bodySize;
    miscDynBufGetNbStoredBytes(&_body, &bodySize);
    sprintf(_header.msgBodySize, "%d", bodySize);
    
    return mcsSUCCESS;
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
         << "\t\tsenderId     = '" << GetSenderId()     << "'" << endl
         << "\t\trecipient    = '" << GetRecipient()    << "'" << endl
         << "\t\trecipientEnv = '" << GetRecipientEnv() << "'" << endl
         << "\t\ttype         = '" << GetType()         << "'" << endl
         << "\t\tcommand      = '" << GetCommand()      << "'" << endl
         << "\t\tcommandId    = '" << GetCommandId()    << "'" << endl
         << "\t\tlastReply    = '" << IsLastReply()     << "'" << endl
         << "\t\tmsgBodySize  = '" << GetBodySize()     << "'" << endl
         << "\t}"                                              << endl
         << "\t" << "body = '";

    // If the body exists
    if (GetBodySize() != 0)
    {
        // Show it
        cout << GetBody(); 
    }
    else
    {
        cout << "(null)";
    }

    cout << "'" << endl
         << "}";
}

/*
 * Public methods
 */
/**
 * Allocate some memory for the message body.
 *
 * \warning This method is not re-entrant, as the message body will be resetted
 * on each call.
 *
 * \param bufLen the total needed buffer size
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT  msgMESSAGE::AllocateBody(const mcsUINT32 bufLen)
{
    logExtDbg("msgMESSAGE::AllocateBody()");

    // Empty the body buffer
    if (miscDynBufReset(&_body) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set the message body size
    if (miscDynBufAlloc(&_body, bufLen) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/

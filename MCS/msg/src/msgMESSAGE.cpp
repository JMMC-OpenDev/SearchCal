/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE.cpp,v 1.24 2005-02-09 16:37:49 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.23  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.22  2005/02/03 06:51:42  gzins
 * Defined IsInternal method as constant
 *
 * Revision 1.21  2005/02/02 16:42:44  scetre
 * Added mcsDynBufInit on the _body
 *
 * Revision 1.20  2005/01/29 19:54:46  gzins
 * Added AppendStringToBody method
 *
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
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *                        Implemented copy constructor and assignment operator
 *                        Changed messageId to commandId
 * gzins     22-Dec-2004  Renamed GetBodyPtr to GetBody
 *                        Removed GetHeaderPtr
 *                        Declared AllocateBody as private
 *                        Renamed isInternal to IsInternal
 *                        Added ClearBody and AppendToBody
 * gzins     20-Dec-2004  Fixed bug in GetBody which returned a wrong pointer
 *                        when body was empty
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer, and removed unused API
 * gzins     08-Dec-2004  Implemented methods for SendId and MessageId 
 * gzins     07-Dec-2004  Removed invalid parameters from Display documentation 
 * gzins     03-Dec-2004  Improved parameter check in SetBody method
 * lafrasse  01-Dec-2004  Added error management code, comment refinments, and
 *                        includes cleaning
 * lafrasse  23-Nov-2004  Moved isInternal from msgMESSAGE_RAW to _isInternal in
 *                        msgMESSAGE, added SetLastReplyFlag method
 * lafrasse  22-Nov-2004  Added void type for functions without parameters
 * lafrasse  19-Nov-2004  Re-commented, and replaced all the srtcpy by some
 *                        strNcpy in order to avoid segmentation faults as far
 *                        as possible
 * scetre    17-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Storage class used to hold all the data transferred between remote processes.
 *
 * \sa msgMESSAGE
 */

static char *rcsId="@(#) $Id: msgMESSAGE.cpp,v 1.24 2005-02-09 16:37:49 lafrasse Exp $"; 
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
    miscDynBufInit(&_body);
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
 * Return the message sender process name.
 *
 * \return a character pointer on the message sender process name
 */
const char* msgMESSAGE::GetSender(void) const
{
    logExtDbg("msgMESSAGE::GetSender()");

    // Return the sender process name
    return _header.sender;
}

/**
 * Set the message sender name.
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
 * Return the message sender MCS environment name.
 *
 * \return a character pointer on the message sender process MCS environment
 * name
 */
const char *msgMESSAGE::GetSenderEnv(void) const
{
    logExtDbg("msgMESSAGE::GetSenderEnv()");

    // Return the message sender environment name
    return _header.senderEnv;
}

/**
 * Set the message sender MCS environment name.
 *
 * \param senderEnv the message sender MCS environment name to be copied in
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
 * Return the message sender process identifier.
 *
 * \return the identifier of the message sender
 */
mcsINT32 msgMESSAGE::GetSenderId(void) const
{
    logExtDbg("msgMESSAGE::GetSenderId()");

    // Get the sender identifier 
    mcsINT32 identifier = -1;
    sscanf(_header.senderId, "%d", &identifier);
    
    // Return sender identifier 
    return identifier;
}

/**
 * Set the identifier of the message sender process.
 *
 * \param identifier the message sender identifier
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetSenderId(mcsINT32 identifier)
{
    logExtDbg("msgMESSAGE::SetSenderId()");

    // Set the sender identifier
    sprintf(_header.senderId, "%d", identifier);

    return mcsSUCCESS;
}

/**
 * Return the message receiver process name.
 *
 * \return a character pointer on the message receiver process name
 */
const char *msgMESSAGE::GetRecipient(void) const
{
    logExtDbg("msgMESSAGE::GetRecipient()");

    // Return the message receiver process name
    return _header.recipient;
}

/**
 * Set the message recipient name.
 *
 * \param recipient the message recipient name to be copied in
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
 * Return the recipient MCS environment name.
 *
 * \return a character pointer on the message recipient process MCS environment
 * name
 */
const char *msgMESSAGE::GetRecipientEnv(void) const
{
    logExtDbg("msgMESSAGE::GetRecipientEnv()");

    // Return the recipient environment name
    return _header.recipientEnv;
}

/**
 * Set the message recipient MCS environment name.
 *
 * \param recipientEnv the message recipient MCS environment name to be copied
 * in
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
msgTYPE msgMESSAGE::GetType(void) const
{
    logExtDbg("msgMESSAGE::GetType()");

    // Return the message type
    return _header.type;
}

/**
 * Set the message type.
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
 * Return the message identifier.
 *
 * \return the message identifier
 */
mcsINT32 msgMESSAGE::GetCommandId(void) const
{
    logExtDbg("msgMESSAGE::GetCommandId()");

    // Get the sender identifier 
    mcsINT32 identifier = -1;
    sscanf(_header.commandId, "%d", &identifier);
    
    // Return the message identifier value
    return identifier;
}

/**
 * Set the message identifier.
 *
 * \param identifier the message identifier 
 * 
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgMESSAGE::SetCommandId(const mcsINT32 identifier)
{
    logExtDbg("msgMESSAGE::SetIdentifier()");

    // Set the message identifier
    sprintf(_header.commandId, "%d", identifier);

    return mcsSUCCESS;
}

/**
 * Return the message command name.
 *
 * \return a character pointer on the message command name
 */
const char *msgMESSAGE::GetCommand(void) const
{
    logExtDbg("msgMESSAGE::GetCommand()");

    // Return the message command name
    return _header.command;
}

/**
 * Set the message command name.
 *
 * \param command the message command name to be copied in
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
 * Return wether the message is the last reply of a dialog or not.
 *
 * \return mcsTRUE if the message is the last reply, mcsFALSE otherwise
 */
mcsLOGICAL msgMESSAGE::IsLastReply(void) const
{
    logExtDbg("msgMESSAGE::GetLastReplyFlag()");

    // Return wether the current message is the last reply or not
    return ((_header.lastReply == 'T') ? mcsTRUE : mcsFALSE);
}

/**
 * Set wether the current message is the last reply of a dialog or not.
 *
 * \param flag mcsTRUE if the message is the last reply, mcsFALSE otherwise
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
 * Return wether the message is an internal one or not.
 *
 * \return mcsTRUE if the message is internal, mcsFALSE otherwise
 */
mcsLOGICAL msgMESSAGE::IsInternal(void) const
{
    logExtDbg("msgMESSAGE::IsInternal()");

    // Return wether the current message is an internal one or not
    return _isInternal;
}

/**
 * Return the message body.
 *
 * \return a character pointer on the message body, or NULL if an error occured
 */
const char *msgMESSAGE::GetBody(void) const
{
    logExtDbg("msgMESSAGE::GetBody()");

    // If the message body is empty
    if (GetBodySize() == 0)
    {
        // Return an empty string
        return ("");
    }
    else
    {
        // Return the pointer on the message body
        return miscDynBufGetBuffer(&_body);
    }
}

/**
 * Return the message body size.
 *
 * \return the message body size, or -1 if an error occured
 */
mcsINT32 msgMESSAGE::GetBodySize(void) const
{
    logExtDbg("msgMESSAGE::GetBodySize()");

    // Get the message body size in the localhost endianess
    mcsINT32 msgBodySize = 0;
    mcsINT32 ndReadFields = sscanf(_header.msgBodySize, "%d", &msgBodySize);

    // Verify that the body size was well read
    if (ndReadFields != 1)
    {
        return -1;
    }

    return msgBodySize;
}

/**
 * Clear the message body.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
 * Copy \em bufLen bytes of \em buffer in the message body.
 *
 * If \em bufLen equal 0, strlen() is used to retrieve the \em buffer length to
 * be copied in.
 *
 * \param buffer pointer on the buffer to be copied in
 * \param bufLen buffer quantity to be copied in
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
 * Append \em bufLen bytes of the \em buffer to the message body.
 *
 * If \em bufLen equal 0, strlen() is used to get the \em buffer length to be
 * appended.
 *
 * \param buffer a pointer on the buffer to be appended to the message body
 * \param bufLen buffer quantity to be append 
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
 * Append a null-terminated charater array to the message body.
 *
 * \param str the null-terminated charater array to be appended to the message
 * body
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
 * Show the msgMESSAGE content on the standard output.
 */
std::ostream& operator<< (      std::ostream&  stream,
                          const msgMESSAGE&    message)
{
    stream << "msgMESSAGE ="                                      << endl
           << "{"                                                 << endl
           << "\tmsgHEADER ="                                     << endl
           << "\t{"                                               << endl
           << "\t\tsender       = '" << message.GetSender()       << "'" << endl
           << "\t\tsenderEnv    = '" << message.GetSenderEnv()    << "'" << endl
           << "\t\tsenderId     = '" << message.GetSenderId()     << "'" << endl
           << "\t\trecipient    = '" << message.GetRecipient()    << "'" << endl
           << "\t\trecipientEnv = '" << message.GetRecipientEnv() << "'" << endl
           << "\t\ttype         = '" << message.GetType()         << "'" << endl
           << "\t\tcommand      = '" << message.GetCommand()      << "'" << endl
           << "\t\tcommandId    = '" << message.GetCommandId()    << "'" << endl
           << "\t\tlastReply    = '" << message.IsLastReply()     << "'" << endl
           << "\t\tmsgBodySize  = '" << message.GetBodySize()     << "'" << endl
           << "\t}"                                               << endl
           << "\t" << "body = '";

    // If the body exists
    if (message.GetBodySize() != 0)
    {
        // Show it
        stream << message.GetBody(); 
    }
    else
    {
        stream << "(null)";
    }

    return stream << "'" << endl
                  << "}";
}


/*
 * Private methods
 */

/**
 * Allocate some memory for the message body.
 *
 * \warning This method is not re-entrant, as the message body will be resetted
 * on each call.
 *
 * \param bufLen the total needed buffer size
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT  msgMESSAGE::AllocateBody(const mcsUINT32 bufLen)
{
    logExtDbg("msgMESSAGE::AllocateBody()");

    // Empty the message body dynamic buffer
    if (miscDynBufReset(&_body) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Try to allocate the desired amount of memory
    if (miscDynBufAlloc(&_body, bufLen) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/

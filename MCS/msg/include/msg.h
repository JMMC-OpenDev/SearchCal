#ifndef msg_H
#define msg_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msg.h,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  10-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the 'msg' library functions declarations.
 *
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


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
#define msgMAXLEN                8192


/**
 * Standard command names
 */
#define msgDEBUG_CMD            "DEBUG"
#define msgPING_CMD             "PING"
#define msgEXIT_CMD             "EXIT"
#define msgVERSION_CMD          "VERSION"



/* 
 * Macro definition
 */

/**
 * Maximum message body size
 */
#define msgBODYMAXLEN            (msgMAXLEN - sizeof(msgHEADER) - 1)


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

    mcsPROCNAME recipient;       /**< Receiver processus name  */

    mcsUINT8    type;            /**< Message type */

    mcsCMD      command;         /**< Command name */

    mcsLOGICAL  lastReply;       /**< TRUE if it is the last answer */

    mcsBYTES32  timeStamp;       /**< Message date */

    mcsINT32    msgBodySize;     /**< Message body size */

} msgHEADER;


/**
 * Complete message structure
 */
typedef struct
{
    msgHEADER  header;

    char       body[msgMAXLEN - sizeof(msgHEADER)];

} msgMESSAGE;



/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT   msgSetBody        (msgMESSAGE         *msg,
                                   char               *buffer,
                                   mcsINT32           bufLen);

char *          msgGetBodyPtr     (msgMESSAGE         *msg);

mcsINT32        msgGetBodySize    (msgMESSAGE         *msg);

char *          msgGetCommand     (msgMESSAGE         *msg);

char *          msgGetSender      (msgMESSAGE         *msg);

char *          msgGetRecipient   (msgMESSAGE         *msg);

mcsLOGICAL      msgIsLastReply    (msgMESSAGE         *msg);

msgTYPE         msgGetType        (msgMESSAGE         *msg);



mcsCOMPL_STAT   msgConnect        (const mcsPROCNAME  procName,
                                   const char*        msgManagerHost);
                                  
mcsCOMPL_STAT   msgDisconnect     ();



mcsCOMPL_STAT   msgReceive        (msgMESSAGE         *msg,
                                   mcsINT32           timeoutInMs);
                                  
mcsCOMPL_STAT   msgReceiveFrom    (int                sd,
                                   msgMESSAGE         *msg,
                                   mcsINT32           timeoutInMs);
                                  


mcsCOMPL_STAT   msgSendCommand    (const char         *command,
                                   const mcsPROCNAME  destProc,
                                   const char         *buffer,  
                                   mcsINT32           bufLen);
                                  
mcsCOMPL_STAT   msgSendTo         (int                sd,
                                   msgMESSAGE         *msg);
                                  
mcsCOMPL_STAT   msgSendReply      (msgMESSAGE         *msg,
                                   mcsLOGICAL         lastReply);
                                  
mcsCOMPL_STAT   msgSendReplyTo    (int                sd,
                                   msgMESSAGE         *msg,
                                   mcsLOGICAL         lastReply);
                                  
int             msgSocketCreate   (unsigned short     *portNumberPt,
                                   int                socketType);
                                  
mcsCOMPL_STAT   msgSocketClose    (int                sd);
                                  

#ifdef __cplusplus
}
#endif


#endif /*!msg_H*/


/*___oOo___*/

#ifndef msg_H
#define msg_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msg.h,v 1.8 2004-11-19 23:55:17 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  10-Aug-2004  Ported from CILAS software
* lafrasse  07-Oct-2004  Added msgIsConnected
* lafrasse  19-Nov-2004  Moved all the C functions declaration to msgPrivate.h
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the 'msg' library structures declarations.
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
    mcsENVNAME  senderEnv;       /**< Sender environnement */
    mcsPROCNAME recipient;       /**< Receiver processus name  */
    mcsENVNAME  recipientEnv;    /**< Receiver environnement */
    mcsSTRING8  identifier;      /**< Identificator */
    mcsLOGICAL  isInternal;      /**< FALSE if it is external >*/
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
 * Pubic functions declaration
 */


#ifdef __cplusplus
}
#endif


#endif /*!msg_H*/


/*___oOo___*/

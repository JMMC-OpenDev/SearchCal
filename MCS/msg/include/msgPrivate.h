#ifndef msgPrivate_H
#define msgPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgPrivate.h,v 1.2 2004-11-19 17:15:42 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  10-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Private 'msg' module header file, holding the MODULE_ID definition and
 * other constants.
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
 * Constants definition
 */
#define MODULE_ID                       "msg"

/**
 * msgManger port number for connection
 */
#define msgMANAGER_PORT_NUMBER           1991
#define msgMANAGER_SELECT_WIDTH          32
#define msgMANAGER_MAX_LENGTH_QUEUE      32

/**
 * Private command name
 */
#define msgREGISTER_CMD                 "REGISTER"
#define msgCLOSE_CMD                    "CLOSE"


/*
 * Private Globals
 */

/**
 * Connection with msgManager socket
 */
extern int msgManagerSd;    

mcsCOMPL_STAT   msgSetBody        (msgMESSAGE_RAW         *msg,
                                   char               *buffer,
                                   mcsINT32           bufLen);

char *          msgGetBodyPtr     (msgMESSAGE_RAW         *msg);

mcsINT32        msgGetBodySize    (msgMESSAGE_RAW         *msg);

char *          msgGetCommand     (msgMESSAGE_RAW         *msg);

char *          msgGetSender      (msgMESSAGE_RAW         *msg);

char *          msgGetRecipient   (msgMESSAGE_RAW         *msg);

mcsLOGICAL      msgIsLastReply    (msgMESSAGE_RAW         *msg);

msgTYPE         msgGetType        (msgMESSAGE_RAW         *msg);

mcsLOGICAL      msgIsConnected    (void);

mcsCOMPL_STAT   msgConnect        (const mcsPROCNAME  procName,
                                   const char*        msgManagerHost);

mcsCOMPL_STAT   msgDisconnect     (void);

mcsCOMPL_STAT   msgReceive        (msgMESSAGE_RAW     *msg,
                                   mcsINT32           timeoutInMs);
                                  
mcsCOMPL_STAT   msgReceiveFrom    (int                sd,
                                   msgMESSAGE_RAW     *msg,
                                   mcsINT32           timeoutInMs);
                                  


mcsCOMPL_STAT   msgSendCommand    (const char         *command,
                                   const mcsPROCNAME  destProc,
                                   const char         *buffer,  
                                   mcsINT32           bufLen);
                                  
mcsCOMPL_STAT   msgSendTo         (int                sd,
                                   msgMESSAGE_RAW     *msg);
                                  
mcsCOMPL_STAT   msgSendReply      (msgMESSAGE_RAW     *msg,
                                   mcsLOGICAL         lastReply);
                                  
mcsCOMPL_STAT   msgSendReplyTo    (int                sd,
                                   msgMESSAGE_RAW     *msg,
                                   mcsLOGICAL         lastReply);
                                  
int             msgSocketCreate   (unsigned short     *portNumberPt,
                                   int                socketType);
                                  
mcsCOMPL_STAT   msgSocketClose    (int                sd);
                                  


#ifdef __cplusplus
}
#endif

#endif /*!msgPrivate_H*/


/*___oOo___*/

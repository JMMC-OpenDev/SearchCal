#ifndef msgManager_H
#define msgManager_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgManager.h,v 1.3 2004-11-22 14:31:20 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
* lafrasse  19-Nov-2004  Changed msgMESSAGE structure name to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the 'msgManager' process declarations.
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
 * System Headers 
 */
#include <sys/select.h>


/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgMESSAGE.h"
#include "msgPrivate.h"


/* 
 * Structure type definition
 */

/**
 * Connected process linked list element structure
 */
typedef struct msgPROCESS
{
   mcsPROCNAME          name;       /**< Connected process name. */

   int                  sd;         /**< Connected process associated socket. */

   struct msgPROCESS   *next;       /**< Next linked list process pointer. */

} msgPROCESS;


/**
 * Connected process linked list structure
 */
typedef struct msgPROCESS_LIST
{
    mcsINT32            nbProcess;  /**< Connected processes number. */

    fd_set             *readMask;   /**< Connected processes read mask. */

    msgPROCESS         *header;     /**< First linked list process pointer. */

} msgPROCESS_LIST;



/*
 * Pubic functions declaration
 */
void            msgSignalHandler          (int                signalNumber);
               


mcsCOMPL_STAT   msgManagerProcessListInit (msgPROCESS_LIST    *list,
                                           fd_set             *readMask);

mcsCOMPL_STAT   msgManagerProcessListAdd  (msgPROCESS_LIST    *list, 
                                           mcsPROCNAME         procName,
                                           int                 sd);

msgPROCESS     *msgManagerProcessListFind (msgPROCESS_LIST    *list,
                                           int                 sd);

msgPROCESS     *msgManagerProcessListFindByName(
                                           msgPROCESS_LIST    *list, 
                                           mcsPROCNAME         procName);

mcsCOMPL_STAT   msgManagerProcessListRemove(
                                           msgPROCESS_LIST    *list, 
                                           msgPROCESS         *process);
               


mcsCOMPL_STAT   msgManagerProcessSetConnection(
                                           int                 connectionSocket,
                                           msgPROCESS_LIST    *procList);



mcsCOMPL_STAT   msgManagerHandleCmd       (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList);

mcsCOMPL_STAT   msgManagerForwardCmd      (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList);

mcsCOMPL_STAT   msgManagerForwardReply    (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList);


#ifdef __cplusplus
}
#endif


#endif /*!msgManager_H*/


/*___oOo___*/

/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgDisconnect.c,v 1.4 2004-11-19 17:15:47 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
* lafrasse  07-Oct-2004  Added msgIsConnected
* lafrasse  19-Nov-2004  Changed msgMESSAGE structure name to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgDisconnect() function definition, used to close the connection
 * whith the 'msgManager' communication server.
 * 
 */

static char *rcsId="@(#) $Id: msgDisconnect.c,v 1.4 2004-11-19 17:15:47 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


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
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to close the connection with the communication server.
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgDisconnect     (void)
{
    msgMESSAGE_RAW msg;

    logExtDbg("msgDisconnect()");

    /* If no connection is already open... */
    if (msgIsConnected() == mcsFALSE)
    {
        /* Raise an error */
        errAdd(msgERR_PROC_NOT_CONNECTED);

        /* Return an errror code */
        return FAILURE;
    }

    /* Try to send a 'close command' message to msgManager */
    if (msgSendCommand (msgCLOSE_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        close(msgManagerSd);
        msgManagerSd = -1;
        return FAILURE;
    }
    
    /* Wait for its answer */
    if (msgReceive(&msg, 1000) == FAILURE)
    {
        close(msgManagerSd);
        msgManagerSd = -1;
        return FAILURE;
    }

    /* If an error occured while deconnecting... */
    if (msgGetType(&msg) == msgTYPE_ERROR_REPLY)
    {
        /* Put the received error in the local MCS error stack */
        errUnpackStack(msgGetBodyPtr(&msg), msgGetBodySize(&msg));
        close(msgManagerSd);
        msgManagerSd = -1;       
        return (FAILURE);
    }
    
    /* Close the socket */ 
    close(msgManagerSd);
    msgManagerSd = -1;       

    logExtDbg("Connection to 'msgManager' closed");

    return (SUCCESS);
}


/*___oOo___*/

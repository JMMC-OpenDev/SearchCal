/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgGetMessageQueue.c,v 1.1 2004-10-01 13:05:41 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     01-Oct-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgGetMessageQueue function declaration.
 */

static char *rcsId="@(#) $Id: msgGetMessageQueue.c,v 1.1 2004-10-01 13:05:41 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <stdio.h>


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

/**
 * Get the socket descriptor for the message queue
 *
 * Returns the socket descriptor of the communication link with message
 * manager. This allows a process to monitor its message queue using the UNIX
 * function select().
 *
 * \warning
 * The file descriptor returned must NOT be read or manipulated in any way
 * (e.g. close()) by the process. Otherwise the monitoring system will lose
 * syncronization with the message manager.
 */
mcsINT32 msgGetMessageQueue()
{
    logExtDbg("msgGetMessageQueue()"); 
    return (msgManagerSd);
}




/*___oOo___*/

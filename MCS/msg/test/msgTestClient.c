/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgTestClient.c,v 1.2 2004-11-22 14:34:06 gzins Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * \e \<msgTestClient\> - 'msg' module client test program.
 *
 * \b Synopsis:\n
 * \e \<msgTestClient\>
 *
 * \b Details:\n
 * \e \<msgTestClient\> is a simple msgManager test program that connect to
 * msgTestServer through the communication server, pass it in DEBUG mode and
 * disconnect from it.
 * 
 */

static char *rcsId="@(#) $Id: msgTestClient.c,v 1.2 2004-11-22 14:34:06 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


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


/* 
 * Main
 */
int main (int argc, char *argv[])
{
    mcsInit("msgTestClient");
    errResetStack();

    /* Try to connect the msgManager communictaion server */
    if (msgConnect("msgTestClient", NULL) == FAILURE)
    {
        logWarning("msgConnect() failed");
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Pause for 1 second */
    sleep(1);

    /* Try to send a DEBUG command to msgManager */
    if (msgSendCommand("DEBUG", "msgTestServer", NULL, 0) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
    }

    /* Try to disconnect from msgManager */
    if (msgDisconnect() == FAILURE)
    {
        logWarning("msgDisconnect failed");
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


/*___oOo___*/

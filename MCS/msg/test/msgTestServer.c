/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgTestServer.c,v 1.4 2004-11-26 13:11:28 lafrasse Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
* lafrasse  20-Nov-2004  Renamed all msgMESSAGE references to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * \e \<msgTestServer\> - 'msg' module server test program.
 *
 * \b Synopsis:\n
 * \e \<msgTestServer\>
 *
 * \b Details:\n
 * \e \<msgTestServer\> is a simple msgManager test program that should play the
 * game of msgManager for to the DEBUG, VERSION and EXIT command only.
 * 
 */

static char *rcsId="@(#) $Id: msgTestServer.c,v 1.4 2004-11-26 13:11:28 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "cmd.h"


/*
 * Local Headers 
 */
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/* 
 * Local functions  
 */
mcsCOMPL_STAT msgDebugCB(msgMESSAGE_RAW *msg)
{
#if 0
    mcsINT32        index;
    mcsLOGICAL      log, verbose, printDate, printFileLine;
    mcsINT32        logLevel, verboseLevel;
    cmdPARAM_LIST  *paramList;

    logExtDbg("msgDebugCB()");

    /* Try to parse the received parameter list */
    paramList=cmdParseParam(msgGetCommand(msg), msgGetBodyPtr(msg));
    if (paramList == NULL)
    {
        msgSendReply(msg, mcsTRUE);
        return FAILURE;
    } 

    /* If the 'log' parameter is received... */
    index = cmdGetParamIndex(paramList, "log");
    if (index != -1)
    {
        /* Try to toggle file logging */
        if (cmdGetLogicalParamByIndex(paramList, index, &log) == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            if (log == mcsTRUE)
            {
                logEnableFileLog();
            }
            else
            {
                logDisableFileLog();
            }
        }
    }


    /* If the 'logLevel' parameter is received... */
    index = cmdGetParamIndex(paramList, "logLevel");
    if (index != -1)
    {
        /* Try to change file logging level */
        if (cmdGetIntegerParamByIndex(paramList, index, &logLevel) == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            logSetFileLogLevel(logLevel);
        }
    }

    /* If the 'verbose' parameter is received... */
    index = cmdGetParamIndex(paramList, "verbose");
    if (index != -1)
    {  
        /* Try to toggle stdout logging */
        if (cmdGetLogicalParamByIndex(paramList, index, &verbose) == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            if (verbose == mcsTRUE)
            {
                logEnableStdoutLog();
            }
            else
            {
                logDisableStdoutLog();
            }
        }
    }

    /* If the 'verboseLevel' parameter is received... */
    index = cmdGetParamIndex(paramList, "verboseLevel");
    if (index != -1)
    {
        /* Try to change stdout logging level */
        if (cmdGetIntegerParamByName(paramList, "verboseLevel", &verboseLevel)
            == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            logSetStdoutLogLevel(verboseLevel);
        }
    }

    /* If the 'printDate' parameter is received... */
    index = cmdGetParamIndex(paramList, "printDate");
    if (index != -1)
    {  
        /* Try to toggle log date printing */
        if (cmdGetLogicalParamByIndex(paramList, index, &printDate) == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            logSetPrintDate(printDate);
        }
    }

    /* If the 'printFileLine' parameter is received... */
    index = cmdGetParamIndex(paramList, "printFileLine");
    if (index != -1)
    {
        /* Try to toggle log line printing */
        if (cmdGetLogicalParamByIndex(paramList, index, &printFileLine)
            == FAILURE)
        {
            msgSendReply(msg, mcsTRUE);
            return FAILURE;
        }
        else
        {
            logSetPrintFileLine(printFileLine);
        }
    }

    /* Verify that there is no unrecognized parameter for the command */
    if (cmdCheckUnusedParams(paramList) == FAILURE)
    {
        msgSendReply(msg, mcsTRUE);
        return FAILURE;
    }

    /* Try to send an confirmation reply */
    msgSetBody(msg, "OK", 0);
    if (msgSendReply(msg, mcsTRUE) == FAILURE)
    {
        errCloseStack();
    }
#endif
    return SUCCESS;
}


mcsCOMPL_STAT msgVersionCB(msgMESSAGE_RAW *msg)
{
    logExtDbg("msgVersionCB()");

    /* Try to reply the msgManager version number */
    mcsBYTES128 buffer;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, rcsId);
    msgSetBody(msg, buffer, 0);
    if (msgSendReply(msg, mcsTRUE) == FAILURE)
    {
        errCloseStack();
    }

    return SUCCESS;
}


/* 
 * Main
 */
int main (int argc, char *argv[])
{
    errResetStack();
    logInfo("msgTestServer starting...");

    /* Try to connect to msgManager */
    if (msgConnect("msgTestServer", NULL) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        /* Try to receive a command */
        msgMESSAGE_RAW msg;
        if (msgReceive(&msg, msgWAIT_FOREVER) == FAILURE)
        {
            /* If the connection with msgManager has been lost... */
            if (errIsInStack("msg", msgERR_BROKEN_PIPE) == mcsTRUE)
            {
                msgDisconnect();
                logWarning("Connection with msgManager lost... ",
                           "Trying to reconnect.");
                for(;;)
                {
                    if (msgConnect("msgTestServer", NULL) == SUCCESS)
                    {
                        logInfo("Connected to msgManager.");
                        break;
                    }
                    errResetStack();
                    sleep(1);
                }
            }
            else
            {
                errCloseStack();
                sleep(1);
            }
        } /* If an error occured during command reception... */
        else
        {
            logInfo("Received %s command.", msgGetCommand(&msg));

            /* If the received command is DEBUG */
            if (strcmp (msgGetCommand(&msg), msgDEBUG_CMD) == 0)
            {
                if (msgDebugCB(&msg) == FAILURE)
                {
                    errCloseStack();
                }
            }
            /* If the received command is VERSION */
            else if (strcmp(msgGetCommand(&msg), msgVERSION_CMD) == 0)
            {
                if (msgVersionCB(&msg) == FAILURE)
                {
                    errCloseStack();
                }
            }            
            /* If the received command is EXIT */
            else if (strcmp(msgGetCommand(&msg), msgEXIT_CMD) == 0)
            {
                /* Try to send a confirmation reply */
                msgSetBody(&msg, "OK", 0);
                if (msgSendReply(&msg, mcsTRUE) == FAILURE)
                {
                    errCloseStack();
                }

                /* Exiting... */
                break;
            }
            else
            {
                /* Try to send a confirmation reply */
                msgSetBody(&msg, "OK", 0);
                if (msgSendReply(&msg, mcsTRUE) == FAILURE)
                {
                    errCloseStack();
                }
            }
        }
    }

    msgDisconnect();

    logInfo("msgTestServer exited.");

    exit(EXIT_SUCCESS);
}


/*___oOo___*/

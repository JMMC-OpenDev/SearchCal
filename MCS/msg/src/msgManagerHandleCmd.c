/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgManagerHandleCmd.c,v 1.3 2004-11-22 14:17:43 gzins Exp $"
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
 * Contain the msgManagerHandleCmd() function definition, used to handle msgManager own
 * command messages.
 * 
 */

static char *rcsId="@(#) $Id: msgManagerHandleCmd.c,v 1.3 2004-11-22 14:17:43 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
#include "msgManager.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to manage msgManager own commands.
 *
 * Those commands are :
 * \li CLOSE - close the connection with msgManager (internal use)
 * \li DEBUG - log messages management
 * \li EXIT - quit msgManager
 * \li PING - test if a process is connected to msgManager
 * \li VERSION - give back the current msgManager version number
 *
 * \param msg a received command
 * \param process the recipient process name
 * \param procList the process list in which looking at the recipient process
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerHandleCmd       (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList)
{
    logExtDbg("msgManagerHandleCmd()");
    logInfo("Received internal '%s' command from '%s'", msgGetCommand(msg),
            msgGetSender(msg));
    
    /* If the received command is a PING request... */ 
    if (strcmp(msgGetCommand(msg), msgPING_CMD) == 0)
    {
        /* If the command recipient is connected... */
        if ((strcmp (msgGetRecipient(msg), "msgManager") == 0)
            ||
            (msgManagerProcessListFindByName(procList, msgGetRecipient(msg)) != NULL))
        {
            /* Build an OK reply message */
            msgSetBody(msg, "OK", 0);
        }
        else
        {
            /* Raise the error */
            errAdd(msgERR_PING, msgGetRecipient(msg));
        }

        /* Try to send the built reply message */
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    } 
    /* If the received command is a VERSION request... */ 
    else if (strcmp(msgGetCommand(msg), msgVERSION_CMD) == 0)
    {
        /* Try to reply the msgManager CVS verson number */
        mcsBYTES128 buffer;
        memset(buffer, '\0', sizeof(buffer));
        sprintf(buffer, rcsId);
        msgSetBody(msg, buffer, 0);
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }  
    /* If the received command is a DEBUG request... */ 
    else if (strcmp(msgGetCommand(msg), msgDEBUG_CMD) == 0)
    {
        mcsINT32   index;
        mcsLOGICAL log, verbose, printDate, printFileLine;
        mcsINT32   logLevel, verboseLevel;
        
        /* Try to analyze the received command parameter */
        cmdPARAM_LIST *paramList;
        paramList = cmdParseParam(msgDEBUG_CMD, msgGetBodyPtr(msg));
        if (paramList == NULL)
        {
            msgSendReplyTo(process->sd, msg, mcsTRUE);
            return FAILURE;
        } 
        
        /* If the 'log' parameter is specified... */
        index = cmdGetParamIndex(paramList, "log");
        if (index != -1)
        {
            /* Try to toogle file log message recording */
            if (cmdGetLogicalParamByIndex(paramList, index, &log) == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
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

        /* If the 'logLevel' parameter is specified... */
        index = cmdGetParamIndex(paramList, "logLevel");
        if (index != -1)
        {
            /* Try to change the file log level accordinaly */
            if (cmdGetIntegerParamByIndex(paramList, index, &logLevel)
                == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetFileLogLevel(logLevel);
            }
        }

        /* If the 'verbose' parameter is specified... */
        index = cmdGetParamIndex(paramList, "verbose");
        if (index != -1)
        {  
            /* Try to toogle stdout log message printing */
            if (cmdGetLogicalParamByIndex(paramList, index, &verbose)
                != FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
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

        /* If the 'verboseLevel' parameter is specified... */
        index = cmdGetParamIndex(paramList, "verboseLevel");
        if (index != -1)
        {
            /* Try to change the stdout log level accordinaly */
            if (cmdGetIntegerParamByName(paramList, "verboseLevel", 
                                         &verboseLevel) == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetStdoutLogLevel(verboseLevel);
            }
        }

        /* If the 'printDate' parameter is specified... */
        index = cmdGetParamIndex(paramList, "printDate");
        if (index != -1)
        {  
            /* Try to toogle log date printing */
            if (cmdGetLogicalParamByIndex(paramList, index, &printDate)
                == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetPrintDate(printDate);
            }
        }
                
        /* If the 'printFileLine' parameter is specified... */
        index = cmdGetParamIndex(paramList, "printFileLine");
        if (index != -1)
        {  
            /* Try to toogle log line printing */
            if (cmdGetLogicalParamByIndex(paramList, index,  &printFileLine)
                != FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
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
            msgSendReplyTo(process->sd, msg, mcsTRUE);
            return FAILURE;
        }

        /* Try to send an hand-checking message */
        msgSetBody(msg, "OK", 0);
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }  
    /* If the received command is a CLOSE request... */
    else if (strcmp(msgGetCommand(msg), msgCLOSE_CMD) == 0)
    {
        /* Try to send an hand-checking message */
        msgSetBody(msg, "OK", 0);
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }

        /* Try to close the connection */
        logInfo("Connection with process '%s' closed", process->name);
        if (msgManagerProcessListRemove(procList, process) == FAILURE)
        {
            errCloseStack();
        }
    }
    /* If the received command is a EXIT request... */
    else if (strcmp(msgGetCommand(msg), msgEXIT_CMD) == 0)
    {
        /* Try to send an hand-checking message */
        msgSetBody(msg, "OK", 0);
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
        
        /* For each connected process... */
        msgPROCESS *currProcess = procList->header;
        while (currProcess != NULL)
        {
            /* Store the next process pointer to prevent errors in case of the
             * current process element destruction
             */
            msgPROCESS *next = currProcess->next;
            
            /* Try to close the connection */
            if (msgManagerProcessListRemove(procList, currProcess) == FAILURE)
            {
                errCloseStack();
            }

            currProcess = next;
        }

        /* Quit msgManager process */
        logInfo("msgManager exiting...");
        exit(EXIT_SUCCESS);
    }
    /* If the received command is an unknown request... */
    else
    {
        logWarning("'%s' received an unknown '%s' command", process->name,
                   msgGetCommand(msg));

        errAdd(msgERR_CMD_NOT_SUPPORTED,  msgGetCommand(msg));
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }

        return FAILURE;
    }

    return SUCCESS;
}


/*___oOo___*/

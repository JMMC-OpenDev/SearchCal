/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhDebugCB.cpp,v 1.1 2005-01-29 15:19:13 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of evhDebugCB class.
 */

static char *rcsId="@(#) $Id: evhDebugCB.cpp,v 1.1 2005-01-29 15:19:13 gzins Exp $"; 
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
#include "msg.h"

/*
 * Local Headers 
 */
#include "evhSERVER.h"
#include "evhDEBUG_CMD.h"
#include "evhPrivate.h"

/**
 * Callback method for DEBUG command.
 * 
 * It hanges logging levels on-line. Levels are defined from 1 to 5 whereby
 * level 1 produces only limited number of logs, and level 5 produces logs at a
 * very detailed level. It recognizes the
 *  following command parameter:
 *   \li stdoutLevel \em &lt;level&gt; specify the level for logs which are
 *   printed on stdout.
 *   \li logfileLevel \em &lt;level&gt; specify the level for logs which are
 *   stored into the log file.
 *   \li printDate \em &lt;T/F&gt; switch on/off printing of date.
 *   \li printFileLine \em &lt;T/F&gt; switch on/off printing of file name and
 *   line number.
 *
 * \return evhCB_COMPL_STAT.
 */
evhCB_COMPL_STAT evhSERVER::DebugCB(msgMESSAGE &msg, void*)
{
    evhDEBUG_CMD debugCmd(msg.GetCommand(), msg.GetBody());

    // Parses command
    if (debugCmd.Parse() == mcsFAILURE)
    {
        SendReply(msg);
        return (evhCB_FAILURE | evhCB_NO_DELETE);
    }

    // If 'stdoutLevel' parameter is specified...
    if (debugCmd.IsDefinedStdoutLevel() == mcsTRUE)
    {
        // Set new level
        mcsINT32 level;
        debugCmd.GetStdoutLevel(&level);
        logSetStdoutLogLevel((logLEVEL)level);
    }
    // End if

    // If 'logfileLevel' parameter is specified...
    if (debugCmd.IsDefinedLogfileLevel() == mcsTRUE)
    {
        // Set new level
        mcsINT32 level;
        debugCmd.GetLogfileLevel(&level);
        logSetFileLogLevel((logLEVEL)level);
    }
    // End if

    // If 'printDate' parameter is specified...
    if (debugCmd.IsDefinedPrintDate() == mcsTRUE)
    {
        // Set new level
        mcsLOGICAL flag;
        debugCmd.GetPrintDate(&flag);
        logSetPrintDate(flag);
    }
    // Endif

    // If 'printFileLine' parameter is specified...
    if (debugCmd.IsDefinedPrintFileLine() == mcsTRUE)
    {
        // Set new level
        mcsLOGICAL flag;
        debugCmd.GetPrintFileLine(&flag);
        logSetPrintFileLine(flag);
    }
    // Endif

    /* Send reply */
    msg.SetBody("OK");
    SendReply(msg);

    return (evhCB_NO_DELETE);
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: logTest.c,v 1.9 2004-08-04 09:46:11 lafrasse Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* mella     07-May-2004  Created
* lafrasse  30-Jun-2004  Changed some APIs :
*                        logSetLog -> logSetFileLogState
*                        logSetLogLevel -> logSetFileLogLevel
*                        logGetLogLevel -> logGetFileLogLevel
*                        logSetVerbose -> logSetStdoutLogState
*                        logSetVerboseLevel -> logSetStdoutLogLevel
*                        logGetVerboseLevel -> logGetStdoutLogLevel
*                        logSetActionLevel -> logSetActionLogLevel
*                        logGetActionLevel -> logGetActionLogLevel
* lafrasse  03-Aug-2004  Changed some APIs :
*                        logSetFileLogVerbosity -> logSetFileLogLevel
*                        logGetFileLogVerbosity -> logGetFileLogLevel
*                        logSetStdoutLogVerbosity -> logSetStdoutLogLevel
*                        logGetStdoutLogVerbosity -> logGetStdoutLogLevel
*                        logSetActionLogVerbosity -> logSetActionLogLevel
*                        logGetActionLogVerbosity -> logGetActionLogLevel
*                        Replaced logSetFileLogState by logEnableFileLog and
*                        logDisableFileLog
*                        Replaced logSetStdoutLogState by logEnableStdoutLog and
*                        logDisableStdoutLog
*
*
*******************************************************************************/

/* 
 * System Headers 
 */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/*
 * Local Headers 
 */
#include "log.h"
#include "logPrivate.h"

/*
 *Local functions
 */

mcsCOMPL_STAT doLogs(void)
{
    /* Perform each log macro */
    logWarning("logging warning");
    logInfo("logging info");
    logTest("logging test");
    logDebug("logging debug");
    logExtDbg("logging extended debug");

    return SUCCESS;
}

mcsCOMPL_STAT doActionLogs(void)
{
    /* Perform each actionLog macro */
    logWarningAction("logging warning action");
    logInfoAction("logging info action");
    logTestAction("logging test action");
    logDebugAction("logging debug action");
    logExtDbgAction("logging extended debug action");

    return SUCCESS;
}


mcsCOMPL_STAT testAll(void)
{
    mcsUINT8 level;

    logSetStdoutLogLevel(logQUIET);
    
    /* Repeat loop changing log level */
    printf("###############################################################\n");
    printf("Testing File Logging\n");
    printf("###############################################################\n");
    for( level = logQUIET; level <= logEXTDBG; level++ )
    {
        /* Change verbose level */
        logSetFileLogLevel(level);
    
        /* Print verbose level */
        printf("*** File logging verbosity level is : %d\n", level);

        /* do each log */
        doLogs();
    }

    printf("###############################################################\n");
    printf("Testing Stdout Logging\n");
    printf("###############################################################\n");
    /* Repeat loop changing verbose level */
    for( level = logQUIET; level <= logEXTDBG; level++ )
    {
        /* Change verbose level */
        logSetStdoutLogLevel(level);

        /* Print verbose level */
        printf("*** Stdout logging verbosity level is : %d\n", level);
        
        /* do each log */
        doLogs();
    }
    
    printf("###############################################################\n");
    printf("Testing Action Logging\n");
    printf("###############################################################\n");
    /* Repeat loop changing action level */
    for( level = logQUIET; level <= logEXTDBG; level++ )
    {
        /* Change verbose level */
        logSetActionLogLevel(level);
        
        /* Print verbose level */
        printf("*** Action logging verbosity level is : %d\n", level);
        
        /* do each log */
        doActionLogs();
    }
    /* \todo handle bad cases */
    return SUCCESS;
}

mcsCOMPL_STAT test1(mcsLOGICAL fileLogState, mcsLOGICAL stdoutLogState)
{
    logExtDbg("ENTER_FUNC test1");

    /* Switch file logging ON or OFF */
    if (fileLogState == mcsTRUE)
    {
        logEnableFileLog();
    }
    else
    {
        logDisableFileLog();
    }

    /* Switch stdout logging ON or OFF */
    if (stdoutLogState == mcsTRUE)
    {
        logEnableStdoutLog();
    }
    else
    {
        logDisableStdoutLog();
    }

    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    printf("File logging switched %s\n",(fileLogState==mcsTRUE?"ON":"OFF"));
    printf("Stdout logging switched %s\n",(stdoutLogState==mcsTRUE?"ON":"OFF"));
    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");

    /* and test log, verbose, action */
    testAll();
     /* \todo handle bad cases */
    
    logExtDbg("EXIT_FUNC test1");
    return SUCCESS;
}

mcsCOMPL_STAT testNoFileLine(void)
{
    logTest("ENTER_FUNC testNoFileLine");
    
    logSetPrintFileLine(mcsFALSE);
    logTest("FileLine must not appear");

    logSetPrintFileLine(mcsTRUE);
    logTest("FileLine must appear");
    
    logTest("EXIT_FUNC testNoFileLine");
    return SUCCESS;
}


/*
 * Main
 */

int main(int argc, char ** argv)
{
    /* Init MCS services */
    mcsInit(argv[0]);

    /* test1 loops 
     * \todo handle function returns
     */
    test1(mcsFALSE, mcsFALSE);
    test1(mcsTRUE, mcsFALSE);
    test1(mcsTRUE, mcsTRUE);
    test1(mcsFALSE, mcsTRUE);

    /*
     * set test level and stdout only
     */
    logDisableFileLog();;
    logEnableStdoutLog();
    logSetStdoutLogLevel(logTEST);

    testNoFileLine();

    mcsExit();

    exit(EXIT_SUCCESS);
}

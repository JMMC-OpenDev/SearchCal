/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: logTest.c,v 1.14 2005-06-01 13:21:39 gzins Exp $"
*
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.13  2005/06/01 13:19:16  gzins
* Changed 'extended debug' to 'trace'
*
* Revision 1.12  2005/01/26 17:28:22  lafrasse
* Added automatic CVS history, refined user documentation, removed all
* ActionLog-related code, and changed SUCCESS in mcsSUCCESS and FAILURE in
* mcsFAILURE
*
* gzins     20-Dec-2004  Added tests for logAddToStdoutLogAllowedModList and
*                        logClearStdoutLogAllowedModList functions
*
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
* lafrasse  30-Jun-2004  Changed some APIs :
*                        logSetLog -> logSetFileLogState
*                        logSetLogLevel -> logSetFileLogLevel
*                        logGetLogLevel -> logGetFileLogLevel
*                        logSetVerbose -> logSetStdoutLogState
*                        logSetVerboseLevel -> logSetStdoutLogLevel
*                        logGetVerboseLevel -> logGetStdoutLogLevel
*                        logSetActionLevel -> logSetActionLogLevel
*                        logGetActionLevel -> logGetActionLogLevel
*
* mella     07-May-2004  Created
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
    logTrace("logging trace");

    return mcsSUCCESS;
}


mcsCOMPL_STAT testAll(void)
{
    mcsUINT8 level;

    logSetStdoutLogLevel(logQUIET);
    
    /* Repeat loop changing log level */
    printf("###############################################################\n");
    printf("Testing File Logging\n");
    printf("###############################################################\n");
    for( level = logQUIET; level <= logTRACE; level++ )
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
    for( level = logQUIET; level <= logTRACE; level++ )
    {
        /* Change verbose level */
        logSetStdoutLogLevel(level);

        /* Print verbose level */
        printf("*** Stdout logging verbosity level is : %d\n", level);
        
        /* do each log */
        doLogs();
    }
    
    return mcsSUCCESS;
}

mcsCOMPL_STAT test1(mcsLOGICAL fileLogState, mcsLOGICAL stdoutLogState)
{
    logTrace("ENTER_FUNC test1");

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
    
    logTrace("EXIT_FUNC test1");
    return mcsSUCCESS;
}

mcsCOMPL_STAT testNoFileLine(void)
{
    logTest("ENTER_FUNC testNoFileLine");
    
    logSetPrintFileLine(mcsFALSE);
    logTest("FileLine must not appear");

    logSetPrintFileLine(mcsTRUE);
    logTest("FileLine must appear");
    
    logTest("EXIT_FUNC testNoFileLine");
    return mcsSUCCESS;
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

    /* Check module name filtering */
    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    printf("Accept log messages from 'mcs' module only \n");
    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    logAddToStdoutLogAllowedModList("mcs");
    test1(mcsFALSE, mcsTRUE);
    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    printf("Clear list of allowed modules. All modules can log messages!!\n");
    printf("ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    logClearStdoutLogAllowedModList();
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

/*******************************************************************************
*  JMMC Project
*  
*  "@(#) $Id: logTest.c,v 1.2 2004-05-13 14:04:40 mella Exp $"
*
* who       when        what
* --------  --------    --------------------------------------------
* mella     07/05/04    creation 
* 
*/

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

    logSetVerboseLevel(logQUIET);
    
    /* Repeat loop changing log level */
    printf("################################################################################\n");
    printf("Testing log\n");
    printf("################################################################################\n");
    for( level = logQUIET; level <= logEXTDBG; level++ ){
        /* Change verbose level */
        logSetLogLevel(level);
    
        /* Print verbose level */
        printf("log level is:%d\n",level);

        /* do each log */
        doLogs();
    }

    printf("################################################################################\n");
    printf("Testing verbose\n");
    printf("################################################################################\n");
    /* Repeat loop changing verbose level */
    for( level = logQUIET; level <= logEXTDBG; level++ ){
        /* Change verbose level */
        logSetVerboseLevel(level);

        /* Print verbose level */
        printf("verbose level is:%d\n",level);
        
        /* do each log */
        doLogs();
    }
    
    printf("################################################################################\n");
    printf("Testing action\n");
    printf("################################################################################\n");
    /* Repeat loop changing action level */
    for( level = logQUIET; level <= logEXTDBG; level++ ){
        /* Change verbose level */
        logSetActionLevel(level);
        
        /* Print verbose level */
        printf("action level is:%d\n",level);
        
        /* do each log */
        doActionLogs();
    }
    /* \todo handle bad cases */
    return SUCCESS;
}

mcsCOMPL_STAT test1(mcsLOGICAL log, mcsLOGICAL verbose)
{
    logExtDbg("ENTER_FUNC test1");
    /* toggle log and verbose */
    logSetLog(log);
    logSetVerbose(verbose);
    printf("tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");
    printf("log toggled to %d\n",log);
    printf("verbose toggled to %d\n", verbose);
    printf("tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n");

    /* and test log, verbose, action */
    testAll();
     /* \todo handle bad cases */
    
    logExtDbg("EXIT_FUNC test1");
    return SUCCESS;
}

mcsCOMPL_STAT testNoFileLine(void){
    logTest("ENTER_FUNC testNoFileLine");
    
    logSetPrintFileLine(mcsFALSE);
    logTest("FileLine must not appear");

    logSetPrintFileLine(mcsTRUE);
    logTest("FileLine must appear");
    
    logTest("EXIT_FUNC testNoFileLine");
    return SUCCESS;
}

int main(int argc, char ** argv)
{
    /* Init names of process and module */
    logIdentify(argv[0],mcsLOG);
    
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
    logSetLog(mcsFALSE);
    logSetVerbose(mcsTRUE);
    logSetVerboseLevel(logTEST);
    
    testNoFileLine();

    
    exit(0);
}
   


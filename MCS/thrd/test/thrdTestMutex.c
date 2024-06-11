/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
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
#include "thrdThread.h"
#include "thrdMutex.h"


/*
 * Gloval variables
 */
thrdMUTEX myMutex;


/*
 * Local functions
 */
thrdFCT_RET myThreadFunction(thrdFCT_ARG param)
{
    int i;

    /* Mutex locking */
    if (thrdMutexLock(&myMutex) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    for (i = 0; i <= 10; i++)
    {
        printf("%s - %d\n", (char*)param, i);
    }

    /* Mutex unlocking */
    if (thrdMutexUnlock(&myMutex) == mcsFAILURE)
    {
        errCloseStack();
    }

    return NULL;
}
 

/* 
 * Main
 */
int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTRACE);

    /* Mutex initialisation */
    if (thrdMutexInit(&myMutex) == mcsFAILURE)
    {
        errCloseStack();
        exit(-1);
    }

    /* Thread creation */
    thrdTHREAD_STRUCT           myThread;
    myThread.function  = myThreadFunction;
    myThread.parameter = "Thread 1";
    thrdThreadCreate(&myThread);

    /* Launch a function in parallel execution */
    myThreadFunction("Thread 2");

    /* Wait for the thread end */
    thrdThreadWait(&myThread);

    /* Close MCS services */
    mcsExit();
    
    /* Mutex destruction */
    if (thrdMutexDestroy(&myMutex) == mcsFAILURE)
    {
        errCloseStack();
        exit(-1);
    }

    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

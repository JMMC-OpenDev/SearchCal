/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTestThread.c,v 1.1 2005-10-21 15:09:01 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

static char *rcsId="@(#) $Id: thrdTestThread.c,v 1.1 2005-10-21 15:09:01 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
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
#include "thrdThread.h"


/*
 * Local functions
 */
thrdFCT_RET myThreadFunction(thrdFCT_ARG param)
{
    int i;
    for (i=0; i<=100; i++)
    {
        printf("%s - %d\n", (char*)param, i);
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

    /* Thread creation */
    thrdTHREAD           myThread;
    myThread.function  = myThreadFunction;
    myThread.parameter = "Thread 1";
    thrdThreadCreate(&myThread);

    /* Launch a function in parallel execution */
    myThreadFunction("Thread 2");

    /* Wait for the thread end */
    thrdThreadWait(&myThread);

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

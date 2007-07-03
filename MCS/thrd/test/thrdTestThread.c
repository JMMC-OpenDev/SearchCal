/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTestThread.c,v 1.4 2007-07-03 12:12:30 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006/10/26 08:03:03  gzins
 * Renamed thrdTHREAD to thrdTHREAD_STRUCT
 *
 * Revision 1.2  2006/05/11 13:04:57  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.1  2005/10/21 15:09:01  lafrasse
 * thrdThread creation
 *
 ******************************************************************************/

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: thrdTestThread.c,v 1.4 2007-07-03 12:12:30 lafrasse Exp $";

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


/*
 * Local functions
 */
thrdFCT_RET myThreadFunction(thrdFCT_ARG param)
{
    int i;
    for (i=0; i<=100; i++)
    {
        printf("%s - %d\n", (char*)param, i);

        if (i%7 == 0)
        {
            printf("%s - Sleeping for 1 second.\n", (char*)param);
            sleep(1);
        }
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

    printf("---------------------------- Test 1 ---------------------------\n");

    /* Thread creation */
    printf("Main() - Launching Thread1() ...\n");
    thrdTHREAD_STRUCT thread1;
    thread1.function  = myThreadFunction;
    thread1.parameter = "Thread1()";
    thrdThreadCreate(&thread1);

    /* Launch a function in parallel execution */
    printf("Main() - Running ...\n");
    myThreadFunction("Main()");

    /* Wait for the thread end */
    printf("Main() - Waiting Thread1() ...\n");
    thrdThreadWait(&thread1);
    printf("OK.\n");

    printf("---------------------------- Test 2 ---------------------------\n");

    /* Thread relaunch */
    printf("Main() - Launching Thread2() ...\n");
    thrdTHREAD_STRUCT thread2;
    thread2.function  = myThreadFunction;
    thread2.parameter = "Thread2()";
    thrdThreadCreate(&thread2);
    printf("OK.\n");

    /* Wait for the thread end */
    printf("Main() - Sleeping 10 seconds ...\n");
    sleep(10);
    printf("OK.\n");

    /* Killing thread */
    printf("Main() - Killing Thread2() ...\n");
    thrdThreadKill(&thread2);
    printf("OK.\n");

    printf("----------------------------- End -----------------------------\n");

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

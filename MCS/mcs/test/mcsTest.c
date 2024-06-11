/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Simple main calling basic mcs functions.
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>


/*
 * MCS Headers 
 */


/*
 * Local Headers 
 */
#include "mcs.h"
#include "mcsPrivate.h"

/*
 * Local variables
 */
mcsMUTEX staticMutex = MCS_MUTEX_STATIC_INITIALIZER;
mcsMUTEX recursiveMutex = MCS_RECURSIVE_MUTEX_INITIALIZER;

/* 
 * Main
 */

int main (int argc, char *argv[])
{
    if (mcsMutexLock(&staticMutex) == mcsFAILURE)
    {
        printf("Could not lock static mutex.\n");
    }

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    printf("Processus   Name is '%s'.\n", mcsGetProcName());
    printf("Environment Name is '%s'.\n", mcsGetEnvName());

    if (mcsMutexLock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not lock reentrant mutex.\n");
    }
    if (mcsMutexUnlock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not unlock reentrant mutex.\n");
    }

    if (mcsMutexLock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not lock reentrant mutex (1).\n");
    }
    if (mcsMutexLock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not lock reentrant mutex (2).\n");
    }
    if (mcsMutexUnlock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not unlock reentrant mutex (1).\n");
    }
    if (mcsMutexUnlock(&recursiveMutex) == mcsFAILURE)
    {
        printf("Could not unlock reentrant mutex (2).\n");
    }

    /* Close MCS services */
    mcsExit();
    
    if (mcsMutexUnlock(&staticMutex) == mcsFAILURE)
    {
        printf("Could not lock static mutex.\n");
    }

    /* Exit from the application with mcsSUCCESS */
    exit (EXIT_SUCCESS);
}

/*___oOo___*/

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

    mcsMUTEX mutex;
    if (mcsMutexInit(&mutex) == mcsFAILURE)
    {
        printf("Could not intialize mutex.\n");
    }
    if (mcsMutexLock(&mutex) == mcsFAILURE)
    {
        printf("Could not lock mutex.\n");
    }
    if (mcsMutexUnlock(&mutex) == mcsFAILURE)
    {
        printf("Could not unlock mutex.\n");
    }
    if (mcsMutexDestroy(&mutex) == mcsFAILURE)
    {
        printf("Could not destroy mutex.\n");
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

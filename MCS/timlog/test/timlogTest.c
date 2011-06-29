/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program for timer log facility.
 */

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
#include "timlog.h"
#include "timlogPrivate.h"

/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    /* Start ACTION */
    timlogInfoStart("ACTION_1");
    timlogInfoStart("ACTION_2");

    sleep (1);
    timlogStop("ACTION_2");
    
    sleep (1);
    timlogStop("ACTION_2");
 
    sleep (1);
    timlogStop("ACTION_1");
 
    timlogClear();

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with mcsSUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

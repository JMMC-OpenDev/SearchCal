/*******************************************************************************
* JMMC project
*
* "@(#) $Id: timlogTest.c,v 1.1 2004-12-17 10:06:44 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     17-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Test program for timer log facility.
 */

static char *rcsId="@(#) $Id: timlogTest.c,v 1.1 2004-12-17 10:06:44 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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

/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == FAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    /* Start ACTION */
    timlogStart(logINFO, "ACTION_1");
    timlogStart(logINFO, "ACTION_2");

    sleep (1);
    timlogStop("ACTION_2");
    
    sleep (1);
    timlogStop("ACTION_2");
 
    sleep (1);
    timlogStop("ACTION_1");
 
    timlogClear();

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

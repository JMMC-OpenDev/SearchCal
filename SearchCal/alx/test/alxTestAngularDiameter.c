/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestAngularDiameter.c,v 1.1 2005-01-21 08:14:25 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * 
 * scetre    11-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes the angular angle of the star. 
 */

static char *rcsId="@(#) $Id: alxTestAngularDiameter.c,v 1.1 2005-01-21 08:14:25 gluck Exp $"; 
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
#include "alx.h"
#include "alxPrivate.h"


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

    logSetStdoutLogLevel(logTEST);

    alxCONFIDENCE_INDEX confIdx;
    mcsFLOAT mgB=10.063;
    mcsFLOAT mgV=8.402;
    mcsFLOAT mgR=6.60;
    mcsFLOAT mgK=3.21;
    mcsFLOAT angularDiam, deltaAngularDiameter;
    if (alxComputeAngularDiameter(mgB,
                                  mgV,
                                  mgR,
                                  mgK,
                                  &angularDiam,
                                  &deltaAngularDiameter,
                                  &confIdx)== FAILURE)
    {
        return FAILURE;
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestVisibility.c,v 1.4 2005-03-30 12:48:46 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/02/17 19:05:54  gzins
 * Added test for 10.0 um
 *
 * Revision 1.2  2005/02/12 15:18:56  gzins
 * Set logging service for test; do not print time stamp and file/line information and set level to logTEST
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    21-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes visibilities.
 */ 

static char *rcsId="@(#) $Id: alxTestVisibility.c,v 1.4 2005-03-30 12:48:46 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main (int argc, char *argv[])
{
     /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting...");
   
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    mcsFLOAT angularDiameter=1.35;
    mcsFLOAT angularDiameterError=0.13;
    mcsFLOAT baseMax=100;
    mcsFLOAT wavelength=2.2;
    alxVISIBILITIES visibilities;
    
    printf("for : \n");
    printf("\t diam       = %f (mas)\n", angularDiameter);
    printf("\t diam error = %f \n", angularDiameterError);
    printf("\t baseMax    = %f (m)\n", baseMax);
    printf("\t wavelength = %f (microm)\n", wavelength);
    
    if (alxComputeVisibility(angularDiameter,
                                angularDiameterError,
                                baseMax,
                                wavelength,
                                &visibilities)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    printf("we compute visibility :\n");
    printf("\t V   = %f\n", visibilities.vis);
    printf("\t V²  = %f\n", visibilities.vis2);
    printf("\tdV² = %f\n", visibilities.vis2Error);
/****************************/
    angularDiameter=0.452;
    angularDiameterError=0.031;
    baseMax=102.45;
    wavelength=0.6;
    
    printf("for : \n");
    printf("\t diam       = %f (mas)\n", angularDiameter);
    printf("\t diam error = %f \n", angularDiameterError);
    printf("\t baseMax    = %f (m)\n", baseMax);
    printf("\t wavelength = %f (microm)\n", wavelength);
    
    if (alxComputeVisibility(angularDiameter,
                                angularDiameterError,
                                baseMax,
                                wavelength,
                                &visibilities)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    printf("we compute visibility :\n");
    printf("\t V   = %f\n", visibilities.vis);
    printf("\t V²  = %f\n", visibilities.vis2);
    printf("\tdV²  = %f\n", visibilities.vis2Error);
    logInfo("Exiting...");
    
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes visibilities.
 */ 


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

    mcsDOUBLE angularDiameter=1.35;
    mcsDOUBLE angularDiameterError=0.13;
    mcsDOUBLE baseMax=100;
    mcsDOUBLE wavelength=2.2;
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
 
    /****************************/
    angularDiameter=0.418;
    angularDiameterError=0.029;
    baseMax=102.45;
    wavelength=2.2;
    
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
    
    /****************************/
    angularDiameter=1.296;
    angularDiameterError=0.089;
    baseMax=102.45;
    wavelength=2.2;
    
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
    
     /****************************/
    angularDiameter=1.096;
    angularDiameterError=0.076;
    baseMax=102.45;
    wavelength=2.2;
    
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
    
    /****************************/
    angularDiameter=0.120;
    angularDiameterError=0.024;
    baseMax=102.45;
    wavelength=2.2;
    
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
    
    /****************************/
    angularDiameter=0.369;
    angularDiameterError=0.074;
    baseMax=102.45;
    wavelength=2.2;
    
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
    
     /****************************/
    angularDiameter=0.331;
    angularDiameterError=0.066;
    baseMax=102.45;
    wavelength=2.2;
    
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

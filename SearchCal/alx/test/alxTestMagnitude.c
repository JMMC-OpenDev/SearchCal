/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestMagnitude.c,v 1.1 2005-01-21 08:14:25 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 *
 * scetre    22-Sep-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes magnitudes.
 *
 */

static char *rcsId="@(#) $Id: alxTestMagnitude.c,v 1.1 2005-01-21 08:14:25 gluck Exp $"; 
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
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == FAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTEST);
 
    mcsFLOAT B=6.6, V=5.77; 
    mcsFLOAT R, I, J, H, K, L, M;    
    alxCONFIDENCE_INDEX confIdx;
    if (alxComputeMagnitudesForBrightStar
        ("B2.5V", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == FAILURE)
    {
        errCloseStack();
        return FAILURE;
    }

    B=6.6, V=5.77;    
    if (alxComputeMagnitudesForBrightStar
        ("K2.5V", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == FAILURE)
    {
        errCloseStack();
        return FAILURE;
    }

    B=6.6, V=5.77;    
    if (alxComputeMagnitudesForBrightStar
        ("B6I", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == FAILURE)
    {
        errCloseStack();
        return FAILURE;
    }
    if (alxComputeRealMagnitudes
        (10, 5, 165, &B, &V, &R, &I, &J, &H, &K, &L, &M) == FAILURE)
    {
        errCloseStack();
        return FAILURE;
    }

/*    mcsFLOAT plx=7.80;
    mcsFLOAT gLat=-23.23;
    mcsFLOAT gLon=167.01;
    mcsFLOAT e_b_v=99.99;
    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == FAILURE)
    {
        return FAILURE;
    }
    
    logTest("Bo = %0.3f",B);
    logTest("Vo = %0.3f",V);
    logTest("Ro = %0.3f",R);
    logTest("Io = %0.3f",I);
    logTest("Jo = %0.3f",J);
    logTest("Ho = %0.3f",H);
    logTest("Ko = %0.3f",K);
    logTest("Lo = %0.3f",L);
    logTest("Mo = %0.3f",M);
   
    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == FAILURE)
    {
        return FAILURE;
    }
    
    logTest("Bo = %0.3f",B);
    logTest("Vo = %0.3f",V);
    logTest("Ro = %0.3f",R);
    logTest("Io = %0.3f",I);
    logTest("Jo = %0.3f",J);
    logTest("Ho = %0.3f",H);
    logTest("Ko = %0.3f",K);
    logTest("Lo = %0.3f",L);
    logTest("Mo = %0.3f",M);

    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == FAILURE)
    {
        return FAILURE;
    }
    
    logTest("Bo = %0.3f",B);
    logTest("Vo = %0.3f",V);
    logTest("Ro = %0.3f",R);
    logTest("Io = %0.3f",I);
    logTest("Jo = %0.3f",J);
    logTest("Ho = %0.3f",H);
    logTest("Ko = %0.3f",K);
    logTest("Lo = %0.3f",L);
    logTest("Mo = %0.3f",M);*/
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestMagnitude.c,v 1.5 2005-02-17 19:05:23 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/02/12 15:18:56  gzins
 * Set logging service for test; do not print time stamp and file/line information and set level to logTEST
 *
 * Revision 1.3  2005/01/31 13:32:53  scetre
 * *** empty log message ***
 *
 * Revision 1.2  2005/01/24 10:56:25  scetre
 * Changed valid format for spectral type
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 *
 * scetre    22-Sep-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes magnitudes.
 *
 */

static char *rcsId="@(#) $Id: alxTestMagnitude.c,v 1.5 2005-02-17 19:05:23 gzins Exp $"; 
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
    /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    mcsFLOAT B=6.6, V=5.77; 
    mcsFLOAT R, I, J, H, K, L, M;    
    alxCONFIDENCE_INDEX confIdx;
    if (alxComputeMagnitudesForBrightStar
        ("B2.5V", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    B=6.6, V=5.77;    
    if (alxComputeMagnitudesForBrightStar
        ("K2.5V", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    B=6.6, V=5.77;    
    if (alxComputeMagnitudesForBrightStar
        ("B6I", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    if (alxComputeRealMagnitudes
        (10, 5, 165, &B, &V, &R, &I, &J, &H, &K, &L, &M) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    B=6.6, V=5.77;    
    if (alxComputeMagnitudesForBrightStar
        ("M8", B, V, &R, &I, &J, &H, &K, &L, &M, &confIdx) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

/*    mcsFLOAT plx=7.80;
    mcsFLOAT gLat=-23.23;
    mcsFLOAT gLon=167.01;
    mcsFLOAT e_b_v=99.99;
    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == mcsFAILURE)
    {
        return mcsFAILURE;
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
   
    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == mcsFAILURE)
    {
        return mcsFAILURE;
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

    if (alxComputeRealMagnitude(plx, gLat, gLon, e_b_v, &M, &L, &K, &H, &J, &I, &R, &V, &B) == mcsFAILURE)
    {
        return mcsFAILURE;
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

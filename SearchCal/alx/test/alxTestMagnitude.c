/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestMagnitude.c,v 1.6 2005-02-21 19:36:30 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/17 19:05:23  gzins
 * Changed remaining FAILURE to mcsFAILURE
 *
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

static char *rcsId="@(#) $Id: alxTestMagnitude.c,v 1.6 2005-02-21 19:36:30 gzins Exp $"; 
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

    mcsFLOAT magnitudes[alxNB_BANDS];    
    alxCONFIDENCE_INDEX confIndexes[alxNB_BANDS];
    magnitudes[alxB_BAND] = 6.6;
    magnitudes[alxV_BAND] = 5.77;
    if (alxComputeMagnitudesForBrightStar
        ("B2.5V", magnitudes, confIndexes) == mcsFAILURE)
    {
        errCloseStack();
    }

    magnitudes[alxB_BAND] = 6.6;
    magnitudes[alxV_BAND] = 5.77;
    if (alxComputeMagnitudesForBrightStar
        ("K2.5V", magnitudes, confIndexes) == mcsFAILURE)
    {
        errCloseStack();
    }

    magnitudes[alxB_BAND] = 6.6;
    magnitudes[alxV_BAND] = 5.77;
    if (alxComputeMagnitudesForBrightStar
        ("A0I", magnitudes, confIndexes) == mcsFAILURE)
    {
        errCloseStack();
    }

    if (alxComputeRealMagnitudes
        (10, 5, 165, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    printf("magnitudes = %f\n", magnitudes[alxI_BAND]); 

    magnitudes[alxB_BAND] = 6.6;
    magnitudes[alxV_BAND] = 5.77;
    if (alxComputeMagnitudesForBrightStar
        ("M8", magnitudes, confIndexes) == mcsFAILURE)
    {
        errCloseStack();
    }

    printf("magnitudes = %f\n", magnitudes[alxI_BAND]); 
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

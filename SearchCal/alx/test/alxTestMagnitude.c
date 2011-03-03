/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestMagnitude.c,v 1.18 2011-03-03 12:59:53 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.17  2011/02/23 15:15:19  lafrasse
 * Updated to reflect alx API changes.
 *
 * Revision 1.16  2006/05/11 13:03:29  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.15  2006/04/07 06:13:24  gzins
 * Adjust logging service for test
 *
 * Revision 1.14  2006/04/06 11:52:21  gzins
 * Set confidence index
 *
 * Revision 1.13  2005/12/22 10:09:14  scetre
 * Updated test
 *
 * Revision 1.12  2005/12/19 20:46:49  gzins
 * Updated test programs according new function names
 *
 * Revision 1.11  2005/12/02 12:05:59  scetre
 * Added test of faint missing magnitude
 *
 * Revision 1.10  2005/07/06 05:07:15  gzins
 * Fixed API to alxComputeAngularDiameter
 *
 * Revision 1.9  2005/03/30 12:48:46  scetre
 * Changed API
 *
 * Revision 1.8  2005/02/22 16:22:04  gzins
 * Updated test after test on B-V delta has been added
 *
 * Revision 1.7  2005/02/22 10:18:08  gzins
 * Initialized magnitudes to blanked value before alxComputeMagnitudesForBrightStar
 *
 * Revision 1.6  2005/02/21 19:36:30  gzins
 * Updated according to alxComputeMagnitudesForBrightStar and alxComputeRealMagnitudes changes.
 *
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

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: alxTestMagnitude.c,v 1.18 2011-03-03 12:59:53 lafrasse Exp $";

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

    alxMAGNITUDES magnitudes;    
    int band;
    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;        
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].value = 6.6;
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxV_BAND].value = 6.67;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    if (alxComputeMagnitudesForBrightStar
        ("B2.5V", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }
    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;        
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].value = 6.6;
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxV_BAND].value = 5.77;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    if (alxComputeMagnitudesForBrightStar
        ("K2.5V", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;        
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxB_BAND].value = 6.6;
    magnitudes[alxV_BAND].value = 6.67;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    if (alxComputeMagnitudesForBrightStar
        ("A0I", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    mcsDOUBLE av;
    alxComputeExtinctionCoefficient(&av, 10, 5, 165);        
    if (alxComputeCorrectedMagnitudes
        (av, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;        
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].value = 6.6;
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxV_BAND].value = 5.77;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    if (alxComputeMagnitudesForBrightStar
        ("M8", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }
printf("*********************\n");
    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].value = 8.452;
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxV_BAND].value = 8.267;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    magnitudes[alxR_BAND].value = 8.020;
    magnitudes[alxR_BAND].isSet = mcsTRUE;
    magnitudes[alxI_BAND].value = 7.880;
    magnitudes[alxI_BAND].isSet = mcsTRUE;
    magnitudes[alxJ_BAND].value = 7.767;
    magnitudes[alxJ_BAND].isSet = mcsTRUE;
    magnitudes[alxH_BAND].value = 7.627;
    magnitudes[alxH_BAND].isSet = mcsTRUE;
    magnitudes[alxK_BAND].value = 7.639;
    magnitudes[alxK_BAND].isSet = mcsTRUE;
    magnitudes[alxL_BAND].value = 7.120;
    magnitudes[alxL_BAND].isSet = mcsTRUE;
   
    /* extinction */
    printf("extinction\n");
    alxComputeExtinctionCoefficient(&av, 9.10, -23.45, 166.61);
    if (alxComputeCorrectedMagnitudes
        (av, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    /* complete */
    printf("complete\n");
    if (alxComputeMagnitudesForBrightStar
        ("A0", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    printf("*********************\n");
    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxB_BAND].value = 8.761;
    magnitudes[alxB_BAND].isSet = mcsTRUE;
    magnitudes[alxV_BAND].value = 7.679;
    magnitudes[alxV_BAND].isSet = mcsTRUE;
    
   
    /* extinction */
    printf("extinction\n");
    alxComputeExtinctionCoefficient(&av, 1.20, -23.58, 166.37);    
    if (alxComputeCorrectedMagnitudes
        (av, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    /* complete */
    printf("complete\n");
    if (alxComputeMagnitudesForBrightStar
        ("K2", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }
    alxDIAMETERS diameters;
    if (alxComputeAngularDiameterForBrightStar(magnitudes[alxB_BAND],
                                               magnitudes[alxV_BAND],
                                               magnitudes[alxR_BAND],
                                               magnitudes[alxK_BAND],
                                               &diameters)== mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (alxComputeApparentMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    printf("*********** FAINT TEST **********\n");
    for (band = 0; band < alxNB_BANDS; band++)
    { 
        magnitudes[band].value = alxBLANKING_VALUE;
        magnitudes[band].isSet = mcsFALSE;
        magnitudes[band].confIndex = alxCONFIDENCE_LOW;
    }
    magnitudes[alxJ_BAND].value = 8.275;
    magnitudes[alxJ_BAND].isSet = mcsTRUE;
    magnitudes[alxK_BAND].value = 7.498;
    magnitudes[alxK_BAND].isSet = mcsTRUE;
     /* complete */
    printf("complete\n");
    if (alxComputeMagnitudesForFaintStar("A0", magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }
    alxDIAMETERS diameters2;
    alxDATA mgV, mgKj;
    if (alxComputeAngularDiameterForFaintStar(magnitudes[alxI_BAND],
                                              magnitudes[alxJ_BAND],
                                              magnitudes[alxK_BAND],
                                              magnitudes[alxH_BAND],
                                              mgV,
                                              mgKj,
                                              &diameters2)== mcsFAILURE)
    {
        return mcsFAILURE;
    }


    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

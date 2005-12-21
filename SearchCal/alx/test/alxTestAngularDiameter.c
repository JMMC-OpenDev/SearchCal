/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestAngularDiameter.c,v 1.9 2005-12-21 14:59:57 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/12/19 20:46:49  gzins
 * Updated test programs according new function names
 *
 * Revision 1.7  2005/07/06 05:07:05  gzins
 * Fixed API to alxComputeAngularDiameter
 * Added test case
 *
 * Revision 1.6  2005/03/30 12:48:46  scetre
 * Changed API
 *
 * Revision 1.5  2005/02/22 16:21:26  gzins
 * Updated for new alxComputeAngularDiameter API
 *
 * Revision 1.4  2005/02/17 19:05:23  gzins
 * Changed remaining FAILURE to mcsFAILURE
 *
 * Revision 1.3  2005/02/12 15:18:56  gzins
 * Set logging service for test; do not print time stamp and file/line information and set level to logTEST
 *
 * Revision 1.2  2005/02/10 08:15:14  gzins
 * Updated for new alxComputeAngularDiameter API
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    11-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes the angular angle of the star. 
 */

static char *rcsId="@(#) $Id: alxTestAngularDiameter.c,v 1.9 2005-12-21 14:59:57 scetre Exp $"; 
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
    /* Configure logging service */
    logSetStdoutLogLevel(logDEBUG);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    alxDATA mgB;
    alxDATA mgV;
    alxDATA mgR;
    alxDATA mgK;
    mgB.value = 11.190; mgB.confIndex = alxCONFIDENCE_HIGH; mgB.isSet = mcsTRUE;
    mgV.value = 10.079; mgV.confIndex = alxCONFIDENCE_HIGH; mgV.isSet = mcsTRUE;
    mgR.value = 7.530;  mgR.confIndex = alxCONFIDENCE_HIGH; mgR.isSet = mcsTRUE;
    mgK.value = 3.092;  mgK.confIndex = alxCONFIDENCE_HIGH; mgK.isSet = mcsTRUE;
    alxDIAMETERS diameters;
    if (alxComputeAngularDiameterForBrightStar
        (mgB, mgV, mgR, mgK, &diameters)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mgB.value = 2.808; 
    mgV.value = 2.870;
    mgR.value = 2.840;  
    mgK.value = 2.636;  
    if (alxComputeAngularDiameterForBrightStar
        (mgB, mgV, mgR, mgK, &diameters)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    alxDATA mgI;
    alxDATA mgJ;
    alxDATA mgKf;
    alxDATA mgH;
    mgI.value = 11.250; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsTRUE;
    mgJ.value = 9.062; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgKf.value = 8.143;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    mgH.value = 8.305;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    
    alxDIAMETERS diametersFaint;
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

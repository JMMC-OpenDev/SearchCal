/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestAngularDiameter.c,v 1.17 2011-02-23 15:15:19 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.16  2006/10/26 16:46:00  gzins
 * Added test cases - faint object and magnitude I unknown
 *
 * Revision 1.15  2006/07/17 08:23:58  scetre
 * Added test if magI is unknown
 *
 * Revision 1.14  2006/06/12 14:46:57  gzins
 * Added test for new polynomial coefficients
 *
 * Revision 1.13  2006/05/11 13:03:29  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.12  2006/04/07 06:13:24  gzins
 * Adjust logging service for test
 *
 * Revision 1.11  2006/04/06 14:08:32  gzins
 * Updated magnitude values for faint test
 *
 * Revision 1.10  2006/03/28 09:58:54  gzins
 * Added
 *
 * Revision 1.9  2005/12/21 14:59:57  scetre
 * Added test with faint object
 *
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

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: alxTestAngularDiameter.c,v 1.17 2011-02-23 15:15:19 lafrasse Exp $";

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
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
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
    alxDATA mgKj;
    mgI.value = 11.094; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsTRUE;
    mgJ.value = 10.418; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgH.value = 9.907;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    mgKf.value = 9.856;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    mgV.value = 10.079; mgV.confIndex = alxCONFIDENCE_HIGH; mgV.isSet = mcsTRUE;
    mgKj.value = 9.856;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    
    alxDIAMETERS diametersFaint;
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, mgV, mgKj, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mgI.value = 0.0; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsTRUE;
    mgJ.value = 0.25; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgH.value = 0.32;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    mgKf.value = 0.36;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, mgV, mgKj, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mgI.value = 0.0; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsFALSE;
    mgJ.value = 0.25; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgH.value = 0.32;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    mgKf.value = 0.36;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, mgV, mgKj, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mgI.value = 0.0; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsFALSE;
    mgJ.value = 13.658; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgH.value = 13.126;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    mgKf.value = 13.069;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, mgV, mgKj, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mgI.value = 0.0; mgI.confIndex = alxCONFIDENCE_HIGH; mgI.isSet = mcsFALSE;
    mgJ.value = 12.950; mgJ.confIndex = alxCONFIDENCE_HIGH; mgJ.isSet = mcsTRUE;
    mgH.value = 12.273;  mgH.confIndex = alxCONFIDENCE_HIGH; mgH.isSet = mcsTRUE;
    mgKf.value = 12.176;  mgKf.confIndex = alxCONFIDENCE_HIGH; mgKf.isSet = mcsTRUE;
    
    if (alxComputeAngularDiameterForFaintStar
        (mgI, mgJ, mgKf, mgH, mgV, mgKj, &diametersFaint)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

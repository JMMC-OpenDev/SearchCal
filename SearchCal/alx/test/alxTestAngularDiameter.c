/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

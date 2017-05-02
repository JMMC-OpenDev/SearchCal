/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes magnitudes.
 */


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

    alxSPECTRAL_TYPE decodedSpectralType;
    char* spectralTypes[] = {"-", "A0", "A0Ia", "A0Ib", "A0IV", "A0V", "A1III/IV", "A1V", "A2", "A2m", "A3", "A3IV", "A5", "A5V", "A7IV", "A7IV-V", "A8/A9V", "A8Vn", "A9V", "A9V...", "Am...", "Ap...", "B0IV...", "B1.5V", "B2", "B2III", "B2:IIIpshev", "B3IIIe", "B5", "B5III", "B5V", "B6III", "B7/B8V", "B8III", "B8V", "B8Vn", "B9", "B9.5IV:", "B9IIIMNp...", "B9IV", "F0", "F0IV...", "F2II/III", "F3Ia", "F5", "F5V", "F8", "G0", "G0Ib", "G0III...", "G0V", "G3Ib", "G3V", "G4Ibp...", "G5", "G5II...", "G5III", "G5IV", "G6/G8III", "G7III", "G8III", "G8IV/V", "K", "K0", "K0III", "K0IV", "K1Iabv", "K1III", "K1III/IV", "K1IIIvar", "K1/K2III", "K2", "K2III", "K2IIIvar", "K2IV", "K2/K3III", "K3Ib", "K3III", "K3IIvar", "K4III", "K4/K5III", "K5", "K5II", "K5III", "K5/M0III", "K7", "M0", "M0III", "M0I-M4Ia", "M1", "M1III", "M1IIIb", "M2Iabpe", "M3", "M3III", "M3/M4III", "M4.5IIIa", "M4III", "M4III:", "M5III", "M5/M6IV", "M6", "M6e-M7", "M6III", "M7III", "M8III:e", "Ma", "Mb", "Mc", "Md", "O", "O...", "O7", "A comp SB", "A0III SB", "A0V SB", "A3IVv SB", "A5Vv SB", "B2III SB", "B2IIIv SB", "B3V SB", "B5III SB", "B5V SB", "B6pv SB", "B8III SB", "B9p SB", "F2IV SB", "F4III SB", "F8V SB", "G2V SB", "G3Ibv SB", "G5III SB", "G8II SB", "G8III compSB", "G8III SB", "G8III-IV SB", "G8V SB", "K0III SB", "K0IV SB", "K0V SB", "K1III SB", "K3III SB", "K4II SB", "K5Ibv SB", "K5III SB", "M0III SB", "O9.5Ib SB", "K1III(+M)", "A8/9 V", "A1/2 III", "K2/3", "K2/3 III", "G5/6 V", "G6/8 V", "B8/9 II/III", NULL};
    char** spectralTypePtr = spectralTypes;
    while (*spectralTypePtr != NULL)
    {
        if (alxString2SpectralType(*spectralTypePtr, &decodedSpectralType) == mcsFAILURE)
        {
            errCloseStack();
        }
        spectralTypePtr++;
    }

    alxMAGNITUDES magnitudes;    
    int band;

    /* ------------------------------------------------------------------- */
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
    
    if (alxString2SpectralType("B2.5V", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    /* ------------------------------------------------------------------- */
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

    if (alxString2SpectralType("K2.5V", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    /* ------------------------------------------------------------------- */
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
    
    if (alxString2SpectralType("A0I", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    mcsDOUBLE av, e_Av, dist, e_dist;
    alxComputeExtinctionCoefficient(&av, &e_Av, &dist, &e_dist, 10, 0.2, 5, 165);        
    if (alxComputeCorrectedMagnitudes("test", av, magnitudes, mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    /* ------------------------------------------------------------------- */
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
    
    if (alxString2SpectralType("M8", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    printf("*********************\n");

    /* ------------------------------------------------------------------- */
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
    alxComputeExtinctionCoefficient(&av, &e_Av, &dist, &e_dist, 9.10, 0.2, -23.45, 166.61);
    if (alxComputeCorrectedMagnitudes("test", av, magnitudes, mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    /* complete */
    printf("complete\n");
    
    if (alxString2SpectralType("A0", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    printf("*********************\n");

    /* ------------------------------------------------------------------- */
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
    alxComputeExtinctionCoefficient(&av, &e_Av, &dist, &e_dist, 1.20, 0.2, -23.58, 166.37);    
    if (alxComputeCorrectedMagnitudes("test", av, magnitudes, mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    /* complete */
    printf("complete\n");
    
    if (alxString2SpectralType("K2", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForBrightStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }

    alxDIAMETERS diameters;
    alxDIAMETERS_COVARIANCE diametersCov;

    if (alxComputeAngularDiameters("test", magnitudes, e_Av, diameters, diametersCov, logTEST) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (alxComputeApparentMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    printf("*********** FAINT TEST **********\n");

    /* ------------------------------------------------------------------- */
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
    
    if (alxString2SpectralType("A0", &decodedSpectralType) == mcsFAILURE)
    {
        errCloseStack();
    }
    
    if (alxComputeMagnitudesForFaintStar(&decodedSpectralType, magnitudes) == mcsFAILURE)
    {
        errCloseStack();
    }
    if (alxComputeAngularDiameters("test", magnitudes, e_Av, diameters, diametersCov, logTEST) == mcsFAILURE)
    {
        return mcsFAILURE;
    }


    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

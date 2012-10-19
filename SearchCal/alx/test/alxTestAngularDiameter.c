/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes the angular angle of the star. 
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


    alxMAGNITUDES mag;
    alxDIAMETERS diameters;

    mag[alxB_BAND].value = 11.190; mag[alxB_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxB_BAND].isSet = mcsTRUE;
    mag[alxV_BAND].value = 10.079; mag[alxV_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxV_BAND].isSet = mcsTRUE;
    mag[alxR_BAND].value = 7.530;  mag[alxR_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxR_BAND].isSet = mcsTRUE;
    mag[alxI_BAND].value = 0.0;    mag[alxI_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxI_BAND].isSet = mcsFALSE;
    mag[alxJ_BAND].value = 0.0;    mag[alxJ_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxJ_BAND].isSet = mcsFALSE;
    mag[alxH_BAND].value = 0.0;    mag[alxH_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxH_BAND].isSet = mcsFALSE;
    mag[alxK_BAND].value = 3.092;  mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;
    mag[alxL_BAND].value = 0.0;    mag[alxL_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxL_BAND].isSet = mcsFALSE;
    mag[alxM_BAND].value = 0.0;    mag[alxM_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxM_BAND].isSet = mcsFALSE;

    if ( alxComputeAngularDiameters(mag,diameters) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    mag[alxB_BAND].value = 2.808;  mag[alxB_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxB_BAND].isSet = mcsTRUE; 
    mag[alxV_BAND].value = 2.870;  mag[alxV_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxV_BAND].isSet = mcsTRUE;
    mag[alxR_BAND].value = 2.840;  mag[alxR_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxR_BAND].isSet = mcsTRUE;
    mag[alxK_BAND].value = 2.636;  mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;

    if ( alxComputeAngularDiameters(mag,diameters) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    alxDIAMETERS diametersFaint;

    mag[alxB_BAND].value = 0.0;    mag[alxB_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxB_BAND].isSet = mcsFALSE;
    mag[alxV_BAND].value = 10.079; mag[alxV_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxV_BAND].isSet = mcsTRUE;
    mag[alxR_BAND].value = 0.0;    mag[alxR_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxR_BAND].isSet = mcsFALSE;
    mag[alxI_BAND].value = 11.094; mag[alxI_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxI_BAND].isSet = mcsTRUE;
    mag[alxJ_BAND].value = 10.418; mag[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxJ_BAND].isSet = mcsTRUE;
    mag[alxH_BAND].value = 9.907;  mag[alxH_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxH_BAND].isSet = mcsTRUE;
    mag[alxK_BAND].value = 9.856;  mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;
    mag[alxL_BAND].value = 0.0;    mag[alxL_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxL_BAND].isSet = mcsFALSE;
    mag[alxM_BAND].value = 0.0;    mag[alxM_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxM_BAND].isSet = mcsFALSE;
    
    if ( alxComputeAngularDiameters(mag,diametersFaint) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    mag[alxI_BAND].value = 0.0;    mag[alxI_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxI_BAND].isSet = mcsFALSE;
    mag[alxJ_BAND].value = 0.25;   mag[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxJ_BAND].isSet = mcsTRUE;
    mag[alxH_BAND].value = 0.32;   mag[alxH_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxH_BAND].isSet = mcsTRUE;
    mag[alxK_BAND].value = 0.36;   mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;

    if ( alxComputeAngularDiameters(mag,diametersFaint) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    mag[alxI_BAND].value = 0.0;    mag[alxI_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxI_BAND].isSet = mcsFALSE;
    mag[alxJ_BAND].value = 13.658; mag[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxJ_BAND].isSet = mcsTRUE;
    mag[alxH_BAND].value = 13.126; mag[alxH_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxH_BAND].isSet = mcsTRUE;
    mag[alxK_BAND].value = 13.069; mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;

    if ( alxComputeAngularDiameters(mag,diametersFaint) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    mag[alxI_BAND].value = 0.0;    mag[alxI_BAND].confIndex = alxNO_CONFIDENCE;   mag[alxI_BAND].isSet = mcsFALSE;
    mag[alxJ_BAND].value = 12.950; mag[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxJ_BAND].isSet = mcsTRUE;
    mag[alxH_BAND].value = 12.273; mag[alxH_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxH_BAND].isSet = mcsTRUE;
    mag[alxK_BAND].value = 12.176; mag[alxK_BAND].confIndex = alxCONFIDENCE_HIGH; mag[alxK_BAND].isSet = mcsTRUE;

    if ( alxComputeAngularDiameters(mag,diametersFaint) == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes galactic coordinates.
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

    logInfo("Starting...");

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    mcsDOUBLE ra, dec, gLat, gLon;
    mcsINT32 i;
    
    /*
     * Input data set
     * --------------
     */
    /* Input star name array */
    mcsSTRING32 starNameTable[11][1] ={
        {"North Pole"},
        {"Kap Sco"},
        {"Alf Aur"},
        {"Alf Cru"},
        {"Del Ori"},
        {"Alf Umi"},
        {"Lam Vel"},
        {"Tet Sco"},
        {"Alf Phe"},
        {"Psi UMa"},
        {"Gal Center"}
    };
    
    /* Input right ascension (ra) array */
    mcsDOUBLE raTable[11][3] ={
        {0, 0, 0},
        {17, 42, 29.3},
        { 5, 16, 41.4},
        {12, 26, 35.9},
        { 5, 32,  0.4},
        { 2, 31, 49.1},
        { 9,  7, 59.8},
        {17, 37, 19.1},
        { 0, 26, 17.0},
        {11,  9, 39.8},
        {17, 45, 36.0}
    };
    
    /* Input declinaison (dec) array */
    mcsDOUBLE decTable[11][4] ={
        { 1, 90,  0,  0.0},
        {-1, 39,  1, 47.9},
        { 1, 45, 59, 52.8},
        {-1, 63,  5, 56.7},
        {-1,  0, 17, 56.7},
        { 1, 89, 15, 50.8},
        {-1, 43, 25, 57.3},
        {-1, 42, 59, 52.2},
        {-1, 42, 18, 21.5},
        { 1, 44, 29, 54.5},
        {-1, 28, 56,  0.0}
    };

    /*
     * data set execution
     * ------------------
     */
    for (i=0; i<11; i++)
    {
        /* Print star name for data set */
        logTest("-------------------------------------------------------\n");
        logTest("Star Name : %s\n", starNameTable[i]);

        /* Convert ra from hh/hm/hs to degrees (decimal) */
        ra = (raTable[i][0] + raTable[i][1]/60 + raTable[i][2]/3600) * 15;

        /* Convert dec from dh/dm/ds to degrees (decimal) */
        dec = decTable[i][0] * 
            (decTable[i][1] + decTable[i][2]/60 + decTable[i][3]/3600);

        /* Compute galactic coodinates */
        if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    
    logInfo("Exiting...");

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

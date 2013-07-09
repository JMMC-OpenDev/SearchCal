/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * This programs commpute the separation between 2 stars
 *
 * \synopsis
 * vobsStarSeparation raStar1 decStar1 raStar2 decStar2 
 *
 * @param raStar1 : right ascension in HH:MM:SS.TTT of star1
 * @param decStar1 : declinaison in HH:MM:SS.TTT of star1
 * @param raStar2 : right ascension in HH:MM:SS.TTT of star2
 * @param decStar2 : declinaison in HH:MM:SS.TTT of star2
 * 
 * This program prints out star coodinates in degrees and radian, and the gives
 * the star separation.
 */



/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <math.h>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "vobs.h"
#include "vobsPrivate.h"


/*
 * Local macros
 */
/* pi/180:  degrees to radians */
#define DD2R 0.017453292519943295769236907684886127134428718885417

/* 180/pi:  radians to degrees */
#define DR2D 57.295779513082320876798154814105170332405472466564

/* pi/(180*3600):  arcseconds to radians */
#define DAS2R 4.8481368110953599358991410235794797595635330237270e-6

/* 180*3600/pi :  radians to arcseconds */
#define DR2AS 2.0626480624709635515647335733077861319665970087963e5

/*
 * Local function
 */
void vobsPrintUsage(void)
{
    printf("Usage: %s <raStar1> <decStar1> <raStar2> <decStar2>\n",
           mcsGetProcName());
    printf("\tRA  is given as HH:MM:SS.TT\n");
    printf("\tDEC is given as DD:MM:SS.TT\n\n");
    exit(EXIT_SUCCESS);
}

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    // Check argument
    if (argc != 5)
    {
        vobsPrintUsage();
    }

    // Star 1
    vobsSTAR star1;
    mcsDOUBLE raStar1InDeg;
    star1.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, argv[1], vobsNO_CATALOG_ID);
    if (star1.GetRa(raStar1InDeg) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    mcsDOUBLE decStar1InDeg;
    star1.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, argv[2], vobsNO_CATALOG_ID);
    if (star1.GetDec(decStar1InDeg) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    // Convert deg to rad
    double raStar1InRad, decStar1InRad;
    raStar1InRad = raStar1InDeg * DD2R;
    decStar1InRad = decStar1InDeg * DD2R;

    // Print out star 1 coordinates
    printf("Star 1 :\n");
    printf("\tRA  : %6.4f deg\tDEC : %6.4f deg\n", raStar1InDeg, decStar1InDeg);
    printf("\tRA  : %6.4f rad\tDEC : %6.4f rad\n", raStar1InRad, decStar1InRad);

    // Star 2
    vobsSTAR star2;
    mcsDOUBLE raStar2InDeg;
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, argv[3], vobsNO_CATALOG_ID);
    if (star2.GetRa(raStar2InDeg) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    mcsDOUBLE decStar2InDeg;
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, argv[4], vobsNO_CATALOG_ID);
    if (star2.GetDec(decStar2InDeg) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }
    // Convert deg to rad
    double raStar2InRad, decStar2InRad;
    raStar2InRad = raStar2InDeg * DD2R;
    decStar2InRad = decStar2InDeg * DD2R;

    // Print out star 2 coordinates
    printf("Star 2 :\n");
    printf("\tRA  : %6.4f deg\tDEC : %6.4f deg\n", raStar2InDeg, decStar2InDeg);
    printf("\tRA  : %6.4f rad\tDEC : %6.4f rad\n", raStar2InRad, decStar2InRad);

    // Compute separation
    double cosTheta = sin(decStar1InRad) * sin(decStar2InRad) +
            cos(decStar1InRad) * cos(decStar2InRad) *
            cos(raStar1InRad - raStar2InRad);
    // Due to computation precision, it is  possible than cosTheta is
    // greater than 1.0. In this case, force it to 1.
    if (cosTheta > 1.0)
    {
        cosTheta = 1.0;
    }
    if (cosTheta < -1.0)
    {
        cosTheta = -1.0;
    }

    // Compute theta in arcseconds
    double theta = 3600 * acos(cosTheta) * DR2D;

    // Print out separation
    printf("\nSeparation --> %6.1f arcsec\n\n", theta);

    // Close MCS services
    mcsExit();

    // Exit from the application with SUCCESS
    exit(EXIT_SUCCESS);
}


/*___oOo___*/

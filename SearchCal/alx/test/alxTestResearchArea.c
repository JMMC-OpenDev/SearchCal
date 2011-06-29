/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


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

    logInfo("Starting...");

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with mcsFAILURE */
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    mcsDOUBLE magMin, magMax;
    mcsDOUBLE ra, dec;
    mcsDOUBLE areaSize=0.0;

    /* Test 1 */
    magMin =  6.0;
    magMax = 12.0;
    ra     =  0.0;
    dec    = 60.0;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    logTest ("Result 1 : ra = %.2f dec = %.2f -> size = %.2f should be 3.59",
             ra, dec, areaSize);

    /* Test 2 */
    magMin =   6.0;
    magMax =  12.0;
    ra     =   0.0;
    dec    = -60.0;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest ("Result 2 : ra = %.2f dec = %.2f -> size = %.2f should be 13.90",
             ra, dec, areaSize);

    /* Test 3  check lower and upper limit array which is 5.5 to 19.5*/
    magMin =  4.0;
    magMax = 21.0;
    ra     =  0.0;
    dec    =  0.0;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest ("Result 3 : ra = %.2f dec = %.2f -> size = %.2f should be 2.96",
             ra, dec, areaSize);

    /* Test 4 */
    magMin =  7.0;
    magMax = 14.0;
    ra     = -74.7;
    dec    = -37.0;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest ("Result 4 : ra = %.2f dec = %.2f -> size = %.2f should be 1.98",
             ra, dec, areaSize);

   /* Test 5 */
    magMin =  7.0;
    magMax = 14.0;
    ra     = 245.75;
    dec    = -24.39;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest ("Result 5 : ra = %.2f dec = %.2f -> size = %.2f should be 2.10",
             ra, dec, areaSize);

   /* Test 6 */
    magMin = 2.0;
    magMax = 4.0;
    ra     = 245.75;
    dec    = -24.39;
    if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &areaSize)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest ("Result 6 : ra = %.2f dec = %.2f -> size = %.2f should be 2.10",
             ra, dec, areaSize);
    logInfo("Exiting...");

    /* Close MCS services */
    mcsExit();

    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: alxTestResearchArea.c,v 1.1 2005-02-12 14:59:11 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
*******************************************************************************/

static char *rcsId="@(#) $Id: alxTestResearchArea.c,v 1.1 2005-02-12 14:59:11 gzins Exp $"; 
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
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting...");

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == FAILURE)
    {
        /* Exit from the application with FAILURE */
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    mcsFLOAT magMin, magMax;
    mcsFLOAT ra, dec;
    mcsFLOAT areaSize=0.0;

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

    logTest ("Result 1 : ra = %.2f dec = %.2f -> size = %.2f should be 6.37",
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
    logTest ("Result 2 : ra = %.2f dec = %.2f -> size = %.2f should be 24.53",
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
    logTest ("Result 3 : ra = %.2f dec = %.2f -> size = %.2f should be 5.25",
             ra, dec, areaSize);

    logInfo("Exiting...");

    /* Close MCS services */
    mcsExit();

    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


/* 
 * System Headers 
 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "timlog.h"

/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsPrivate.h"

int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    timlogStart(MODULE_ID, logINFO, "98", "testStar");
    logInfo("Starting ...");

    vobsSTAR star;
    mcsDOUBLE ra, dec;

    for (int el = 0; el < star.NbProperties(); el++)
    {
        printf("%s\t", star.GetProperty(el)->GetName());
    }
    printf("\n");

    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "-03 47 29.08", vobsNO_CATALOG_ID);
    star.GetRa(ra);
    logTest("RA: %s", star.GetProperty(vobsSTAR_POS_EQ_RA_MAIN)->GetValueOrBlank());
    logTest("ra (deg) = %f", ra);

    /* Test format DMS (DD MM SS.TT) */
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "24 06 18.5", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+89 59 59.9", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    /* Test format DMS (DD MM.mm) */
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+00 48.6", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "-06 45.9", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "-30 57.3", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    /* test invalid DEC formats */
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "1 2 3 4.5", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "1.2345", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
    star.ClearCache();
    star.GetDec(dec);
    logTest("DE: %s", star.GetProperty(vobsSTAR_POS_EQ_DEC_MAIN)->GetValueOrBlank());
    logTest("dec (deg) = %f", dec);

    // Reset:
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "24 06 18.5", vobsNO_CATALOG_ID, vobsCONFIDENCE_HIGH, mcsTRUE);

    vobsSTAR star2(star);
    for (int el2 = 0; el2 < star2.NbProperties(); el2++)
    {
        printf("%s\t", star2.GetProperty(el2)->GetValueOrBlank());
    }
    printf("\n");

    // Print out error stack if it is not empty
    if (errStackIsEmpty() == mcsFALSE)
    {
	logError("Errors:");
	errCloseStack();
    }

    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);
}

/*___oOo___*/

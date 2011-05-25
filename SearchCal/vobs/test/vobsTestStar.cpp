/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestStar.cpp,v 1.13 2011-03-03 13:09:43 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
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
    for (int el = 0; el < star.NbProperties(); el++)
    {
        printf("%s\t", star.GetNextProperty((mcsLOGICAL)(el==0))->GetName());
    }
    printf("\n");
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "-03 47 29.08", "");
    mcsDOUBLE ra;
    star.GetRa(ra);
    logTest("ra (deg) = %f", ra);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "24 06 18.5", "");
    mcsDOUBLE dec;
    star.GetDec(dec);
    logTest("dec (deg) = %f", dec);

    vobsSTAR star2=star;
    for (int el2 = 0; el2 < star2.NbProperties(); el2++)
    {
        printf("%s\t", star2.GetNextProperty((mcsLOGICAL)(el2==0))->GetValue());
    }
    printf("\n");
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestStar.cpp,v 1.8 2005-02-04 10:04:55 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStar.cpp,v 1.8 2005-02-04 10:04:55 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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

    logSetStdoutLogLevel(logINFO);
    timlogStart(MODULE_ID, logINFO, "98", "testStar");

    vobsSTAR star;
    for (int el = 0; el < star.NbProperties(); el++)
    {
        printf("%s\t", star.GetNextProperty((mcsLOGICAL)(el==0))->GetName());
    }
    printf("\n");
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "-03 47 29.08");
    mcsFLOAT ra;
    star.GetRa(ra);
    printf("ra (deg) = %f\n", ra);

    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "24 06 18.5");
    mcsFLOAT dec;
    star.GetDec(dec);
    printf("dec (deg) = %f\n", dec);

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

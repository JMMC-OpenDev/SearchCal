/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDate.c,v 1.5 2005-01-19 10:28:08 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  22-Jul-2004  Correted some typos, code factorization, and error
*                        management
* lafrasse  02-Aug-2004  Changed local includes to use miscDate headers
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDate.c,v 1.5 2005-01-19 10:28:08 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "miscDate.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);

    mcsBYTES32  time;
    int i;

    /* Test of miscGetUtcTimeStr() */
    printf("miscGetUtcTimeStr() Function Test :\n\n");
    for (i=0; i<7; ++i)
    {
        printf("   UTC Time     (precision s=%d)  = ", i);

        if (miscGetUtcTimeStr(time, i) == FAILURE)
        {
            printf("FAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("%s\n", time);
        }
    }
    printf("\n\n");

    /* Test of miscGetLocalTimeStr() */
    printf("miscGetLocalTimeStr() Function Test :\n\n");
    for (i=6; i>=0; --i)
    {
        printf("   Local Time   (precision s=%d)  = ", i);

        if (miscGetLocalTimeStr(time, i) == FAILURE)
        {
            printf("FAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("%s\n", time);
        }
    }
    printf("\n\n");

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

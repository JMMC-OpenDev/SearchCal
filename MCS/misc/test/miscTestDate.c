/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDate.c,v 1.2 2004-07-22 15:29:15 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  22-Jul-2004  Correted some typos, code factorization, and error
*                        management
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDate.c,v 1.2 2004-07-22 15:29:15 lafrasse Exp $"; 
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
#include "misc.h"


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
            errDisplayStack();
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
            errDisplayStack();
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

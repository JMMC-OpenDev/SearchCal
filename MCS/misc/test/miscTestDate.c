/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestDate.c,v 1.7 2005-03-03 16:10:59 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  23-Jun-2004  Forked from miscTestUtils.c
 * lafrasse  22-Jul-2004  Correted some typos, code factorization, and error
 *                        management
 * lafrasse  02-Aug-2004  Changed local includes to use miscDate headers
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDate.c,v 1.7 2005-03-03 16:10:59 gluck Exp $"; 
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

        if (miscGetUtcTimeStr(i, time) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
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

        if (miscGetLocalTimeStr(i, time) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
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

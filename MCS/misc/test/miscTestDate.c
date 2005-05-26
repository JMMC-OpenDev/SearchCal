/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestDate.c,v 1.8 2005-05-26 09:53:31 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/03/03 16:10:59  gluck
 * Update due to code review
 *
 * Revision 1.6  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  02-Aug-2004  Changed local includes to use miscDate headers
 * lafrasse  22-Jul-2004  Correted some typos, code factorization, and error
 *                        management
 * lafrasse  23-Jun-2004  Forked from miscTestUtils.c
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDate.c,v 1.8 2005-05-26 09:53:31 lafrasse Exp $"; 
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
    
    mcsSTRING32 timeStr;
    mcsUINT32   precision;

    /* Test of miscGetUtcTimeStr() */
    printf("miscGetUtcTimeStr() Function Test :\n\n");
    for (precision = 0; precision < 7; precision++)
    {
        printf("   UTC Time    (precision s=%u)  =  ", precision);

        if (miscGetUtcTimeStr(precision, timeStr) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("'%s'\n", timeStr);
        }
    }
    printf("\n\n");

    /* Test of miscGetLocalTimeStr() */
    printf("miscGetLocalTimeStr() Function Test :\n\n");
    for (precision = 0; precision < 7; precision++)
    {
        printf("   Local Time  (precision s=%u)  =  ", precision);

        if (miscGetLocalTimeStr(precision, timeStr) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("'%s'\n", timeStr);
        }
    }
    printf("\n\n");

    exit(EXIT_SUCCESS);
}

/*___oOo___*/

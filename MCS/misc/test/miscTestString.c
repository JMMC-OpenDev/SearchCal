/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestString.c,v 1.4 2004-08-02 14:08:46 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  23-Jul-2004  Added error management, and miscIsSpaceStr test
* lafrasse  02-Aug-2004  Changed local includes to use miscString headers
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestString.c,v 1.4 2004-08-02 14:08:46 lafrasse Exp $"; 
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
#include "miscString.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    mcsBYTES256	string;

    /* Test of miscStripQuotes() */
    printf("miscStripQuotes() Function Test :\n\n");
    printf("   Original String  = |(null)|\n");
    printf("   Resulting String = ");
    if (miscStripQuotes(NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    strcpy ((char *)string, "   \"   kjkdjd kjkjk   kjkj  \"      ");
    printf("   Original String  = |%s|\n", string);
    printf("   Resulting String = ");
    if (miscStripQuotes(string) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    printf("\n\n");

    /* Test of miscStrToUpper() */
    printf("miscStrToUpper() Function Test :\n\n");
    printf("   Original String  = |(null)|\n");
    printf("   Resulting String = ");
    if (miscStrToUpper(NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n", string);
    printf("   Resulting String = ");
    if (miscStrToUpper(string) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    printf("\n\n");

    /* Test of miscIsSpaceStr() */
    printf("miscIsSpaceStr() Function Test :\n\n");
    strcpy ((char *)string, "*");
    printf("   miscIsSpaceStr(|%s|  ) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " ");
    printf("   miscIsSpaceStr(|%s|  ) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "* ");
    printf("   miscIsSpaceStr(|%s| ) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " *");
    printf("   miscIsSpaceStr(|%s| ) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "  ");
    printf("   miscIsSpaceStr(|%s| ) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "*  ");
    printf("   miscIsSpaceStr(|%s|) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " * ");
    printf("   miscIsSpaceStr(|%s|) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "  *");
    printf("   miscIsSpaceStr(|%s|) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "   ");
    printf("   miscIsSpaceStr(|%s|) = ", string);
    if (miscIsSpaceStr(string) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    printf("\n\n");

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

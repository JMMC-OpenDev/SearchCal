/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestString.c,v 1.7 2005-01-18 22:11:19 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  23-Jul-2004  Added error management, and miscIsSpaceStr test
* lafrasse  02-Aug-2004  Changed local includes to use miscString headers
* gzins     15-Dec-2004  Added test for miscTrimString function
* lafrasse  17-Jan-2005  Added miscSplitString function
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestString.c,v 1.7 2005-01-18 22:11:19 lafrasse Exp $"; 
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

    /* Test of miscTrimString() */
    printf("miscTrimString() Function Test :\n\n");
    strcpy ((char *)string, "   \"   kjkdjd kjkjk   kjkj  \"      ");
    printf("   Original String  = |%s|\n", string);
    printf("   Resulting String = ");
    if (miscTrimString(string, " ") == FAILURE)
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


    /* Test of miscReplaceStrByStr */
    printf("miscReplaceStrByStr() Function Test :\n\n");
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .;/");
    printf("   Original String  = %s\n",string);
    if (miscReplaceChrByChr(string, 'A', 'Z') == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Replace 'A' by 'Z'\n");
        printf("   New String  = %s\n", string);
    }

    if (miscReplaceChrByChr(string, 'Z', 'A') == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Replace 'Z' by 'A'\n");
        printf("   New String  = %s\n", string);
    }
    printf("\n\n");

    /* Test of miscSplitString */
    printf("miscSplitString() Function Test :\n\n");
    strcpy((char *)string, "ABCD E FG HIJ KLMNOPQ R ST UVWXYZ |---10---||---20---||---30---||---40---||---50---||---60---||---70---||---80---||---90---||--100---||--110---||--120---||--130---||--140---||--150---||--160---||--170---||--180---||--190---||--200---||--210---||--220---||--230---||--240---||--250---||--260---|");
    printf("   Original String  = '%s'\n",string);
    mcsSTRING256 subStrings[50];
    mcsUINT32    nbSubString = 0;

    if (miscSplitString(NULL, ' ', NULL, 0, NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', NULL, 0, NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 0, NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 5, NULL) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 5, &nbSubString) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 10, &nbSubString) == FAILURE)
    {
        printf("FAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = '%s'\n", i, subStrings[i]);
        }
    }

    exit (EXIT_SUCCESS);
}


/*___oOo___*/

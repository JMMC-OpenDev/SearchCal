/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestString.c,v 1.13 2005-05-26 08:48:20 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2005/02/25 16:43:52  lafrasse
 * Added miscDeleteChr()
 *
 * Revision 1.11  2005/02/21 15:27:52  lafrasse
 * Added miscIsCommentLine()
 *
 * Revision 1.10  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  17-Jan-2005  Added miscSplitString function
 * gzins     15-Dec-2004  Added test for miscTrimString function
 * lafrasse  02-Aug-2004  Changed local includes to use miscString headers
 * lafrasse  23-Jul-2004  Added error management, and miscIsSpaceStr test
 * lafrasse  23-Jun-2004  Forked from miscTestUtils.c
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: miscTestString.c,v 1.13 2005-05-26 08:48:20 lafrasse Exp $"; 
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
    mcsBYTES256 string;
    mcsSTRING4  pattern;



    /* Test of miscStripQuotes() */
    printf("miscStripQuotes() Function Test :\n\n");
    printf("   Original String  = |(null)|\n");
    printf("   Resulting String = ");
    if (miscStripQuotes(NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    strcpy ((char *)string, "   \"   kjkdjd kjkjk   kjkj  \"      ");
    printf("   Original String  = |%s|\n", string);
    printf("   Resulting String = ");
    if (miscStripQuotes(string) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
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
    if (miscTrimString(string, " ") == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
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
    if (miscStrToUpper(NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("|%s|\n", string);
    }
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n", string);
    printf("   Resulting String = ");
    if (miscStrToUpper(string) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
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



    /* Test of miscIsCommentLine() */
    printf("miscIsCommentLine() Function Test :\n\n");
    strcpy ((char *)string, "    azerty\n    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, "//    azerty\n    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " //   azerty\n    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " \t //   azerty\n    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " \t    azerty//\n    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    strcpy ((char *)string, " \t    azerty\n//    qwerty");
    strcpy ((char *)pattern, "//");
    printf("   miscIsCommentLine(|%s|,|%s|) = ", string, pattern);
    if (miscIsCommentLine(string, pattern) == mcsFALSE)
    {
        printf("FALSE.\n");
    }
    else
    {
        printf("TRUE.\n");
    }
    printf("\n\n");



    /* Test of miscReplaceChrByChr */
    printf("miscReplaceChrByChr() Function Test :\n\n");
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n\n",string);
    if (miscReplaceChrByChr(string, 'A', 'Z') == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Replace 'A' by 'Z'\n");
        printf("   New String       = |%s|\n\n", string);
    }

    if (miscReplaceChrByChr(string, 'Z', 'A') == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Replace 'Z' by 'A'\n");
        printf("   New String       = |%s|\n\n", string);
    }
    printf("\n\n");



    /* Test of miscDeleteChr */
    printf("miscDeleteChr() Function Test :\n\n");
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n",string);
    if (miscDeleteChr(string, '/', mcsFALSE) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Removed first '/'\n");
        printf("   New String       = |%s|\n\n", string);
    }
    strcpy ((char *)string, "Abc deF GhI jKl 012 .; Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n",string);
    if (miscDeleteChr(string, '/', mcsFALSE) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Removed first '/'\n");
        printf("   New String       = |%s|\n\n", string);
    }
    strcpy ((char *)string, "/Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n",string);
    if (miscDeleteChr(string, '/', mcsFALSE) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Removed first '/'\n");
        printf("   New String       = |%s|\n\n", string);
    }

    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .;/");
    printf("   Original String  = |%s|\n",string);
    if (miscDeleteChr(string, ' ', mcsTRUE) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Removed all ' '\n");
        printf("   New String       = |%s|\n\n", string);
    }
    strcpy ((char *)string, " Abc deF GhI jKl 012 .;/ Abc deF GhI jKl 012 .; ");
    printf("   Original String  = |%s|\n",string);
    if (miscDeleteChr(string, ' ', mcsTRUE) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("   Removed all ' '\n");
        printf("   New String       = |%s|\n\n", string);
    }
    printf("\n\n");



    /* Test of miscSplitString */
    printf("miscSplitString() Function Test :\n\n");
    strcpy((char *)string, "ABCD E FG HIJ KLMNOPQ R ST UVWXYZ |---10---||---20---||---30---||---40---||---50---||---60---||---70---||---80---||---90---||--100---||--110---||--120---||--130---||--140---||--150---||--160---||--170---||--180---||--190---||--200---||--210---||--220---||--230---||--240---||--250---||--260---|");
    printf("   Original String  = |%s|\n",string);
    mcsSTRING256 subStrings[50];
    mcsUINT32    nbSubString = 0;

    if (miscSplitString(NULL, ' ', NULL, 0, NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', NULL, 0, NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 0, NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 5, NULL) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 5, &nbSubString) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    if (miscSplitString(string, ' ', subStrings, 10, &nbSubString) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        int i = 0;
        for (i = 0; i<nbSubString; i++)
        {
            printf("   subString[%2d]    = |%s|\n", i, subStrings[i]);
        }
    }

    exit (EXIT_SUCCESS);
}


/*___oOo___*/

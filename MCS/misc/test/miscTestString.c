/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestString.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  forked from miscTestUtils.c
*
********************************************************************************
*   NAME
* 
*   SYNOPSIS
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*-----------------------------------------------------------------------------*/

#define _POSIX_SOURCE 1

static char *rcsId="@(#) $Id: miscTestString.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Local Headers 
 */
#include "mcs.h"
#include "misc.h"

/* 
 * Main
 */

int main (int argc, char *argv[])
{
    mcsBYTES256	string;

    /* Test of miscStripQuotes() */
    printf("miscStripQuotes() Function Test :\n\n");
    strcpy ((char *)string, "   \"   kjkdjd kjkjk   kjkj  \"      \0");
    printf("   Original String  = |%s|\n", string);
    miscStripQuotes(string);
    printf("   Resulting String = |%s|\n", string);
    printf("\n\n");

    /* Test of miscStrToUpper() */
    printf("miscStrToUpper() Function Test :\n\n");
    strcpy ((char *)string, "Abc deF GhI jKl 012 .;/\0");
    printf("   Original String  = |%s|\n", string);
    miscStrToUpper(string);
    printf("   Resulting String = |%s|\n", string);
    printf("\n\n");

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

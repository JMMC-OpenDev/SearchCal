/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestFile.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"
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

static char *rcsId="@(#) $Id: miscTestFile.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"; 
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
    mcsBYTES256 fullFileName;

    /* Test of miscGetFileName() */
    printf("miscGetFileName() Function Test :\n\n");
    printf("   File Path                      | File Name\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    printf("\n\n");

    /* Test of miscGetExtension() */
    printf("miscGetExtension() Function Test :\n\n");
    printf("   File Path                      | File Extension\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    printf("\n\n");

    /* Test of miscYankExtension() */
    printf("miscYankExtension() Function Test :\n\n");
    printf("   File Path                      | Without Extension\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    printf("\n\n");

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestUtils.c,v 1.7 2004-06-18 12:13:23 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
* lafrasse  17-Jun-2004  added miscGetLocalTimeStr test
*                        added miscStripQuotes test
*                        added miscStrToUpper test
* lafrasse  18-Jun-2004  added miscGetExtension test
*                        added miscYankExtension test
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

static char *rcsId="@(#) $Id: miscTestUtils.c,v 1.7 2004-06-18 12:13:23 lafrasse Exp $"; 
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
    mcsBYTES32  utcTime;
    mcsBYTES256	string;

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
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName);
    printf("%s\n", fullFileName);
    printf("\n\n");

    printf("=============================================================\n\n");

    /* Test of miscGetUtcTimeStr() */
    printf("miscGetUtcTimeStr() Function Test :\n\n");
    miscGetUtcTimeStr(utcTime, 0);
    printf("   UTC Time                      = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 1);
    printf("   UTC Time     (precision s=1)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 2);
    printf("   UTC Time     (precision s=2)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 3);
    printf("   UTC Time     (precision s=3)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 4);
    printf("   UTC Time     (precision s=4)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 5);
    printf("   UTC Time     (precision s=5)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 6);
    printf("   UTC Time     (precision s=5)  = %s\n", utcTime);
    printf("\n\n");

    /* Test of miscGetLocalTimeStr() */
    printf("miscGetLocalTimeStr() Function Test :\n\n");
    miscGetLocalTimeStr(utcTime, 0);
    printf("   Local Time                    = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 1);
    printf("   Local Time   (precision s=1)  = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 2);
    printf("   Local Time   (precision s=2)  = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 3);
    printf("   Local Time   (precision s=3)  = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 4);
    printf("   Local Time   (precision s=4)  = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 5);
    printf("   Local Time   (precision s=5)  = %s\n", utcTime);
    miscGetLocalTimeStr(utcTime, 6);
    printf("   Local Time   (precision s=5)  = %s\n", utcTime);
    printf("\n\n");

    printf("=============================================================\n\n");

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

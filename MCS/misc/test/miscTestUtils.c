/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestUtils.c,v 1.1 2004-06-17 08:27:58 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
*
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

static char *rcsId="@(#) $Id: miscTestUtils.c,v 1.1 2004-06-17 08:27:58 gzins Exp $"; 
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

    /* Test of miscGetFileName() */
    printf("Test de la fonction miscGetFileName() :\n\n");
    printf("   Chemin d'acces au fichier      | Nom du fichier\n");
    printf("   -------------------------------+---------------\n");
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

    /* Test of miscGetUtcTimeStr() */
    printf("Test de la fonction miscGetUtcTimeStr() :\n\n");
    miscGetUtcTimeStr(utcTime, 0);
    printf("   Heure locale                   = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 1);
    printf("   Heure locale (precision s=1)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 2);
    printf("   Heure locale (precision s=2)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 3);
    printf("   Heure locale (precision s=3)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 4);
    printf("   Heure locale (precision s=4)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 5);
    printf("   Heure locale (precision s=5)  = %s\n", utcTime);
    miscGetUtcTimeStr(utcTime, 6);
    printf("   Heure locale (precision s=5)  = %s\n", utcTime);
    printf("\n\n");

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

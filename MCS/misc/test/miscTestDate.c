/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDate.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"
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

static char *rcsId="@(#) $Id: miscTestDate.c,v 1.1 2004-06-23 09:05:46 lafrasse Exp $"; 
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
    mcsBYTES32  utcTime;

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

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestNetwork.c,v 1.1 2004-08-03 09:09:04 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  03-Aug-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestNetwork.c,v 1.1 2004-08-03 09:09:04 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <stdlib.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "miscNetwork.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);

    mcsBYTES256  string;
    mcsUINT32    length = 256;

    /* Test of miscGetHostName() */
    printf("miscGetHostName() Function Test :\n\n");
    printf("miscGetHostName(NULL, 0)  = ");
    if (miscGetHostName(NULL, 0) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }

    printf("miscGetHostName(string, 0)  = ");
    if (miscGetHostName(string, 0) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }

    printf("miscGetHostName(string, length)  = ");
    if (miscGetHostName(string, length) == FAILURE)
    {
        printf("FAILURE.\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    printf("\n\n");

    mcsExit();
    exit (EXIT_SUCCESS);
}

/*___oOo___*/

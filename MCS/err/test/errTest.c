/*******************************************************************************
* JMMC project
*
* "@(#) $Id: errTest.c,v 1.1 2004-06-23 13:07:03 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     17-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errTest.c,v 1.1 2004-06-23 13:07:03 gzins Exp $"; 
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
#include "log.h"
#include "err.h"
#include "errPrivate.h"

/* 
 * Main
 */
int main (int argc, char *argv[])
{
    int i;

    char     buffer[2048];

    logSetVerboseLevel(logEXTDBG);
    for (i = 1; i < 25; i++)
    {
        errAdd(i,i);
    }

    printf ("Error 10 in stack ? : %s\n",
        errIsInStack("err", 10)==mcsTRUE?"Oui":"Non");
    printf ("Error 24 in stack ? : %s\n",
        errIsInStack("err", 24)==mcsTRUE?"Oui":"Non");

    printf ("\nDisplay error stack\n");
    errDisplayStack();

    printf ("\nSave/restore error stack\n");
    errPackStack(buffer, sizeof(buffer));
    errCloseStack();
    errUnpackStack(buffer, strlen(buffer));
    errDisplayStack();

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: errTest.c,v 1.2 2004-06-23 16:57:26 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     17-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errTest.c,v 1.2 2004-06-23 16:57:26 gzins Exp $"; 
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

    for (i = 1; i <= 10; i++)
    {
        errAdd(i,i);
    }

    printf ("Error 10 in stack ? : %s\n",
            errIsInStack("err", 10)==mcsTRUE?"Oui":"Non");
    printf ("Error 12 in stack ? : %s\n",
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

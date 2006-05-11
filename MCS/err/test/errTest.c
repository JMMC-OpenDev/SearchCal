/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: errTest.c,v 1.5 2006-05-11 13:04:16 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/02/15 08:29:59  gzins
 * Added CVS log as file modification history
 *
 * gzins     17-Jun-2004  created
 *
 ******************************************************************************/

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: errTest.c,v 1.5 2006-05-11 13:04:16 mella Exp $";
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
#include "errErrors.h"

/* 
 * Main
 */
int main (int argc, char *argv[])
{
    char     buffer[2048];

    printf ("Adding errors in stack...\n");
    errAdd(errERR_NUM_1, 1234);
    errUserAdd(errERR_USR_MSG);
    errAdd(errERR_NUM_2, 9876);

    printf ("Error 1 in stack ? : %s\n",
            errIsInStack("err", errERR_NUM_2)==mcsTRUE?"Oui":"Non");
    printf ("Error 24 in stack ? : %s\n",
            errIsInStack("err", 24)==mcsTRUE?"Oui":"Non");

    printf ("\nGetting user message ...\n");
    printf ("User message = %s\n", errUserGet()); 

    printf ("\nDisplaying error stack...\n");
    errDisplayStack();

    printf ("\nSaving/restoring error stack...\n");
    errPackStack(buffer, sizeof(buffer));
    errCloseStack();
    errUnpackStack(buffer, strlen(buffer));
    errDisplayStack();

    exit (EXIT_SUCCESS);
}

/*___oOo___*/

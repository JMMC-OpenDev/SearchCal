/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: mcsTest.c,v 1.3 2005-02-15 12:37:36 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/13 16:43:42  gzins
 * Added CVS log as modification history
 *
 * lafrasse  01-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Simple main calling basic mcs functions.
 * 
 */

static char *rcsId="@(#) $Id: mcsTest.c,v 1.3 2005-02-15 12:37:36 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>


/*
 * MCS Headers 
 */


/*
 * Local Headers 
 */
#include "mcs.h"
#include "mcsPrivate.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    printf("Processus   Name is '%s'.\n", mcsGetProcName());
    printf("Environment Name is '%s'.\n", mcsGetEnvName());

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with mcsSUCCESS */
    exit (EXIT_SUCCESS);
}

/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: envShow.cpp,v 1.8 2005-12-06 07:13:55 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/12/02 13:44:05  gzins
 * Changed location of mcsEnvList file; located in MCSTOP/etc
 *
 * Revision 1.6  2005/02/28 14:25:00  lafrasse
 * Reversed changelog order
 *
 * Revision 1.5  2005/02/13 17:26:51  gzins
 * Minor changes in documentation
 *
 * Revision 1.4  2005/02/13 16:53:13  gzins
 * Added CVS log as modification history
 *
 * lafrasse  08-Dec-2004  Comment refinments
 * lafrasse  07-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Utility showing the list of environments defined in the mcscfgEnvList file,
 * and to verify that there is no duplicated environment, or several
 * environments using same port on the same workstation. 
 *
 * \synopsis
 * \<envEnvShow\>
 *
 * \usedfiles
 * \filename mcscfgEnvList :  MCS environment definition file, located in
 * $MCSDATA
 */

static char *rcsId="@(#) $Id: envShow.cpp,v 1.8 2005-12-06 07:13:55 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "env.h"
#include "envPrivate.h"

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    envLIST list;
    list.Show();

    // Show errors (if any)
    errDisplayStack();

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

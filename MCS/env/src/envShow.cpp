/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envShow.cpp,v 1.2 2004-12-08 14:39:59 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  07-Dec-2004  Created
* lafrasse  08-Dec-2004  Comment refinments
*
*
*******************************************************************************/

/**
 * \file
 * Small tool that allow you to show the content of the mcsEnvList file, and to
 * verify weither it is well structured or not.
 *
 * \synopsis
 * \<envEnvShow\>
 *
 * \usedfiles
 * \filename mcsEnvList :  MCS environment List file, stored in $MCSROOT/etc
 * 
 */

static char *rcsId="@(#) $Id: envShow.cpp,v 1.2 2004-12-08 14:39:59 lafrasse Exp $"; 
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
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    envLIST list;
    list.Show();

    errDisplayStack();

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envTest.cpp,v 1.1 2004-12-07 16:45:56 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  07-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Test tool for the envLIST object.
 *
 * \synopsis
 * \<envTest\>
 * 
 */

static char *rcsId="@(#) $Id: envTest.cpp,v 1.1 2004-12-07 16:45:56 lafrasse Exp $"; 
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
    cout << "Using $MCSENV environment name :" << endl;
    string  hostName   = list.GetHostName();
    int     portNumber = list.GetPortNumber();
    cout << "Host Name ='" << hostName << "', "
         << "Port Number = '" << portNumber << "'." << endl << endl;

    errDisplayStack();

    cout << "Using 'remote' environment name :" << endl;
    hostName   = list.GetHostName("remote");
    portNumber = list.GetPortNumber("remote");
    cout << "Host Name ='" << hostName << "', "
         << "Port Number = '" << portNumber << "'." << endl;

    errDisplayStack();

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

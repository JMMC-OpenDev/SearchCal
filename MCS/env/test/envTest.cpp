/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: envTest.cpp,v 1.6 2005-02-28 14:25:00 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/28 14:19:25  lafrasse
 * Moved the MCS 'default' environment definition from hard-coded values to the 'mcsEnvList' file
 *
 * Revision 1.4  2005/02/13 17:26:51  gzins
 * Minor changes in documentation
 *
 * Revision 1.3  2005/02/13 16:53:13  gzins
 * Added CVS log as modification history
 *
 * lafrasse  08-Dec-2004  Refined the output format
 * lafrasse  07-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test tool for the envLIST object.
 *
 * \synopsis
 * \<envTest\>
 * 
 */

static char *rcsId="@(#) $Id: envTest.cpp,v 1.6 2005-02-28 14:25:00 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <iomanip>

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
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }

    envLIST list;
    char* envName;

    cout << "+--------------------+--------------------+-------------+" << endl
         << "|   ENVIRONMENT NAME |          HOST NAME | PORT NUMBER |" << endl
         << "+--------------------+--------------------+-------------+" << endl;

    envName = NULL;
    cout << "| " << setw(18) << "NULL"                      << " "
         << "| " << setw(18) << list.GetHostName(envName)   << " "
         << "| " << setw(11) << list.GetPortNumber(envName) << " |"     << endl;

    envName = "default";
    cout << "| " << setw(18) << envName                     << " "
         << "| " << setw(18) << list.GetHostName(envName)   << " "
         << "| " << setw(11) << list.GetPortNumber(envName) << " |"     << endl;

    cout << "+--------------------+--------------------+-------------+" << endl;

    errDisplayStack();

    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

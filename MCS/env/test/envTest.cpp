/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test tool for the envLIST object.
 *
 * \synopsis
 * \<envTest\>
 * 
 */


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

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdTEST.cpp,v 1.1 2004-12-05 18:57:21 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     16-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 *  Simple test file for cmdCOMMAND class
 */

static char *rcsId="@(#) $Id: cmdTEST.cpp,v 1.1 2004-12-05 18:57:21 gzins Exp $"; 
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
#include "cmd.h"
#include "cmdCOMMAND.h"
#include "cmdPARAM.h"
#include "cmdPrivate.h"


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
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logEXTDBG);
    string mnemo, params; 

    if(argc == 1)
    {
        mnemo.assign("VALID");
        params.assign("-integer 1 -double 0.1 -boolean true -string \"str with blank and --\"");
    }
    if(argc == 2)
    {
        mnemo.assign("VALID");
        params.assign(argv[1]);
    }
    
    if(argc == 3)
    {
        mnemo.assign(argv[1]);
        params.assign(argv[2]);
    }
    
    // printf things to parse
    cout<<"MNEMO = " << mnemo <<endl;
    cout<<"PARAMS = " << params <<endl;

    cmdCOMMAND myCmd(mnemo, params);

    if (argc != 3)
    {
        // we have parsed a VALID command
        // get variables from mandatory parameters
        
        string pName("integer");
        mcsINT32 v1;
        if(myCmd.getParamValue(pName,&v1)==FAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v1 << endl;
        }

        pName.assign("double");
        mcsDOUBLE v2;
        if(myCmd.getParamValue(pName,&v2)==FAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v2 << endl;
        }
        pName.assign("boolean");
        mcsLOGICAL v3;
        if(myCmd.getParamValue(pName,&v3)==FAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << (int)v3 << endl;
        }
        pName.assign("string");
        char * v4;
        if(myCmd.getParamValue(pName,&v4)==FAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v4 << endl;
        }
    }
    
    // Print help for myCmd
    string help;
    help = myCmd.getHelp();
    cout<<help<<endl;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

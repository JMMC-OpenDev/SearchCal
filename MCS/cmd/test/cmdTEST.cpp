/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdTEST.cpp,v 1.7 2005-02-15 11:02:48 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/15 10:58:58  gzins
 * Added CVS log as file modification history
 *
 * mella     16-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 *  Simple test file for cmdCOMMAND class
 */

static char *rcsId="@(#) $Id: cmdTEST.cpp,v 1.7 2005-02-15 11:02:48 gzins Exp $"; 
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
#include "cmdVALID_CMD.h"
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
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with mcsFAILURE
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

    cmdVALID_CMD myCmd(mnemo, params);

    if (argc != 3)
    {
        // we have parsed a VALID command
        // get variables from mandatory parameters
        
        string pName("dinteger");
        mcsINT32 v1;
        if(myCmd.GetParamValue(pName,&v1)==mcsFAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
            errCloseStack();
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v1 << endl;
        }

        pName.assign("double");
        mcsDOUBLE v2;
        if(myCmd.GetParamValue(pName,&v2)==mcsFAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
            errCloseStack();
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v2 << endl;
        }
        pName.assign("boolean");
        mcsLOGICAL v3;
        if(myCmd.GetParamValue(pName,&v3)==mcsFAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
            errCloseStack();
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << (int)v3 << endl;
        }
        pName.assign("string");
        char * v4;
        if(myCmd.GetParamValue(pName,&v4)==mcsFAILURE) {
            cout << "Can't get user value into myCmd for parameter " << pName << endl;
            errCloseStack();
        } else {
            cout << pName << " parameter from myCmd gets the next user value: " << v4 << endl;
        }
    }
    
    // Print help for myCmd
    string help;
    myCmd.GetDescription(help);
    cout<<"Description:" <<endl;
    cout<<help<<endl;

    // Print short description
    myCmd.GetShortDescription(help);
    cout<<"Short Description:" <<endl;
    cout<<help<<endl;
   
    // Print get dstring value
    char *dstringValue;
     if(myCmd.GetParamValue("string",&dstringValue)==mcsFAILURE) {
            cout << "Can't get user value into myCmd for parameter dstring" << endl;
            errCloseStack();
        } else {
            cout << "dstring parameter from myCmd gets the next user value: " << dstringValue << endl;
        }
    
    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

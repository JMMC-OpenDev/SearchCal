/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdTEST.cpp,v 1.11 2006-01-04 12:33:01 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/02/27 09:28:37  gzins
 * Updated to test error handling
 *
 * Revision 1.9  2005/02/22 12:39:11  mella
 * Place main code into a new lock {} to perform a better c++ housekeeping
 *
 * Revision 1.8  2005/02/17 18:00:10  gzins
 * Included test for GetCmdParamLine()
 *
 * Revision 1.7  2005/02/15 11:02:48  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
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

static char *rcsId="@(#) $Id: cmdTEST.cpp,v 1.11 2006-01-04 12:33:01 mella Exp $"; 
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
    // insert a new block to be sure c++ does a good housekeeping
    {
        // Initialize MCS services
        if (mcsInit(argv[0]) == mcsFAILURE)
        {
            // Error handling if necessary

            // Exit from the application with mcsFAILURE
            exit (EXIT_FAILURE);
        }

        logSetStdoutLogLevel(logTEST);
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

        // Print help for myCmd
        string help;
        if (myCmd.GetDescription(help) == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        };
        cout<<"Description:" <<endl;
        cout<<help<<endl;

        // Print short description
        if (myCmd.GetShortDescription(help) == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        };
        cout<<"Short Description:" <<endl;
        cout<<help<<endl;

        if (argc != 3)
        {
            // we have parsed a VALID command
            // get variables from mandatory parameters

            string pName("dinteger");
            mcsINT32 v1;
            if (myCmd.GetParamValue(pName, &v1) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            }
            else
            {
                cout << pName << " parameter from myCmd gets the next user value: " << v1 << endl;
            }

            pName.assign("double");
            mcsDOUBLE v2;
            if (myCmd.GetParamValue(pName, &v2) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            }
            else
            {
                cout << pName << " parameter from myCmd gets the next user value: " << v2 << endl;
            }
            pName.assign("boolean");
            mcsLOGICAL v3;
            if (myCmd.GetParamValue(pName, &v3) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            }
            else
            {
                cout << pName << " parameter from myCmd gets the next user value: " << (int)v3 << endl;
            }
            pName.assign("string");
            char * v4;
            if (myCmd.GetParamValue(pName, &v4) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            } 
            else
            {
                cout << pName << " parameter from myCmd gets the next user value: " << v4 << endl;
            }
        }

        // test to get string value of a parameter with default value
        char *dstringValue;
        if (myCmd.GetParamValue("dstring", &dstringValue) == mcsFAILURE) 
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
        else
        {
            cout << "dstring parameter from myCmd gets the next value: " << dstringValue << endl;
        }

        // test to get a double value of a parameter with default value
        mcsDOUBLE ddoubleValue;
        if (myCmd.GetParamValue("ddouble", &ddoubleValue) == mcsFAILURE) 
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
        else
        {
            cout << "ddouble parameter from myCmd gets the next value: " << ddoubleValue << endl;
        }

        // test to get a double value of a parameter with default value
        mcsINT32 dintegerValue;
        if (myCmd.GetParamValue("dinteger", &dintegerValue) == mcsFAILURE) 
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
        else
        {
            cout << "dinteger parameter from myCmd gets the next value: " << dintegerValue << endl;
        }

         // test to get a double value of a parameter with default value
        mcsLOGICAL dbooleanValue;
        if (myCmd.GetParamValue("dboolean", &dbooleanValue) == mcsFAILURE) 
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
        else
        {
            cout << "dboolean parameter from myCmd gets the next value: " << dbooleanValue << endl;
        }

 
        
        

        string cmdParamline;
        if (myCmd.GetCmdParamLine(cmdParamline) == mcsFAILURE) 
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
        else
        {
            cout << "command parameter line: " << cmdParamline << endl;
        }

    }
    // Close MCS services
    mcsExit();

    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTestTask.cpp,v 1.1 2004-12-05 19:03:25 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     10-Jun-2004  created
*
*
********************************************************************************
*   NAME
* 
*   SYNOPSIS
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*-----------------------------------------------------------------------------*/
#define _POSIX_SOURCE 1

static char *rcsId="@(#) $Id: evhTestTask.cpp,v 1.1 2004-12-05 19:03:25 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#define MODULE_ID "myTask"
#include "mcs.h"
#include "log.h"
#include "evh.h"

class myTASK : public evhTASK
{
public:
    myTASK() {};
    virtual ~myTASK() {};
    virtual mcsCOMPL_STAT PrintAppOptions();
    virtual mcsCOMPL_STAT ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
                                          mcsINT32 *optind, 
                                          mcsLOGICAL *optUsed);
private:
    mcsLOGICAL  noTimeout;
    mcsBYTES256 configFileName;
};

mcsCOMPL_STAT myTASK::PrintAppOptions()
{
    cout <<" Other options:    -noTimeout   disable waiting for a reply on "
        "a CCS message" << endl;
    cout <<"                   -c <file>    specify application "
        "configuration file" << endl;
    return SUCCESS;

}

mcsCOMPL_STAT myTASK::ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
                                      mcsINT32 *optind, mcsLOGICAL *optUsed)
{
    logExtDbg ("myTASK::ParseAppOptions ()");
    // No timeout option
    if(strcmp(argv[*optind], "-noTimeout") == 0)
    {
        noTimeout = mcsTRUE;
        return SUCCESS;
    }
    // Application configuration file option
    else if (strcmp(argv[*optind], "-c") == 0)
    {
        if ((*optind + 1) < argc)
        {
            *optind += 1;
            optarg = argv[*optind];
            if ( sscanf (optarg, "%s", configFileName) != 1)
            {
                logWarning ("%s: Argument to option %s is invalid: '%s'",
                            Name(), argv[*optind-1], optarg);
                return FAILURE;
            }
            return SUCCESS;
        }
        else
        {
            logWarning ("%s: Option %s requires an argument",
                        Name(), argv[*optind]);
            return FAILURE;
        }
    }

    // This option has not been processed. 
    *optUsed = mcsFALSE; 
    
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    // Create objects living in the application
    myTASK myTask;
    
    logInfo("Program starting ...");
    // Parse input parameter
    if (myTask.Init(argc, argv) == FAILURE)
    {
        exit (EXIT_FAILURE);
    }

    // Specific application code

    logInfo("Program exiting ...");
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

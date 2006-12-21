/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCLI.cpp,v 1.1 2006-12-21 15:13:48 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * brief description of the program, which ends at this dot.
 *
 * @synopsis
 * \<Command Name\> [\e \<param1\> ... \e \<paramN\>] 
 *                     [\e \<option1\> ... \e \<optionN\>] 
 *
 * @param param1 : description of parameter 1, if it exists
 * @param paramN : description of parameter N, if it exists
 *
 * @opt
 * @optname option1 : description of option 1, if it exists
 * @optname optionN : description of option N, if it exists
 * 
 * @details
 * OPTIONAL detailed description of the c main file follows here.
 * 
 * @usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * @filename fileName1 :  usage description of fileName1
 * @filename fileName2 :  usage description of fileName2
 *
 * @env
 * OPTIONAL. If needed, environmental variables accessed by the program. For
 * each variable, name, and usage description, as below.
 * @envvar envVar1 :  usage description of envVar1
 * @envvar envVar2 :  usage description of envVar2
 * 
 * @warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * @ex
 * OPTIONAL. Command example if needed
 * \n Brief example description.
 * @code
 * Insert your command example here
 * @endcode
 *
 * @sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * @sa \<entity to refer\>
 * 
 * @bug OPTIONAL. Known bugs list if it exists.
 * @bug Bug 1 : bug 1 description
 *
 * @todo OPTIONAL. Things to forsee list, if needed. 
 * @todo Action 1 : action 1 description
 * 
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclsvrCLI.cpp,v 1.1 2006-12-21 15:13:48 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * @namespace std
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
#include "sclsvr.h"
#include "sclsvrPrivate.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */




/* 
 * Local functions  
 */

/**
 * Structure used to pass two values as one paramter to a thrdTHREAD function.
 */
typedef struct
{
    char* query;

    miscoDYN_BUF* dynBuf;

    sclsvrSERVER* server;

} sclwsGetVOTableThreadParams;

/**
 * Monitor any action and forward it to the shell.
 *
 * @param param a pointer on any data needed by the fuction.
 *
 * @return always NULL.
 */
thrdFCT_RET sclwsGetVOTableThreadFunction(thrdFCT_ARG param)
{   
    printf("getVOTable()\n");

    sclwsGetVOTableThreadParams* paramsPtr = (sclwsGetVOTableThreadParams*)param;
    char* query  = (char*) paramsPtr->query;
    miscoDYN_BUF* dynBuf = (miscoDYN_BUF*) paramsPtr->dynBuf;
    sclsvrSERVER* server = (sclsvrSERVER*) paramsPtr->server;

    server->GetCal(query, *dynBuf, NULL);

    return NULL;
}


/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    //logSetStdoutLogLevel(logDEBUG);
    logDisableStdoutLog();

    sclsvrSERVER server;
    miscoDYN_BUF dynBuf;
    // Fetch the resulting voTable
    // Thread parameters creation
    sclwsGetVOTableThreadParams     getVOTableThreadParams;
    getVOTableThreadParams.query  = argv[1];
    getVOTableThreadParams.dynBuf = &dynBuf;
    getVOTableThreadParams.server = &server;
    // Thread creation and launch
    thrdTHREAD_STRUCT                getVOTableThread;
    getVOTableThread.function      = sclwsGetVOTableThreadFunction;
    getVOTableThread.parameter     = (thrdFCT_ARG*)&getVOTableThreadParams;
    // Launch the thread
    if (thrdThreadCreate(&getVOTableThread) == mcsFAILURE)
    {
        cout << "Thread creation failure !" << endl;
        return mcsFAILURE;
    }
    cout << "Thread creation succeed." << endl;

    bool   lastMessage = false;
    mcsSTRING256 buffer;
    // Get any new action and forward it to the GUI ...
    do
    {
        server.GetCalStatus((char*)buffer, &lastMessage);
        cout << "buffer = '" << buffer << "'" << endl;
    }
    while (lastMessage == mcsFALSE); // ... until the last action occured

    // Wait for the thread end
    if (thrdThreadWait(&getVOTableThread) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    uint resultSize;
    getVOTableThreadParams.dynBuf->GetNbStoredBytes(&resultSize);
    cout << "Resulting VOTable (size = '" << resultSize << "') :" << endl << getVOTableThreadParams.dynBuf->GetBuffer() << endl;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

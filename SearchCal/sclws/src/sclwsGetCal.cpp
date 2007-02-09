/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsGetCal.cpp,v 1.3 2007-02-09 17:07:46 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2007/02/04 20:56:45  lafrasse
 * Updated webservice URL port number.
 * Updated according to APIs changes in sclsvr.
 *
 * Revision 1.1  2006/12/22 15:17:50  lafrasse
 * Creation
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsGetCal.cpp,v 1.3 2007-02-09 17:07:46 lafrasse Exp $"; 

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
#include "thrd.h"


/*
 * Local Headers 
 */
#include "soapH.h"
#include "soap.nsmap"
#include "sclwsPrivate.h"
#include "sclwsErrors.h"


/*
 * Local Variables
 */
struct Namespace *namespaces;
char* wsURL = "http://jmmc.fr:8078";
 
typedef struct
{
    char* taskID;        /** GETCAL WebService communication ID */

    char* query;         /** the query */

    std::string result;  /** The VO Table */

} sclwsGetVOTableThreadParams;


/*
 * Local Functions
 */
/**
 * Monitor any action and forward it to the shell.
 *
 * @param param a pointer on any data needed by the fuction.
 *
 * @return always NULL.
 */
thrdFCT_RET sclwsGetVOTableThreadFunction(thrdFCT_ARG param)
{   
    // Define a new SOAP context for the thread (can run in parallel of main())
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Retrieve parameters
    sclwsGetVOTableThreadParams* paramPtr = (sclwsGetVOTableThreadParams*)param;
    char*  taskID      = paramPtr->taskID;
    char*  query       = paramPtr->query;
    char*  result      = NULL;

    // Launch the GETCAL query
    soap_call_ns__GetCalAsyncQuery(&v_soap, wsURL, "", taskID, query, &result);
    if (v_soap.error)
    {
        soap_print_fault(&v_soap, stderr);
    }

    logDebug("Received '%d' bytes.", strlen(result));

    // Give back the received VO Table
    paramPtr->result = result;

    soap_end(&v_soap);
    return NULL;
}


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
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout log level 
    logSetStdoutLogLevel(logINFO);

    // Define a SOAP context for main()
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Fetch the a CGETCAL WebService communication ID
    char* taskID;
    soap_call_ns__GetCalAsyncID(&v_soap, wsURL, "", &taskID);
    if (v_soap.error)
    {
        soap_print_fault(&v_soap, stderr);
        exit (EXIT_FAILURE);
    }
    logInfo("Connection to '%s' established (ID = '%s').", wsURL, taskID);

    // Launch the thread that handle the GETCAL query
    // Thread parameters creation
    sclwsGetVOTableThreadParams      getVOTableThreadParams;
    getVOTableThreadParams.taskID  = taskID;
    getVOTableThreadParams.query   = argv[1];
    // Thread creation and launch
    thrdTHREAD_STRUCT                getVOTableThread;
    getVOTableThread.function      = sclwsGetVOTableThreadFunction;
    getVOTableThread.parameter     = (thrdFCT_ARG*)&getVOTableThreadParams;
    // Launch the thread
    if (thrdThreadCreate(&getVOTableThread) == mcsFAILURE)
    {
        errAdd(sclwsERR_THREAD_CREATION);
        exit (EXIT_FAILURE);
    }

    // Fetch each queried catalog name to monitor query progress
    char* catalogName  = NULL;
    int   catalogIndex = 0;
    int   nbOfCatalogs = 0;
    bool  lastCatalog  = false;
    do
    {
        // Ask and wait for the currently quereid catalog name
        logDebug("Waiting for the Catalog Name.");
        soap_call_ns__GetCalWaitForCurrentCatalogName(&v_soap, wsURL, "", taskID, &catalogName);
        if (v_soap.error)
        {
            soap_print_fault(&v_soap, stderr);
            exit (EXIT_FAILURE);
        }
        // Ask for the currently quereid catalog index
        logDebug("Getting the Catalog Index.");
        soap_call_ns__GetCalCurrentCatalogIndex(&v_soap, wsURL, "", taskID, &catalogIndex);
        if (v_soap.error)
        {
            soap_print_fault(&v_soap, stderr);
            exit (EXIT_FAILURE);
        }
        // Ask for the total number of catalogs
        logDebug("Getting the Number of Catalogs.");
        soap_call_ns__GetCalNbOfCatalogs(&v_soap, wsURL, "", taskID, &nbOfCatalogs);
        if (v_soap.error)
        {
            soap_print_fault(&v_soap, stderr);
            exit (EXIT_FAILURE);
        }
        // Ask for the last catalog flag
        logDebug("Getting the Last Catalog Flag.");
        soap_call_ns__GetCalIsLastCatalog(&v_soap, wsURL, "", taskID, &lastCatalog);
        if (v_soap.error)
        {
            soap_print_fault(&v_soap, stderr);
            exit (EXIT_FAILURE);
        }
        logInfo("Querying '%s' - %d/%d (%d).", catalogName, catalogIndex, nbOfCatalogs, lastCatalog);
    }
    while(lastCatalog  == false); // Until last catalog is reached

    // Wait for the thread end (eg. result receiving)
    logDebug("Waiting for thread termination");
    if (thrdThreadWait(&getVOTableThread) == mcsFAILURE)
    {
        errAdd(sclwsERR_THREAD_CREATION);
        errCloseStack();
        exit (EXIT_FAILURE);
    }
    logDebug("Thread terminated");

    if (getVOTableThreadParams.result.length() == 0)
    {
        logWarning("Received nothing.");
    }
    else
    {
        // Output the received VO Table on stdout
        cout << getVOTableThreadParams.result << endl;
    }

    // Dealloc SOAP context
    logDebug("Disconnecting from '%s' (ID = '%s').", wsURL, taskID);
    soap_end(&v_soap);
    logInfo("Connection closed.");

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

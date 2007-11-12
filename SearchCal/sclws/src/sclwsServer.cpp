/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsServer.cpp,v 1.8 2007-11-12 10:32:15 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2007/07/03 17:07:58  lafrasse
 * Corrected connection port number.
 *
 * Revision 1.6  2007/07/03 17:00:03  lafrasse
 * Added support for query cancellation.
 *
 * Revision 1.5  2007/07/02 13:58:47  lafrasse
 * Added signal and error handling, plus proper exit.
 *
 * Revision 1.4  2007/02/16 09:54:55  lafrasse
 * Refined log management.
 *
 * Revision 1.3  2007/02/09 17:07:46  lafrasse
 * Enhanced log and error monitoring.
 * Corrected a bug inherent to early deallocation of sclsvrSERVER _progress
 * leading to a crash on empty resulting queries.
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsServer.cpp,v 1.8 2007-11-12 10:32:15 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>


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
#include "evh.h"


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
uint   portNumber = 8078;
struct soap  v_soap;    // SOAP execution context


/*
 * Local Functions
 */
void* sclwsJobHandler(void* p_tsoap)
{
    struct soap* v_soap = (struct soap*) p_tsoap;

    pthread_detach(pthread_self());

    // Fulfill the received remote call
    soap_serve(v_soap);

    soap_destroy(v_soap); // Dealloc C++ data
    soap_end(v_soap);     // Dealloc data and cleanup
    soap_done(v_soap);    // Detach soap struct

    free(v_soap);

    pthread_exit(NULL);

    return NULL;
}

void sclwsExit(int returnCode)
{
    // Dealloc SOAP context
    logInfo("Cleaning gSOAP ...");
    soap_done(&v_soap);

    // Close MCS services
    logInfo("Cleaning MCS ...");
    mcsExit();
    
    // Exit from the application with SUCCESS
    printf("Exiting now.\n");
    exit (returnCode);
}


/* 
 * Signal catching functions  
 */
void sclwsSignalHandler (int signalNumber)
{
    logInfo("Received a '%d' system signal ...", signalNumber);

    if (signalNumber == SIGPIPE)
    {
        return;
    }

    sclwsExit(EXIT_SUCCESS);
}


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    /* Init system signal trapping */
    if (signal(SIGINT, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGINT, ...) function error");
        sclwsExit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        sclwsExit(EXIT_FAILURE);
    }
    if (signal(SIGPIPE, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        sclwsExit(EXIT_FAILURE);
    }

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        sclwsExit(EXIT_FAILURE);
    }

    // Used to handle common comand-line options (log levels, ...)
    evhTASK cliHandler;
    if (cliHandler.Init(argc, argv) == mcsFAILURE)
    {
        exit (EXIT_FAILURE);
    }

    // @TODO : manage a "garbage collector" to close pending connections

    logInfo("Initializing gSOAP ...", portNumber);

    struct soap *v_tsoap;   // SOAP execution context fork for the threads
    pthread_t v_tid;        // Thread ID

    // SOAP Initialization
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Main SOAP socket creation
    if (soap_bind(&v_soap, NULL, portNumber, 100) < 0)
    {
        errAdd(sclwsERR_BIND, portNumber);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }

    logInfo("Listening on port '%d'.", portNumber);

    // Infinite loop to receive requests
    for (;;)
    {
        // Wait (without timeout) a new connection
        v_soap.accept_timeout = 1;
        if (soap_accept(&v_soap) < 0)
        {
            continue;
        }
        
        // Fork the SOAP context end start a new thread to handle the request
        v_tsoap = soap_copy(&v_soap);
        int status = pthread_create(&v_tid, NULL, (void*(*)(void*))sclwsJobHandler, (void*)v_tsoap);
        if (status != 0)
        {
            // error handling
            errAdd(sclwsERR_THREAD_CREATION);
            errCloseStack();
            sclwsExit(EXIT_FAILURE);
        }
    }

    sclwsExit(EXIT_SUCCESS);
}

/*___oOo___*/

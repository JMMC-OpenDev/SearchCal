/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * SearchCal SOAP webservice server.
 *
 * \b Synopsis: \n
 * \<sclwsServer\>
 * 
 * \b Details: \n
 * This daemon listen on port:
 * \li 8079 by default (beta config);
 * \li 8078 for production purpose (code editing required);
 * \li value of the 'SCLWS_PORT_NUMBER' environment variable if defined.
 * 
 * \env
 * \envvar SCLWS_PORT_NB : socket port number the server should bind on (must be greater than 1024, less than 65536).
 */

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <time.h>


/**
 * @namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "misc.h"
#include "log.h"
#include "err.h"
#include "evh.h"
#include "thrd.h"
#include "timlog.h"
#include "alx.h"


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
// SOAP execution context
struct Namespace* namespaces;
struct soap       globalSoapContext;


/*
 * Local macro
 */
#define STL_LOCK(error) { \
    if (thrdMutexLock(&threadStlMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

#define STL_UNLOCK(error) { \
    if (thrdMutexUnlock(&threadStlMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

/**
 * Shared mutex to circumvent un thread safe STL
 */
static thrdMUTEX threadStlMutex = MCS_RECURSIVE_MUTEX_INITIALIZER;
/**
 * Used to store each inactive threads
 * to ensure proper thread end using pthread_join
 */
static list<pthread_t> inactiveThreadList;
/**
 * Used to store each created threads
 * to ensure proper thread end using pthread_join
 */
static list<pthread_t> activeThreadList;

/** pthread identifier of the GC thread */
static pthread_t gcThreadId;

/*
 * Local Functions
 */

/**
 * Traverse the given thread list to call pthread_cancel (if doCancel = true) and pthread_join on each thread and remove them
 * @param name list name
 * @param threadList thread list to modify (passed by reference)
 * @param doCancel use pthread_cancel before calling pthread_join to interrupt properly the job
 */
void joinThreads(const char* name, list<pthread_t> &threadList, const bool doCancel)
{
    pthread_t* threadId;
    bool finished = false;

    logDebug("GC: joinThreads[%s] : start", name);
    
    while (!finished)
    {
        threadId = NULL;
        
        STL_LOCK();

        if (threadList.empty())
        {
            finished = true;
        } 
        else
        {
            /* get last element */
            threadId = &threadList.back();
        }

        STL_UNLOCK();
        
        if (threadId != NULL)
        {
             if (doCancel) 
             {
                logWarning("GC: Cancelling Thread[%d] ...", *threadId);

                pthread_cancel(*threadId);
                
                logWarning("GC: Thread[%d] cancelled.", *threadId);
             }
             
            logInfo("GC: Waiting for Thread[%d] ...", *threadId);

            pthread_join(*threadId, NULL);
            
            logInfo("GC: Thread[%d] terminated.", *threadId);
            
            STL_LOCK();

            /* remove item now */
            threadList.remove(*threadId);

            STL_UNLOCK();
        }
    }
    
    logDebug("GC: joinThreads[%s] : exit", name);
}


/**
 * Traverse the given thread list to call pthread_join on each thread and remove them WITHOUT cancel
 * @param name list name
 * @param threadList thread list to modify (passed by reference)
 */
void joinThreads(const char* name, list<pthread_t> &threadList)
{
    joinThreads(name, threadList, false);
}


/**
 * Main SOAP handler used by new pthread
 * @param soapContextPtr soap context
 * @return null
 */
void* sclwsJobHandler(void* soapContextPtr)
{
    struct soap* soapContext = (struct soap*) soapContextPtr;

    pthread_t threadId = pthread_self();
    
    logDebug("sclwsJobHandler - enter soap_serve : %d", threadId);
    
    // Fulfill the received remote call
    soap_serve(soapContext);
    
    logDebug("sclwsJobHandler - exit soap_serve : %d", threadId);

    soap_destroy(soapContext); // Dealloc C++ data
    soap_end(soapContext);     // Dealloc data and cleanup
    soap_done(soapContext);    // Detach soap struct

    free(soapContext);
   
    STL_LOCK(NULL);

    logDebug("adding Thread to inactiveThreadList : %d", threadId);

    inactiveThreadList.push_back(threadId);
    activeThreadList.remove(threadId);

    STL_UNLOCK(NULL);
    
    pthread_exit(NULL);
    
    return NULL;
}


/**
 * Main GC handler used by dedicated pthread
 * @return null
 */
void* sclwsGCHandler(void* args)
{
    logDebug("sclwsGCHandler: start");

    struct timespec sleepTime;
    struct timespec remainingSleepTime;

    // define sleep delay:
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 200 * 1000 * 1000;
    
    while (true)
    {

        // cancellation can occur while sleeping:
        nanosleep(&sleepTime, &remainingSleepTime);

        // Cleanup inactive threads:
        logDebug("GC: cleanup inactiveThreadList");
        
        joinThreads("inactiveThreadList", inactiveThreadList);

        // perform garbage collection (GC):
        freeServerList(false);
    }
    
    logDebug("sclwsGCHandler: exit");
    
    pthread_exit(NULL);

    return NULL;    
}


/**
 * Exit hook
 * @param returnCode
 */
void sclwsExit(int returnCode)
{
    logWarning("sclwsExit[%d] : ", returnCode);

    // wait for GC thread:
    logInfo("Cancelling GC Thread[%d] ...", gcThreadId);

    pthread_cancel(gcThreadId);

    logInfo("GC Thread[%d] cancelled.", gcThreadId);

    logInfo("Waiting for GC Thread[%d] ...", gcThreadId);

    pthread_join(gcThreadId, NULL);

    logInfo("GC Thread[%d] terminated.", gcThreadId);
    
    // cleanup all threads (join all created threads) without cancellation :
    bool doCancelActiveThreads = false;
    
    joinThreads("activeThreadList", activeThreadList, doCancelActiveThreads);
    joinThreads("inactiveThreadList", inactiveThreadList);
    
    // perform GC:
    freeServerList(true);
    
    // Dealloc SOAP context
    logTest("Cleaning gSOAP ...");
    soap_done(&globalSoapContext);

    // Close MCS services
    logDebug("Cleaning MCS ...");
    mcsExit();
    
    // Exit from the application with SUCCESS
    printf("Exiting now.\n");
    exit (returnCode);
}


/**
 * Init hook
 */
void sclwsInit() {
    logInfo("sclwsInit : start");
    
    /* Start timer log (initialize the hash table) */
    timlogDebugStart("sclwsInit");
    
    // initialize alx module (preload tables):
    alxAngularDiameterInit();
    alxCorrectedMagnitudeInit();
    alxInterstellarAbsorptionInit();
    alxResearchAreaInit();

    /* Stop timer log */
    timlogStop("sclwsInit");

    // logs any error and reset global stack:
    errCloseStack();
    
    // Create GC thread:
    int status = pthread_create(&gcThreadId, NULL, (void*(*)(void*))sclwsGCHandler, (void*)NULL);
    if (status != 0)
    {
        // Error handling
        errAdd(sclwsERR_THREAD_CREATION);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }
    logInfo("GC Thread[%d] started.", gcThreadId);
    
    logInfo("sclwsInit : done");
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
        sclwsExit(EXIT_FAILURE);
    }

    // Initialize the server to load configuration files before creating threads:
    sclwsInit();
    
    // @TODO : enhance the "garbage collector" thread to close pending connections

    logDebug("Initializing gSOAP ...");

    // SOAP Initialization
    soap_init(&globalSoapContext);
    
    // no socket timeout:
    globalSoapContext.accept_timeout = 0;
    
    // reuse server port:
    globalSoapContext.bind_flags = SO_REUSEADDR;
    
    soap_set_namespaces(&globalSoapContext, soap_namespaces);

    mcsUINT16 portNumber = sclwsGetServerPortNumber();

    // Main SOAP socket creation
    SOAP_SOCKET m = soap_bind(&globalSoapContext, NULL, portNumber, 100);
    if ((m < 0) || (!soap_valid_socket(m)))
    {
        errAdd(sclwsERR_BIND, portNumber);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }

    /* use logError to initialize anyway the log module (socket ...) */
    logError("Server ready: Listening on port '%d'.", portNumber);
    
    // Infinite loop to receive requests
    for (uint nbOfConnection = 1; ; nbOfConnection++)
    {
        // Wait (without timeout) a new connection
        SOAP_SOCKET s = soap_accept(&globalSoapContext);
        
        if ((s < 0) || (!soap_valid_socket(s)))
        {
           continue; // retry
        }         

        // Fork the SOAP context
        struct soap* forkedSoapContext = soap_copy(&globalSoapContext);

        // Start a new thread to handle the request
        pthread_t threadId;
        int status = pthread_create(&threadId, NULL, (void*(*)(void*))sclwsJobHandler, (void*)forkedSoapContext);
        if (status != 0)
        {
            // Error handling
            errAdd(sclwsERR_THREAD_CREATION);
            errCloseStack();
            sclwsExit(EXIT_FAILURE);
        }

        STL_LOCK(EXIT_FAILURE);

        logDebug("adding Thread to activeThreadList : %d", threadId);
        
        activeThreadList.push_back(threadId);

        STL_UNLOCK(EXIT_FAILURE);        

    }

    sclwsExit(EXIT_SUCCESS);
}

/*___oOo___*/

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
#include "vobs.h"


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

#define TH_ID_LOCK(error) { \
    if (thrdMutexLock(&threadIdMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

#define TH_ID_UNLOCK(error) { \
    if (thrdMutexUnlock(&threadIdMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

/**
 * Shared mutex to circumvent un thread safe STL
 */
static thrdMUTEX threadStlMutex = MCS_MUTEX_STATIC_INITIALIZER;
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


/**
 * Shared mutex to circumvent un thread safe STL related to thread Id generation
 */
static thrdMUTEX threadIdMutex = MCS_MUTEX_STATIC_INITIALIZER;

/**
 * Used to store each "pthread_t"-"thread Id pointer (int)" couples
 */
static map<pthread_t, mcsUINT32*> sclwsThreadIdMap;

/** thread Id generator */
static mcsUINT32 threadIdGenerator = 0;

/*
 * Local Functions
 */


/**
 * Get the unique identifier (positive integer) > 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @return unique identifier (positive integer) > 1 or -1 if not found
 */
mcsUINT32 getThreadId(const pthread_t threadId)
{
    mcsUINT32 thId;
    
    TH_ID_LOCK(-1);
    
    mcsUINT32* prev = sclwsThreadIdMap[threadId];
    if (prev == NULL) {
        thId = -1;
    } else {
        thId = *prev;
    }
    
    TH_ID_UNLOCK(-1);
    
    return thId;
}

/**
 * Generate an unique identifier (positive integer) > 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @return unique identifier (positive integer) > 1
 */
mcsUINT32 getUniqueThreadId(const pthread_t threadId)
{
    mcsUINT32* thId;
    
    TH_ID_LOCK(-1);
    
    mcsUINT32* prev = sclwsThreadIdMap[threadId];
    if (prev == NULL) {
        ++threadIdGenerator;
        
        thId = (mcsUINT32*)malloc(sizeof(mcsUINT32));
        *thId = threadIdGenerator;
        
        sclwsThreadIdMap[threadId] = thId;
    } else {
        thId = prev;
    }
    
    TH_ID_UNLOCK(-1);
    
    return *thId;
}


/**
 * Free integer pointers present in sclwsThreadIdMap
 */
void freeThreadIdMap()
{
    map<pthread_t, mcsUINT32*>::iterator iter;
    for (iter = sclwsThreadIdMap.begin(); iter != sclwsThreadIdMap.end(); iter++)
    {
        free(iter->second);
        
        sclwsThreadIdMap.erase(iter);
    }
}


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
                logWarning("Cancelling Thread[%d] ...", getThreadId(*threadId));

                pthread_cancel(*threadId);
                
                logWarning("Thread[%d] cancelled.", getThreadId(*threadId));
             }
             
            logInfo("Waiting for Thread[%d] ...", getThreadId(*threadId));

            pthread_join(*threadId, NULL);
            
            logInfo("Thread[%d] terminated.", getThreadId(*threadId));
            
            STL_LOCK();

            /* remove item now */
            threadList.remove(*threadId);

            STL_UNLOCK();
        }
    }
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
    const pthread_t threadId = pthread_self();
    mcsUINT32 threadNum = getUniqueThreadId(threadId);
    
    mcsSTRING32 threadName;
    snprintf(threadName, sizeof(threadName) - 1,  "SoapThread-%d", threadNum);    
    
    // Define thread info for logging purposes:
    mcsSetThreadInfo(threadNum, threadName);
    
    logDebug("adding Thread to activeThreadList : %s", threadName);

    STL_LOCK(NULL);

    activeThreadList.push_back(threadId);

    STL_UNLOCK(NULL);        
    
    struct soap* soapContext = (struct soap*) soapContextPtr;
    
    // Fulfill the received remote call
    soap_serve(soapContext);

    soap_destroy(soapContext); // Dealloc C++ data
    soap_end(soapContext);     // Dealloc data and cleanup
    soap_done(soapContext);    // Detach soap struct

    free(soapContext);

    logDebug("moving Thread to inactiveThreadList : %s", threadName);
   
    STL_LOCK(NULL);

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
    const pthread_t threadId = pthread_self();
    mcsUINT32 threadNum = getUniqueThreadId(threadId);
    
    // Define thread info for logging purposes:
    mcsSetThreadInfo(threadNum, "GCThread");    
    
    logDebug("sclwsGCHandler: start");

    struct timespec sleepTime;
    struct timespec remainingSleepTime;

    // define sleep delay:
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 50 * 1000 * 1000;
    
    while (true)
    {
        // cancellation can occur while sleeping:
        nanosleep(&sleepTime, &remainingSleepTime);

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
    logInfo("Cancelling GC Thread ...");

    pthread_cancel(gcThreadId);

    logInfo("GC Thread cancelled.");

    logInfo("Waiting for GC Thread ...");

    pthread_join(gcThreadId, NULL);

    logInfo("GC Thread terminated.");
    
    // cleanup all threads (join all created threads) without cancellation :
    bool doCancelActiveThreads = false;
    
    joinThreads("activeThreadList", activeThreadList, doCancelActiveThreads);
    joinThreads("inactiveThreadList", inactiveThreadList);
    
    // perform GC:
    freeServerList(true);
    
    // free thread id map:
    freeThreadIdMap();
    
    // Dealloc SOAP context
    logTest("Cleaning gSOAP ...");
    soap_done(&globalSoapContext);

    // free the timlog table:
    timlogClear();
    
    // Stop err module:
    errExit();
    
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
    /* enable thread name output in logs */
    logSetPrintThreadName(mcsTRUE);

    logInfo("sclwsInit : start");
    
    /* Initialize the timlog module (hash table) */
    timlogInit();
    
    // initialize alx module (preload tables):
    alxAngularDiameterInit();
    alxCorrectedMagnitudeInit();
    alxInterstellarAbsorptionInit();
    alxResearchAreaInit();
    
    // initialize vobs module (vizier URI):
    vobsGetVizierURI();

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
    logInfo("GC Thread started.");
    
    logInfo("sclwsInit : done");
}


/* 
 * Signal catching functions  
 */
void sclwsSignalHandler (int signalNumber)
{
    logQuiet("Received a '%d' system signal ...", signalNumber);

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
    
    // Initialize err module:
    errInit();

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

    // no signals:
    globalSoapContext.socket_flags = MSG_NOSIGNAL;
    
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

    /* Initialize the log module (socket ...) */
    logQuiet("Server ready: Listening on port '%d'.", portNumber);
    
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
    }

    sclwsExit(EXIT_SUCCESS);
}

/*___oOo___*/

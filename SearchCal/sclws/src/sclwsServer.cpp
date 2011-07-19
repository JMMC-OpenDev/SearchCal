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
#include <malloc.h>


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
    if (thrdMutexLock(&sclwsThreadStlMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

#define STL_UNLOCK(error) { \
    if (thrdMutexUnlock(&sclwsThreadStlMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

#define TH_ID_LOCK(error) { \
    if (thrdMutexLock(&sclwsThreadIdMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

#define TH_ID_UNLOCK(error) { \
    if (thrdMutexUnlock(&sclwsThreadIdMutex) == mcsFAILURE) \
    { \
        errAdd(sclwsERR_STL_MUTEX); \
        return error; \
    } \
}

/** free memory (malloc_trim) interval in milliseconds */
#define FREE_MEM_MS 30000

/** gc interval in milliseconds */
#define GC_INTERVAL_MS 250

/** lower thread id used when looping */
#define MIN_THREAD_ID 2

/** upper thread id before restarting to MIN_THREAD_ID */
#define MAX_THREAD_ID 999

/** 
 * Shared mutex to circumvent un thread safe STL
 */
static thrdMUTEX sclwsThreadStlMutex = MCS_MUTEX_STATIC_INITIALIZER;
/**
 * Used to store each inactive threads
 * to ensure proper thread end using pthread_join
 */
static std::list<pthread_t> sclwsInactiveThreadList;
/**
 * Used to store each created threads
 * to ensure proper thread end using pthread_join
 */
static std::list<pthread_t> sclwsActiveThreadList;

/** pthread identifier of the GC thread */
static pthread_t sclwsGCThreadId;

/**
 * Shared mutex to circumvent un thread safe STL related to thread Id generation
 */
static thrdMUTEX sclwsThreadIdMutex = MCS_MUTEX_STATIC_INITIALIZER;

/* thread ID map type */
typedef std::map<pthread_t, mcsUINT32> ThreadIdMap;

/**
 * Used to store each "pthread_t"-"thread Id (int)" pairs
 */
static ThreadIdMap sclwsThreadIdMap;

/** thread Id generator */
static mcsUINT32 sclwsThreadIdGenerator = 0;

/** thread creation counter */
static mcsUINT32 sclwsThreadCreated = 0;

/** thread termination counter */
static mcsUINT32 sclwsThreadJoined  = 0;


/*
 * Local Functions
 */

/**
 * Generate an unique identifier (positive integer) > 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @return unique identifier (positive integer) > 1
 */
mcsUINT32 sclwsGetUniqueThreadId(const pthread_t threadId)
{
    mcsUINT32 thId;
    
    TH_ID_LOCK(-1);
    
    ThreadIdMap::iterator prev = sclwsThreadIdMap.find(threadId);
    if (prev == sclwsThreadIdMap.end()) {
        // increment thread id:
        sclwsThreadIdGenerator++;
        
        if (sclwsThreadIdGenerator > MAX_THREAD_ID)
        {
            sclwsThreadIdGenerator = MIN_THREAD_ID; 
        }
        
        thId = sclwsThreadIdGenerator;

        sclwsThreadIdMap[threadId] = thId;
    } else {
        thId = (*prev).second;
    }
    
    TH_ID_UNLOCK(-1);
    
    return thId;
}

/**
 * Get and remove the unique identifier (positive integer) > 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @return unique identifier (positive integer) > 1 or -1 if not found
 */
mcsUINT32 sclwsGetAndRemoveThreadId(const pthread_t threadId)
{
    mcsUINT32 thId;
    
    TH_ID_LOCK(-1);
    
    ThreadIdMap::iterator prev = sclwsThreadIdMap.find(threadId);
    if (prev == sclwsThreadIdMap.end()) {
        thId = -1;
    } else {
        thId = (*prev).second;
        
        // remove identifier:
        sclwsThreadIdMap.erase(prev);
    }
    
    TH_ID_UNLOCK(-1);
    
    return thId;
}

/**
 * Free sclwsThreadIdMap
 */
void sclwsFreeThreadIdMap()
{
    TH_ID_LOCK();

    sclwsThreadIdMap.clear();
    
    TH_ID_UNLOCK();    
}

/**
 * Dump thread / session statistics and puts malloc statistics in stderr (DEBUG mode only)
 */
void sclwsFreeMemory()
{
    logInfo("Freeing memory...");

    malloc_trim(0);
    
#ifdef DEBUG            
    // memory allocation information:
    fprintf(stderr, "---------------------------------------\nmalloc_stats:\n");
    fflush(stderr);                   
    
    // dump memory statistics:
    malloc_stats();
    
#endif
    
}

/**
 * Dump thread / session statistics and puts malloc statistics in stderr (DEBUG mode only)
 */
void sclwsStats()
{
    // data race are possible (dirty reads allowed):
    logInfo("Thread Statistics : %d created / %d terminated.", sclwsThreadCreated, sclwsThreadJoined);

    logInfo("Session Statistics: %d created / %d deleted.", sclwsGetServerCreated(), sclwsGetServerDeleted());
}

/**
 * Traverse the given thread list to call pthread_cancel (if doCancel = true) and pthread_join on each thread and remove them
 * @param name list name
 * @param threadList thread list to modify (passed by reference)
 * @param doCancel use pthread_cancel before calling pthread_join to interrupt properly the job
 */
mcsLOGICAL sclwsJoinThreads(const char* name, std::list<pthread_t> &threadList, const bool doCancel)
{
    mcsLOGICAL result    = mcsFALSE;
    bool       finished  = false;
    pthread_t* threadId;
    mcsUINT32  threadNum;
    
    while (!finished)
    {
        threadId = NULL;
        
        STL_LOCK(result);

        if (threadList.empty())
        {
            finished = true;
        } 
        else
        {
            /* get last element */
            threadId = &threadList.back();
        }

        STL_UNLOCK(result);
        
        if (threadId != NULL)
        {
            result = mcsTRUE;
            
            threadNum = sclwsGetAndRemoveThreadId(*threadId);

            if (doCancel) 
            {
                logWarning("%s: Cancelling Thread-%d ...", name, threadNum);

                pthread_cancel(*threadId);
                
                logWarning("%s: Thread-%d cancelled.", name, threadNum);
            }
             
            logInfo("%s: Waiting for Thread-%d ...", name, threadNum);

            pthread_join(*threadId, NULL);
            
            logInfo("%s: Thread-%d terminated.", name, threadNum);
            
            STL_LOCK(result);

            sclwsThreadJoined++;
            
            /* remove item after pthread has joined */
            threadList.remove(*threadId);

            STL_UNLOCK(result);
        }
    }
    return result;
}

/**
 * Traverse the given thread list to call pthread_join on each thread and remove them WITHOUT cancel
 * @param name list name
 * @param threadList thread list to modify (passed by reference)
 */
mcsLOGICAL sclwsJoinThreads(const char* name, std::list<pthread_t> &threadList)
{
    return sclwsJoinThreads(name, threadList, false);
}


/**
 * Main SOAP handler used by new pthread
 * @param soapContextPtr soap context
 * @return null
 */
void* sclwsJobHandler(void* soapContextPtr)
{
    // Use block to ensure C++ frees local variables before calling pthread_exit()
    {
        const pthread_t threadId = pthread_self();
        mcsUINT32 threadNum = sclwsGetUniqueThreadId(threadId);
    
        mcsSTRING32 threadName;
        snprintf(threadName, sizeof(threadName) - 1,  "SoapThread-%d", threadNum);    
    
        // Define thread info for logging purposes:
        mcsSetThreadInfo(threadNum, threadName);
    
        logDebug("adding Thread to activeThreadList : %s", threadName);

        STL_LOCK(NULL);

        sclwsThreadCreated++;
        sclwsActiveThreadList.push_back(threadId);

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

        sclwsInactiveThreadList.push_back(threadId);
        sclwsActiveThreadList.remove(threadId);

        STL_UNLOCK(NULL);
    }    
    
    pthread_exit(NULL);
    
    return NULL;
}


/**
 * Main GC handler used by dedicated pthread
 * @return null
 */
void* sclwsGCJobHandler(void* args)
{
    // Use block to ensure C++ frees local variables before calling pthread_exit()
    {
        const bool isLogInfo = (logIsStdoutLogLevel(logINFO) == mcsTRUE);
        
        const pthread_t threadId = pthread_self();
        mcsUINT32 threadNum = sclwsGetUniqueThreadId(threadId);
    
        // Define thread info for logging purposes:
        mcsSetThreadInfo(threadNum, "GCThread");    
    
        logDebug("sclwsGCHandler: start");

        // true for the first time statistics:
        mcsLOGICAL result     = mcsTRUE;
        int iteration         = 0;
        
        struct timespec sleepTime;
        struct timespec remainingSleepTime;

        // define sleep delay:
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = GC_INTERVAL_MS * 1000 * 1000;
        
        const int modFree = FREE_MEM_MS / GC_INTERVAL_MS;
    
        while (true)
        {
            /* Since default cancel type is deferred, changing the state   */
            /* will allow the next cancelation point to cancel the thread (nanosleep) */
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

            // cancellation can occur while sleeping:
            nanosleep(&sleepTime, &remainingSleepTime);

            /* disable cancelation during cleanup */
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            
            // perform GC:
            if (sclwsJoinThreads("inactiveThreadList", sclwsInactiveThreadList) == mcsTRUE)
            {
                result = mcsTRUE;
            }

            // perform garbage collection (GC):
            if (sclwsFreeServerList(false) == mcsTRUE)
            {
                result = mcsTRUE;
            }
            
            if ((iteration % modFree == 0) && (result == mcsTRUE))
            {
                // free memory:
                sclwsFreeMemory();

                if (isLogInfo)
                {
                    sclwsStats();
                }
                
                result = mcsFALSE;
            }
            
            iteration++;
        }
    
        logDebug("sclwsGCHandler: exit");
    }    
    
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

    pthread_cancel(sclwsGCThreadId);

    logInfo("GC Thread cancelled.");

    logInfo("Waiting for GC Thread ...");

    pthread_join(sclwsGCThreadId, NULL);

    logInfo("GC Thread terminated.");
    
    // cleanup all threads (join all created threads) without cancellation :
    bool doCancelActiveThreads = false;
    
    sclwsJoinThreads("activeThreadList", sclwsActiveThreadList, doCancelActiveThreads);
    sclwsJoinThreads("inactiveThreadList", sclwsInactiveThreadList);
    
    // Now, no more thread is running:
    
    // Dealloc SOAP context
    logInfo("Cleaning gSOAP ...");
    soap_done(&globalSoapContext);
    
    // perform GC:
    sclwsFreeServerList(true);
    
    // free thread id map:
    sclwsFreeThreadIdMap();

    // free the timlog table:
    timlogClear();
    
    // Stop err module:
    errExit();
    
    // free memory:
    sclwsFreeMemory();
    
    // dump statistics:
    sclwsStats();
    
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
    int status = pthread_create(&sclwsGCThreadId, NULL, (void*(*)(void*))sclwsGCJobHandler, (void*)NULL);
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
    /* Tune linux memory management */
    
    // disable automatic malloc trim (it is done periodically i.e. GC like):
    mallopt(M_TRIM_THRESHOLD, -1);
    
    // Turn off mmap usage (slower and do not release memory efficiently)
    mallopt (M_MMAP_MAX, 0);
    
    // Define mmap threshold= 32M (default=128K):
    // mallopt(M_MMAP_THRESHOLD, 32 * 1024 * 1024);
    
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

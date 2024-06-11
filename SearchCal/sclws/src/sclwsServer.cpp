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
#include <execinfo.h>
#include <unistd.h>


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
#include "sclsvr.h"


/*
 * Local Headers
 */
#include "soapH.h"
#include "soap.nsmap"
#include "sclwsErrors.h"
#include "sclwsPrivate.h"


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

#define addSignalHandler(num, sh)                    \
if (signal(num, sh) == SIG_ERR)                      \
{                                                    \
    logError("signal(%d, ...) function error", num); \
    exit(EXIT_FAILURE);                              \
}

/** free memory (malloc_trim) interval in milliseconds */
#define FREE_MEM_MS 5000

/** free memory padding (malloc_trim) in bytes */
#define FREE_MEM_PAD 4 * 4096

/** display statistics interval in milliseconds */
#define STATS_MS 60000

/** gc interval in milliseconds */
#define GC_INTERVAL_MS 200

/** undefined thread id */
#define UNDEFINED_THREAD_ID -1

/** thread id assigned to GC Thread */
#define GC_THREAD_ID 0

/** lower thread id used when looping */
#define MIN_THREAD_ID 1

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

/**
 * Used to store each "pthread_t"-"thread Id (int)" pairs
 */
static std::map<pthread_t, mcsUINT32> sclwsThreadIdMap;

/** thread Id generator */
static mcsUINT32 sclwsThreadIdGenerator = MIN_THREAD_ID;

/** thread creation counter */
static mcsUINT32 sclwsThreadCreated = 0;

/** thread termination counter */
static mcsUINT32 sclwsThreadJoined  = 0;

/** flag to avoid reentrant shutdown hook */
static mcsLOGICAL sclwsShutdown = mcsFALSE;


/*
 * Local Functions
 */

/**
 * Generate an unique identifier (positive integer) >= 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @param threadNum unique identifier pointer (positive integer) > 1
 */
void sclwsGetUniqueThreadId(const pthread_t threadId, mcsUINT32* threadNum)
{
    // reset threadNum:
    *threadNum = UNDEFINED_THREAD_ID;

    mcsUINT32 thId;

    TH_ID_LOCK();

    std::map<pthread_t, mcsUINT32>::iterator prev = sclwsThreadIdMap.find(threadId);
    if (prev == sclwsThreadIdMap.end())
    {

        thId = sclwsThreadIdGenerator;

        sclwsThreadIdMap.insert(std::pair<pthread_t, mcsUINT32>(threadId, thId));

        // increment thread id for later call:
        sclwsThreadIdGenerator++;

        if (sclwsThreadIdGenerator > MAX_THREAD_ID)
        {
            sclwsThreadIdGenerator = MIN_THREAD_ID;
        }

    }
    else
    {
        thId = (*prev).second;
    }

    TH_ID_UNLOCK();

    *threadNum = thId;
}

/**
 * Get and remove the unique identifier (positive integer) >= 1 representing this pthread identifier
 * @param threadId pthread identifier
 * @param threadNum unique identifier (positive integer) >= 1 or -1 if not found
 */
void sclwsGetAndRemoveThreadId(const pthread_t threadId, mcsUINT32* threadNum)
{
    // reset threadNum:
    *threadNum = UNDEFINED_THREAD_ID;

    mcsUINT32 thId;

    TH_ID_LOCK();

    std::map<pthread_t, mcsUINT32>::iterator prev = sclwsThreadIdMap.find(threadId);
    if (prev == sclwsThreadIdMap.end())
    {
        thId = UNDEFINED_THREAD_ID;
    }
    else
    {
        thId = (*prev).second;

        // remove identifier:
        sclwsThreadIdMap.erase(prev);
    }

    TH_ID_UNLOCK();

    *threadNum = thId;
}

/**
 * Define the thread name
 * @param threadNum unique identifier pointer (positive integer) > 1
 * @param threadName returned thread name
 */
void sclwsGetSoapThreadName(const mcsUINT32 threadNum, char* threadName)
{
    sprintf(threadName, "SoapThread-%03d", threadNum);
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
    logDebug("Freeing memory...");

    malloc_trim(FREE_MEM_PAD);
}

/**
 * Get thread statistics
 */
void sclwsThreadStats(mcsUINT32 *threadCreated, mcsUINT32 *threadJoined)
{
    STL_LOCK();

    *threadCreated = sclwsThreadCreated;
    *threadJoined = sclwsThreadJoined;

    STL_UNLOCK();
}

/**
 * Dump thread / session statistics and puts malloc statistics in stderr (DEBUG mode only)
 */
void sclwsStats()
{
    // Stats:
    mcsUINT32 threadCreated, threadJoined;
    mcsUINT32 serverCreated, serverDeleted, serverCancelled, serverFailed;

    // Get thread statistics
    threadCreated = threadJoined = 0;
    sclwsThreadStats(&threadCreated, &threadJoined);
    logInfo("Thread  Statistics: %d created / %d terminated.", threadCreated, threadJoined);

    // GetCal statistics
    serverCreated = serverDeleted = serverCancelled = serverFailed = 0;
    sclwsGetCalStats(&serverCreated, &serverDeleted, &serverCancelled, &serverFailed);
    logInfo("GetCal  Statistics: %d created / %d deleted / %d cancelled / %d failed.",
            serverCreated, serverDeleted, serverCancelled, serverFailed);

    // GetStar statistics
    serverCreated = serverDeleted = serverFailed = 0;
    sclwsGetStarStats(&serverCreated, &serverDeleted, &serverFailed);
    logInfo("GetStar Statistics: %d created / %d deleted / %d failed.",
            serverCreated, serverDeleted, serverFailed);

    if (doLog(logTEST))
    {
        // memory allocation information:
        fprintf(stdout, "---------------------------------------\nmalloc_stats:\n");
        fflush(stdout);

        // dump memory statistics:
        malloc_stats();
    }
}

/**
 * Traverse the given thread list to call pthread_cancel (if doCancel = true) and pthread_join on each thread and remove them
 * @param name list name
 * @param threadList thread list to modify (passed by reference)
 * @param doCancel use pthread_cancel before calling pthread_join to interrupt properly the job
 */
mcsLOGICAL sclwsJoinThreads(const char* name, std::list<pthread_t>& threadList, const bool doCancel)
{
    mcsLOGICAL result    = mcsFALSE;
    bool       next      = true;
    pthread_t  threadId  = 0;
    mcsUINT32  threadNum = 0;
    mcsSTRING32 threadName;
    mcsINT32   thJoinErr = 0;

    while (next)
    {
        STL_LOCK(result);

        if (threadList.empty())
        {
            next = false;
        }
        else
        {
            // get an iterator on last element
            std::list<pthread_t>::iterator last = --threadList.end();

            /* get last element */
            threadId = *last;

            /* remove it now */
            threadList.erase(last);

            // note: this information is not accurate as the join will be done later ...
            sclwsThreadJoined++;
        }

        STL_UNLOCK(result);

        if (next)
        {
            result = mcsTRUE;

            sclwsGetAndRemoveThreadId(threadId, &threadNum);

            // note: this could happen during hot shutdown: sclwsJoinThreads("activeThreadList"...)
            if (threadNum == (mcsUINT32) UNDEFINED_THREAD_ID)
            {
                // fix thread joined counter:
                STL_LOCK(result);
                sclwsThreadJoined--;
                STL_UNLOCK(result);
            }
            else
            {
                sclwsGetSoapThreadName(threadNum, threadName);

                if (doCancel)
                {
                    logWarning("%s: Cancelling %s ...", name, threadName);

                    pthread_cancel(threadId);

                    logWarning("%s: %s cancelled.", name, threadName);
                }

                logTest("%s: Waiting for %s ...", name, threadName);

                thJoinErr = pthread_join(threadId, NULL);

                if (thJoinErr != 0)
                {
                    logDebug("%s: pthread_join failed %s (%d)", name, threadName, threadNum);

                    // fix thread joined counter:
                    STL_LOCK(result);
                    sclwsThreadJoined--;
                    STL_UNLOCK(result);
                }
                else
                {
                    logTest("%s: %s terminated.", name, threadName);
                }
            }
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
 * Define the thread information (thread number and name)
 * @param threadId pthread identifier
 * @param threadName returned thread name
 */
void sclwsInitSoapThread(const pthread_t threadId, char* threadName)
{
    mcsUINT32 threadNum;
    sclwsGetUniqueThreadId(threadId, &threadNum);
    sclwsGetSoapThreadName(threadNum, threadName);

    // Define thread info for logging purposes:
    mcsSetThreadInfo(threadNum, threadName);
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
        // block all signals for this thread:
        sigset_t my_set;
        sigfillset(& my_set);
        pthread_sigmask(SIG_SETMASK, &my_set, NULL);

        const pthread_t threadId = pthread_self();

        mcsSTRING32 threadName;
        sclwsInitSoapThread(threadId, threadName);

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

        sclwsActiveThreadList.remove(threadId);

        /*
         * Putting this pthread in sclwsInactiveThreadList
         * to let GCThread terminates it (using pthread_join)
         */
        sclwsInactiveThreadList.push_back(threadId);

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
        // block all signals for this thread:
        sigset_t my_set;
        sigfillset(& my_set);
        pthread_sigmask(SIG_SETMASK, &my_set, NULL);

        const bool isLogInfo = doLog(logINFO);

        // Define thread info for logging purposes:
        mcsSetThreadInfo(GC_THREAD_ID, "GCThread");

        logDebug("sclwsGCHandler: start");

        // true for the first time statistics:
        mcsLOGICAL result     = mcsTRUE;
        int iteration         = 0;

        struct timespec sleepTime;
        struct timespec remainingSleepTime;

        // define sleep delay:
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = GC_INTERVAL_MS * 1000 * 1000;

        const int modFree  = FREE_MEM_MS / GC_INTERVAL_MS;
        const int modStats = STATS_MS    / GC_INTERVAL_MS;

        while (true)
        {
            /* Since default cancel type is deferred, changing the state   */
            /* will allow the next cancelation point to cancel the thread (nanosleep) */
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

            // cancellation can occur while sleeping ie returns immediately:
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

            if (result == mcsTRUE)
            {
                if (iteration % modFree == 0)
                {
                    // free memory:
                    sclwsFreeMemory();
                }

                if (iteration % modStats == 0)
                {
                    if (isLogInfo)
                    {
                        sclwsStats();
                    }

                    result = mcsFALSE;
                }
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
    if (sclwsShutdown == mcsFALSE)
    {
        sclwsShutdown = mcsTRUE;

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

        // free property meta data:
        sclsvrExit();

        // free the timlog table:
        timlogClear();

        // Stop err and log module:
        errExit();
        logExit();

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
    else
    {
        logWarning("sclwsExit already in progress; please wait ...");
    }
}

/**
 * Init hook
 */
void sclwsInit()
{
    /* enable thread name output in logs */
    logSetPrintThreadName(mcsTRUE);

    logInfo("sclwsInit : start");

    /* Initialize the timlog module (hash table) */
    timlogInit();

    // initialize alx module (preload tables):
    alxInit();

    // initialize vobs module (vizier URI):
    vobsGetVizierURI();

    // initialize sclsvr module and preload JSDC:
    sclsvrInit(true);

    // logs any error and reset global stack:
    errCloseStack();

    // Create GC thread:
    int status = pthread_create(&sclwsGCThreadId, NULL, (void*(*)(void*) )sclwsGCJobHandler, (void*) NULL);
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
static inline void printStackTrace(mcsUINT32 max_frames = 63)
{
    printf("Stack trace:\n");

    // storage array for stack trace address data
    void* addrlist[max_frames + 1];

    // retrieve current stack addresses
    mcsINT32 addrlen = backtrace(addrlist, sizeof ( addrlist ) / sizeof (void*));

    if (addrlen == 0)
    {
        printf("[none]\n");
        return;
    }
    fflush(stdout);

    // write symbols to stdout
    backtrace_symbols_fd(addrlist, addrlen, STDOUT_FILENO);
}

void sclwsSignalHandler(int signum)
{
    // associate each signal with a signal name string.
    const char* name = NULL;
    switch ( signum )
    {
        case SIGINT:  name = "SIGINT";
            break;
        case SIGQUIT: name = "SIGQUIT";
            break;
        case SIGILL:  name = "SIGILL";
            break;
        case SIGABRT: name = "SIGABRT";
            break;
        case SIGBUS:  name = "SIGBUS";
            break;
        case SIGFPE:  name = "SIGFPE";
            break;
        case SIGSEGV: name = "SIGSEGV";
            break;
        case SIGPIPE: name = "SIGPIPE";
            break;
        case SIGTERM: name = "SIGTERM";
            break;
        default:      name = "[undefined]";
    }

    printf("Received a %s [%d] system signal ...\n", name, signum);
    fflush(stdout);

    if ((signum == SIGINT)
            || (signum == SIGQUIT)
            || (signum == SIGPIPE))
    {
        // just ignore such signal
        return;
    }
    if (signum == SIGTERM)
    {
        // Stop properly the service:
        sclwsExit(EXIT_SUCCESS);
    }
    else
    {
        // Fatal error:
        // Dump a stack trace.
        printStackTrace();

        exit(signum);
    }
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

    // Init system signal trapping
    addSignalHandler(SIGINT,  sclwsSignalHandler);
    addSignalHandler(SIGQUIT, sclwsSignalHandler);
    addSignalHandler(SIGILL,  sclwsSignalHandler);
    addSignalHandler(SIGABRT, sclwsSignalHandler);
    addSignalHandler(SIGBUS,  sclwsSignalHandler);
    addSignalHandler(SIGFPE,  sclwsSignalHandler);
    addSignalHandler(SIGSEGV, sclwsSignalHandler);
    addSignalHandler(SIGPIPE, sclwsSignalHandler);
    addSignalHandler(SIGTERM, sclwsSignalHandler);

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        sclwsExit(EXIT_FAILURE);
    }

    // Initialize log and err module:
    logInit();
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
        int status = pthread_create(&threadId, NULL, (void*(*)(void*) )sclwsJobHandler, (void*) forkedSoapContext);
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

/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of sclwsWS class.
 */

/*
 * System Headers
 */
#include <iostream>
#include <sstream>
using namespace std;
#include <map>
#include <uuid/uuid.h>
#include <pthread.h>
#include <unistd.h>

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "thrd.h"
#include "sclsvr.h"
#include "misco.h"
#include "timlog.h"

/*
 * Local Headers
 */
#include "soapH.h"
#include "sclwsErrors.h"
#include "sclwsPrivate.h"

/*
 * Local macro
 */
#define STL_LOCK(error) {                               \
    if (thrdMutexLock(&sclwsStlMutex) == mcsFAILURE)    \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        return error;                                   \
    }                                                   \
}

#define STL_LOCK_AND_SOAP_ERROR(soapContext) {          \
    if (thrdMutexLock(&sclwsStlMutex) == mcsFAILURE)    \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        sclwsReturnSoapError(soapContext);              \
    }                                                   \
}

#define STL_UNLOCK(error) {                             \
    if (thrdMutexUnlock(&sclwsStlMutex) == mcsFAILURE)  \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        return error;                                   \
    }                                                   \
}

#define STL_UNLOCK_AND_SOAP_ERROR(soapContext) {        \
    if (thrdMutexUnlock(&sclwsStlMutex) == mcsFAILURE)  \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        sclwsReturnSoapError(soapContext);              \
    }                                                   \
}

#define UUID_LOCK_AND_SOAP_ERROR(soapContext) {         \
    if (thrdMutexLock(&sclwsUuidMutex) == mcsFAILURE)   \
    {                                                   \
        errAdd(sclwsERR_UUID_MUTEX);                    \
        sclwsReturnSoapError(soapContext);              \
    }                                                   \
}

#define UUID_UNLOCK_AND_SOAP_ERROR(soapContext) {       \
    if (thrdMutexUnlock(&sclwsUuidMutex) == mcsFAILURE) \
    {                                                   \
        errAdd(sclwsERR_UUID_MUTEX);                    \
        sclwsReturnSoapError(soapContext);              \
    }                                                   \
}

/* TODO: create GC_MUTEX error */
#define GC_LOCK(error) {                                \
    if (thrdMutexLock(&sclwsGCMutex) == mcsFAILURE)     \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        return error;                                   \
    }                                                   \
}

/* TODO: create GC_MUTEX error */
#define GC_UNLOCK(error) {                              \
    if (thrdMutexUnlock(&sclwsGCMutex) == mcsFAILURE)   \
    {                                                   \
        errAdd(sclwsERR_STL_MUTEX);                     \
        return error;                                   \
    }                                                   \
}


/** minimum delay (ms) before freeing server resources to let pending GetCalStatus queries run whereas GetCalQuery finished */
#define DELAY_BEFORE_GC 10000

/*
 * Local Variables
 */

/**
 * Shared mutex to circumvent un thread safe STL
 */
thrdMUTEX sclwsStlMutex = MCS_MUTEX_STATIC_INITIALIZER;

/**
 * Shared mutex to circumvent un thread safe to uuid library
 */
thrdMUTEX sclwsUuidMutex = MCS_MUTEX_STATIC_INITIALIZER;

/**
 * Used to store each "Communication ID"-"sclsvSERVER instance" couples
 * to ensure multiple concurrent query handling.
 */
map<string, sclsvrSERVER*> sclwsServerList;
/**
 * Used to store each "Communication ID"-"thread ID" couples
 * to ensure proper thread cancellation on ns__GetCalCancel() call.
 */
multimap<string, pthread_t> sclwsThreadList;
typedef multimap<string, pthread_t>::iterator sclwsTHREAD_ITERATOR;
typedef pair<sclwsTHREAD_ITERATOR, sclwsTHREAD_ITERATOR> sclwsTHREAD_RANGE;

/* server information used by GC thread */
struct sclwsServerInfo
{
    struct timeval lastUsedTime; /* last used time */
    string jobId;                /* job id */
    sclsvrSERVER* server;        /* server instance */
} ;

/**
 * Shared mutex to circumvent un thread safe STL
 */
thrdMUTEX sclwsGCMutex = MCS_MUTEX_STATIC_INITIALIZER;

/**
 * Used to store each inactive server instance to free
 */
static std::list<sclwsServerInfo*> sclwsGCServerInfoList;

/* server statistics */
struct sclwsServerStats
{
    mcsUINT32 created;
    mcsUINT32 deleted;
    mcsUINT32 cancelled;
    mcsUINT32 failed;
} ;

/** thread creation counter */
static sclwsServerStats sclwsServerStatsGetCal = { 0, 0, 0, 0 };

/** thread termination counter */
static sclwsServerStats sclwsServerStatsGetStar = { 0, 0, 0, 0 };

/**
 * Return the number of created and deleted server instances (GETCAL)
 * @param serverCreated number of created server instances (GETCAL)
 * @param serverDeleted number of deleted server instances (GETCAL)
 * @param serverCancelled number of cancelled server instances (GETCAL)
 */
void sclwsGetCalStats(mcsUINT32 *serverCreated, mcsUINT32 *serverDeleted, mcsUINT32 *serverCancelled, mcsUINT32 *serverFailed)
{
    STL_LOCK();

    *serverCreated   = sclwsServerStatsGetCal.created;
    *serverDeleted   = sclwsServerStatsGetCal.deleted;
    *serverCancelled = sclwsServerStatsGetCal.cancelled;
    *serverFailed    = sclwsServerStatsGetCal.failed;

    STL_UNLOCK();
}

/**
 * Return the number of created and deleted server instances (GETSTAR)
 * @param serverCreated number of created server instances (GETSTAR)
 * @param serverDeleted number of deleted server instances (GETSTAR)
 */
void sclwsGetStarStats(mcsUINT32 *serverCreated, mcsUINT32 *serverDeleted, mcsUINT32 *serverFailed)
{
    STL_LOCK();

    *serverCreated = sclwsServerStatsGetStar.created;
    *serverDeleted = sclwsServerStatsGetStar.deleted;
    *serverFailed  = sclwsServerStatsGetStar.failed;

    STL_UNLOCK();
}

/*
 * Local methods
 */
mcsUINT16 sclwsGetServerPortNumber(void)
{
    mcsUINT16 defaultPortNumber = 8079; // Default value for production purpose.

    // Try to read ENV. VAR. to get port number to bind
    mcsINT32 readPortNumber = -1;
    if (miscGetEnvVarIntValue(SCLWS_PORTNUMBER_ENVVAR_NAME, &readPortNumber) == mcsFAILURE)
    {
        logInfo("'%s' environment variable is not set, will use default '%d' instead.", SCLWS_PORTNUMBER_ENVVAR_NAME, defaultPortNumber);
        return defaultPortNumber;
    }

    // Assert given prot number validity
    mcsINT32 minPortNumber = 1024;
    mcsINT32 maxPortNumber = 65536;
    if ((readPortNumber <= minPortNumber) || (readPortNumber >= maxPortNumber))
    {
        logWarning("'%s' environment variable does not contain a valid port number ('%d' not in [%d, %d[), will use default '%d' instead.", SCLWS_PORTNUMBER_ENVVAR_NAME, readPortNumber, minPortNumber, maxPortNumber, defaultPortNumber);
        return defaultPortNumber;
    }

    // Everything went fine, return read port number
    return readPortNumber;
}

int sclwsDumpServerList(struct soap* soapContext, const char* methodName, char* jobId)
{
    if (doLog(logDEBUG))
    {
        STL_LOCK_AND_SOAP_ERROR(soapContext);

        logDebug("sclwsServerList[%s = %s] dump:", methodName, jobId);

        for (std::map<string, sclsvrSERVER*>::iterator iter = sclwsServerList.begin(); iter != sclwsServerList.end(); iter++)
        {
            logDebug("ID: %s -> %p", iter->first.c_str(), iter->second);
        }

        STL_UNLOCK_AND_SOAP_ERROR(soapContext);
    }
    return SOAP_OK;
}

/*
 * Public methods
 */

/**
 * Free server instances (GC like)
 * @param forceCleanup flag to force cleanup
 */
mcsLOGICAL sclwsFreeServerList(const bool forceCleanup)
{
    mcsLOGICAL result     = mcsFALSE;
    const bool isLogDebug = doLog(logDEBUG);

    /*
     * Note/TODO : it waits for the known active GetCalStatus thread but if this query answers 1,
     * another future GetCalStatus query will happen next, and it will then fail (no server associated to jobId) !!
     *
     * Workaround: wait few seconds (DELAY_BEFORE_GC) before freeing resources anc check if any thread is working at this time
     */

    GC_LOCK(result);

    if (!sclwsGCServerInfoList.empty())
    {
        if (isLogDebug)
        {
            logDebug("freeServerList: enter");
        }

        struct timeval now;
        long deltaMillis;
        sclwsServerInfo* info;
        string jobId;
        bool jobHasThread;

        /* Get local time */
        gettimeofday(&now, NULL);

        std::list<sclwsServerInfo*>::iterator iter;
        for (iter = sclwsGCServerInfoList.begin(); iter != sclwsGCServerInfoList.end(); iter++)
        {
            info = *iter;

            jobId = info->jobId;

            // skip too recent ones ...

            deltaMillis  = (now.tv_sec  - info->lastUsedTime.tv_sec) * 1000;
            deltaMillis += (now.tv_usec - info->lastUsedTime.tv_usec) / 1000;

            if (forceCleanup || deltaMillis > DELAY_BEFORE_GC)
            {

                if (isLogDebug)
                {
                    logDebug("freeServerList: check Session '%s'", jobId.c_str());
                }

                jobHasThread = false;

                STL_LOCK(result);

                sclwsTHREAD_RANGE range = sclwsThreadList.equal_range(jobId);

                // TODO: use a better solution to check if the range is empty
                sclwsTHREAD_ITERATOR threadIterator;
                for (threadIterator = range.first; threadIterator != range.second; ++threadIterator)
                {
                    jobHasThread = true;

                    logInfo("freeServerList: Session '%s' has at least one thread : %d", jobId.c_str(), threadIterator->second);

                    break;
                }

                STL_UNLOCK(result);

                if (!jobHasThread)
                {
                    // do cleanup
                    result = mcsTRUE;

                    STL_LOCK(result);

                    sclwsServerStatsGetCal.deleted++;

                    sclwsServerList.erase(jobId);

                    STL_UNLOCK(result);

                    // Delete the server instance
                    logInfo("freeServerList: Session '%s': deleting associated server.", jobId.c_str());

                    // free Server resources:
                    delete(info->server);

                    // erase and get next correct position:
                    iter = sclwsGCServerInfoList.erase(iter);

                    // go backward as loop goes forward:
                    iter--;

                    // free server_info:
                    delete(info);
                }
            }
            else
            {
                if (isLogDebug)
                {
                    logDebug("Session '%s' terminated too recently = %d ms", jobId.c_str(), deltaMillis);
                }
            }
        }

        if (isLogDebug)
        {
            logDebug("freeServerList: exit");
        }
    }

    GC_UNLOCK(result);

    return result;
}

/*
 * sclwsGETCAL Web Service
 */

/**
 * Get a unique session Id used by all this Web Service related functions
 *
 * The returned Id is used to perform asynchroneous treatment using the
 * synchroneous SOAP based web service.
 *
 * @param soapContext SOAP execution context.
 * @param jobId give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalOpenSession(struct soap* soapContext, char** jobId)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }

    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError(soapContext);
    }

    if (doLog(logINFO))
    {
        // Compute connection IP and log it
        mcsSTRING16 connectionIP;
        // @WARNING : IP address computing code is probably endian-ness dependant !
        unsigned long ipAddress = soapContext->ip;
        long firstByte  = (long) ((ipAddress >> 24) & 0xFF);
        long secondByte = (long) ((ipAddress >> 16) & 0xFF);
        long thirdByte  = (long) ((ipAddress >> 8)  & 0xFF);
        long forthByte  = (long) ((ipAddress >> 0)  & 0xFF);
        snprintf(connectionIP, sizeof (connectionIP), "%ld.%ld.%ld.%ld", firstByte, secondByte, thirdByte, forthByte);

        logInfo("GetCal:  Accepted connection from IP address '%s'.", connectionIP);
    }

    // Create a "Universally Unique Identifier" (man uuid for more informations)
    uuid_t uuidID;

    UUID_LOCK_AND_SOAP_ERROR(soapContext);

    uuid_generate(uuidID);

    UUID_UNLOCK_AND_SOAP_ERROR(soapContext);

    // Allocate SOAP-aware memory to return the generated UUID
    int jobIdLength = 37; // An UUID is 36-byte string (plus tailing '\0')
    *jobId = (char*) soap_malloc(soapContext, jobIdLength);
    if (*jobId == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, jobIdLength);
        sclwsReturnSoapError(soapContext);
    }

    // Get the string of the newly generated uuid_t structure
    uuid_unparse(uuidID, *jobId);
    logDebug("Session '%s': unique identifier generated.", *jobId);

    // Create a new instance of sclsvrSERVER to perform the GETCAL query
    sclsvrSERVER* server = new sclsvrSERVER(mcsFALSE);
    if (server == NULL)
    {
        errAdd(sclwsERR_SERVER_INSTANCIATION);
        sclwsReturnSoapError(soapContext);
    }

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    sclwsServerStatsGetCal.created++;

    // Associate the new sclsvrSERVER instance with the generated UUID for later
    sclwsServerList[*jobId] = server;

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    logWarning("Session '%s': server instanciated.", *jobId);

    return sclwsDumpServerList(soapContext, "GetCalOpenSession", *jobId);
}

/**
 * Launch a GetCal query.
 *
 * @param soapContext SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param query the query to execute, in a double-quoted sclinsQuery format.
 * @param voTable give-back pointer to return the resulting VO Table.
 *
 * @return a SOAP error code.
 */
int ns__GetCalSearchCal(struct soap* soapContext,
                        char*  jobId,
                        char*  query,
                        char** voTable)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError(soapContext);
    }
    if (query == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "query");
        sclwsReturnSoapError(soapContext);
    }
    if (voTable == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "voTable");
        sclwsReturnSoapError(soapContext);
    }

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Retrieve the sclsvrSERVER instance associated with the received UUID
    sclsvrSERVER* server = sclwsServerList[jobId];

    if (server == NULL)
    {
        STL_UNLOCK_AND_SOAP_ERROR(soapContext);
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError(soapContext);
    }

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    // @TODO: this iterator may be inconsistent at erase time (concurrency)
    sclwsTHREAD_ITERATOR threadIterator =
            sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    // check reentrance in case of http retry on the client side (curl or apache HttpClient)
    // to avoid concurrency issue with sclsvrSERVER object:
    if (server->IsWorking())
    {
        logWarning("Session '%s': query in progress: '%s'; aborting.", jobId, query);

        errAdd(sclwsERR_GETCAL_WORKING);

        STL_LOCK_AND_SOAP_ERROR(soapContext);

        // Delete the thread ID
        sclwsThreadList.erase(threadIterator);

        STL_UNLOCK_AND_SOAP_ERROR(soapContext);

        sclwsReturnSoapError(soapContext);
    }

    logWarning("Session '%s': launching query : '%s'", jobId, query);

    int status = SOAP_OK;

    const char* result = NULL;
    miscDynSIZE resultSize = 0;

    // Launch the GETCAL query with the received parameters
    miscoDYN_BUF dynBuf;
    if (server->GetCal(query, &dynBuf) == mcsFAILURE)
    {
        sclwsDefineSoapError(soapContext);
        status = SOAP_ERR;
        goto cleanup;
    }

    // Allocate SOAP-aware memory to return the resulting VO Table
    dynBuf.GetNbStoredBytes(&resultSize);
    if (resultSize != 0)
    {
        logDebug("Session '%s': resulting VOTable ('%d' bytes)", jobId , resultSize);
        result = dynBuf.GetBuffer();
    }
    else
    {
        logDebug("Session '%s': no star found.", jobId);
        result = "";
        resultSize = strlen(result);
    }
    resultSize++; // For the trailing '\0'
    *voTable = (char*) soap_malloc(soapContext, resultSize);
    strncpy(*voTable, result, resultSize);

cleanup:

    logWarning("Session '%s': terminating query.", jobId);

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    if (status == SOAP_ERR)
    {
        sclwsServerStatsGetCal.failed++;
    }

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    // add server information for GC thread:
    sclwsServerInfo* info = new sclwsServerInfo;

    /* Get local time */
    gettimeofday(&info->lastUsedTime, NULL);

    info->jobId = string(jobId);
    info->server = server;

    GC_LOCK(status);

    sclwsGCServerInfoList.push_back(info);

    GC_UNLOCK(status);

    sclwsDumpServerList(soapContext, "GetCalSearchCal", jobId);

    return status;
}

/**
 * Give back the query execution state.
 *
 * @param soapContext SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param status give-back pointer to return the current query status.
 *
 * @return a SOAP error code.
 */
int ns__GetCalQueryStatus(struct soap* soapContext,
                          char*  jobId,
                          char** status)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError(soapContext);
    }
    if (status == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "status");
        sclwsReturnSoapError(soapContext);
    }

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = sclwsServerList[jobId];

    if (server == NULL)
    {
        STL_UNLOCK_AND_SOAP_ERROR(soapContext);
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError(soapContext);
    }

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    // @TODO: this iterator may be inconsistent at erase time (concurrency)
    sclwsTHREAD_ITERATOR threadIterator =
            sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    // Allocate SOAP-aware memory to return the current catalog name
    int statusLength = 256; // Should be enough !
    *status = (char*) soap_malloc(soapContext, statusLength);
    if (*status == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, statusLength);
        goto errCond;
    }
    if (server->GetStatus((mcsSTRING256*)*status) == mcsFAILURE)
    {
        goto errCond;
    }

    logInfo("Session '%s': query status = '%s'.", jobId, *status);

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    return sclwsDumpServerList(soapContext, "GetCalQueryStatus", jobId);

errCond:

    logWarning("Session '%s': query status failed.", jobId);

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    sclwsReturnSoapError(soapContext);
}

/**
 * Abort the work done under the given ID.
 *
 * @param soapContext SOAP execution context.
 * @param jobId pointer to communication "session" ID.
 * @param isOK give-back pointer to return whether the cancellation went OK or not.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCancelSession(struct soap* soapContext,
                            char*  jobId,
                            bool*  isOK)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError(soapContext);
    }
    if (isOK == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "isOK");
        sclwsReturnSoapError(soapContext);
    }

    logWarning("Session '%s': cancelling query ...", jobId);

    // Cancellation pending
    *isOK = false;

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = sclwsServerList[jobId];

    if (server != NULL)
    {
        sclwsServerStatsGetCal.cancelled++;

        // define cancellation flag within LOCK
        bool* cancelFlag = server->GetCancelFlag();

        logInfo("Setting cancel flag: true", cancelFlag);

        // dirty write:
        *cancelFlag = true;

        /*
         * Valgrind report:
        ==12272== Possible data race during write of size 1 at 0x9681920 by thread #8
        ==12272==    at 0x51362C5: ns__GetCalCancelSession(soap*, char*, bool*) (sclwsWS.cpp:720)
        ==12272== Possible data race during read of size 1 at 0x9681920 by thread #5
        ==12272==    at 0x55A521C: vobsIsCancelled() (vobsREMOTE_CATALOG.cpp:76)
         */
    }

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    // Cancellation succesfull
    *isOK = true;

    return sclwsDumpServerList(soapContext, "GetCalCancelSession", jobId);
}

/*
 * sclwsGETSTAR Web Service
 */

int ns__GetStar(struct soap* soapContext, char *query, char **output)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }
    if (query == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "query");
        sclwsReturnSoapError(soapContext);
    }
    if (output == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "output");
        sclwsReturnSoapError(soapContext);
    }

    if (doLog(logINFO))
    {
        // Compute connection IP and log it
        mcsSTRING16 connectionIP;
        // @WARNING : IP address computing code is probably endian-ness dependant !
        unsigned long ipAddress = soapContext->ip;
        long firstByte  = (long) ((ipAddress >> 24) & 0xFF);
        long secondByte = (long) ((ipAddress >> 16) & 0xFF);
        long thirdByte  = (long) ((ipAddress >> 8)  & 0xFF);
        long forthByte  = (long) ((ipAddress >> 0)  & 0xFF);
        snprintf(connectionIP, sizeof (connectionIP), "%ld.%ld.%ld.%ld", firstByte, secondByte, thirdByte, forthByte);

        logInfo("GetStar: Accepted connection from IP address '%s'.", connectionIP);
    }

    // Create a new instance of sclsvrSERVER to perform the GETSTAR query
    sclsvrSERVER* server = new sclsvrSERVER(mcsFALSE);
    if (server == NULL)
    {
        errAdd(sclwsERR_SERVER_INSTANCIATION);
        sclwsReturnSoapError(soapContext);
    }

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    sclwsServerStatsGetStar.created++;

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    logWarning("GetStar: server instanciated; launching query : '%s'", query);

    int status = SOAP_OK;

    const char* result = NULL;
    miscDynSIZE resultSize = 0;

    // Launch the GETSTAR query with the received parameters
    miscoDYN_BUF dynBuf;
    if (server->GetStar(query, &dynBuf) == mcsFAILURE)
    {
        sclwsDefineSoapError(soapContext);
        status = SOAP_ERR;
        goto cleanup;
    }

    // Allocate SOAP-aware memory to return the output
    dynBuf.GetNbStoredBytes(&resultSize);
    if (resultSize != 0)
    {
        logDebug("GetStar: Output ('%d' bytes)", resultSize);
        result = dynBuf.GetBuffer();
    }
    else
    {
        logDebug("GetStar: No star found.");
        result = "";
        resultSize = strlen(result);
    }
    resultSize++; // For the trailing '\0'
    *output = (char*) soap_malloc(soapContext, resultSize);
    strncpy(*output, result, resultSize);

cleanup:

    logWarning("GetStar: terminating query.");

    STL_LOCK_AND_SOAP_ERROR(soapContext);

    sclwsServerStatsGetStar.deleted++;

    if (status == SOAP_ERR)
    {
        sclwsServerStatsGetStar.failed++;
    }

    STL_UNLOCK_AND_SOAP_ERROR(soapContext);

    // free Server resources:
    delete(server);

    return status;
}

/*
 * sclwsGetServerStatus Web Service
 */

int ns__GetServerStatus(struct soap* soapContext, char** status)
{
    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError(soapContext);
    }
    if (status == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "status");
        sclwsReturnSoapError(soapContext);
    }

    // Generate server status:
    ostringstream out;

    out << "SearchCal Server Status" << endl << endl;
    out << "SearchCal software: http://www.jmmc.fr/searchcal (In case of problem, please report to jmmc-user-support@jmmc.fr)" << endl << endl;

    // Get the software name and version
    mcsSTRING32 softwareVersion;
    snprintf(softwareVersion, sizeof (softwareVersion) - 1, "%s v%s", "SearchCal Server", sclsvrVERSION);
    out << "version: " << softwareVersion << endl;

    out << "vizierURI: " << vobsGetVizierURI() << endl;
    out << "server port: " << sclwsGetServerPortNumber() << endl << endl;

    // Stats:
    mcsUINT32 threadCreated, threadJoined;
    mcsUINT32 serverCreated, serverDeleted, serverCancelled, serverFailed;

    // Get thread statistics
    threadCreated = threadJoined = 0;
    sclwsThreadStats(&threadCreated, &threadJoined);
    out << "Thread  Statistics: " << threadCreated << " created / " << threadJoined << " terminated." << endl;

    // GetCal statistics
    serverCreated = serverDeleted = serverCancelled = serverFailed = 0;
    sclwsGetCalStats(&serverCreated, &serverDeleted, &serverCancelled, &serverFailed);
    out << "GetCal  Statistics: " << serverCreated << " created / " << serverDeleted << " deleted / " << serverCancelled << " cancelled / " << serverFailed << " failed." << endl;

    // GetStar statistics
    serverCreated = serverDeleted = serverFailed = 0;
    sclwsGetStarStats(&serverCreated, &serverDeleted, &serverFailed);
    out << "GetStar Statistics: " << serverCreated << " created / " << serverDeleted << " deleted / " << serverFailed << " failed." << endl;

    string content = out.str();

    // Return result:

    int resultSize = content.length();
    resultSize++; // For the trailing '\0'

    *status = (char*) soap_malloc(soapContext, resultSize);
    strncpy(*status, content.c_str(), resultSize);

    return SOAP_OK;
}

/*___oOo___*/

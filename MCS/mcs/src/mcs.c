/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


/* 
 * System Headers
 */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <libgdome/gdome.h>
#include <libxml/parser.h>

/* 
 * Local Headers
 */
#include "mcs.h"
#include "mcsPrivate.h"

/*
 * Local variables 
 */
static mcsPROCNAME mcsProcName = mcsUNKNOWN_PROC;
static mcsENVNAME  mcsEnvName  = mcsUNKNOWN_ENV;

/* Global mutex to protected Gdome library access (recursive needed for err module) */
static mcsMUTEX gdomeMUTEX = MCS_RECURSIVE_MUTEX_INITIALIZER;

/* Gdome implementation singleton used by multiple threads */
static GdomeDOMImplementation* domimpl = NULL;

/** thread local storage key for thread informations */
static pthread_key_t tlsKey_threadInfo;

/** extern environment variables */
extern char **environ;

/* mutex dedicated to make mcsGetEnv_r() thread safe */
static pthread_mutex_t env_mutex = MCS_MUTEX_STATIC_INITIALIZER;

/*
 * Local functions
 */
static mcsCOMPL_STAT mcsStoreProcName (const char *procName);
static mcsCOMPL_STAT mcsStoreEnvName  (const char *envName);

/**
 * Thread local key destructor
 * @param value value to free
 */
static void tlsThreadIdDestructor(void* value)
{
  free(value);
  pthread_setspecific(tlsKey_threadInfo, NULL);
}


/**
 * \mainpage mcs : MCS services 
 * 
 * \n
 * \section description Description
 * This module is provided mcsInit() function which has to be used to register
 * process and environment to the MCS services. A process and its environment
 * have to be registered before calling a MCS services (log, msg, ...), as shown
 * in the following example.
 * 
 * \section example Code example
 * \code
 *  #include "mcs.h"
 *  ...
 *   
 *  int main(int argc, char *argv[])
 *  {
 *      if (mcsInit(argv[0]) == mcsFAILURE)
 *      {
 *          Process Error condition
 *          exit (EXIT_FAILURE);
 *      }
 *      ...
 *
 *      mcsExit();
 *
 *      exit (EXIT_SUCCESS);
 *  }
 *
 * \endcode
 */

/**
 * Initializes the MCS services.
 * It register application and environment to the MCS services. If process is
 * not identified, then default name is mcsUNKNOWN_PROC. If the $MCSENV
 * environment variable is not defined, then default environment name is
 * mcsUNKNOWN_ENV.
 *
 * \param procName name of the process.
 *
 * \warning This routine must be called ONCE, normally in the initialisation
 * phase of the applications, BEFORE any call to a MCS function (log, msg,...)
 * can be performed.
 *
 * \return mcsSUCCESS
 *
 * \sa mcsGetProcName, mcsGetEnvName.
 */
mcsCOMPL_STAT mcsInit(const mcsPROCNAME  procName)
{
    /* Store the application name */
    mcsStoreProcName(procName);

    /* Store the environment name */
    /* If the $MCS_ENV_NAME environment variable is defined */
    mcsSTRING32 envValue;
    if ((mcsGetEnv_r("MCSENV", envValue, sizeof(envValue)) == mcsSUCCESS) && (strlen(envValue) != 0))
    {
        /* Copy the environment variable content in mcsEnvName */
        mcsStoreEnvName(envValue);
    }

    /*
     * Starting with 2.4.7, libxml2 makes provisions to ensure that concurrent threads can safely work in parallel parsing different documents. There is however a couple of things to do to ensure it:
     *
     * configure the library accordingly using the --with-threads options
     * call xmlInitParser() in the "main" thread before using any of the libxml2 API (except possibly selecting a different memory allocator)
     */
    xmlInitParser();

    if (domimpl == NULL)
    {
        /* Get a DOMImplementation reference */
        domimpl = gdome_di_mkref();
    }

    const int rc = pthread_key_create(&tlsKey_threadInfo, tlsThreadIdDestructor);
    if (rc != 0) {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Returns the process name.
 *
 * It returns the name which has been used to register to the process using
 * mcsInit().
 *
 * \return the process name.
 *
 * \sa mcsInit
 */
const char *mcsGetProcName()
{
    return ((const char*)mcsProcName);
}

/**
 * Returns the environment name.
 *
 * \return the environment name.
 *
 * \sa mcsInit
 */
const char *mcsGetEnvName()
{
    return ((const char*)mcsEnvName);
}

/**
 * Return the thread identifier (positive integer >=0). 0 means Main thread
 * @return thread identifier (positive integer >=0)
 */
mcsUINT32 mcsGetThreadId()
{
    void* global;
    mcsTHREAD_INFO* threadInfo;
    mcsUINT32 id = 0;
    
    global = pthread_getspecific(tlsKey_threadInfo);
    
    if (global != NULL)
    {
        threadInfo = (mcsTHREAD_INFO*)global;
        id = threadInfo->id;
    }
    return id;
}

/**
 * Return the thread name.
 * \param thread name
 */
void mcsGetThreadName(mcsSTRING16* name)
{
    void* global;
    
    global = pthread_getspecific(tlsKey_threadInfo);
    
    if (global != NULL)
    {
        mcsTHREAD_INFO* threadInfo;
        threadInfo = (mcsTHREAD_INFO*)global;
        strncpy(*name, threadInfo->name, sizeof(mcsSTRING16) - 1);
    }
    else
    {
        strcpy(*name, "Main");
    }
}

/**
 * Define the thread identifier (positive integer >=1)
 * @param id thread identifier (positive integer >=1)
 */
void mcsSetThreadInfo(mcsUINT32 id, const mcsSTRING16 name)
{
    mcsTHREAD_INFO* threadInfo;
    threadInfo = malloc(sizeof(mcsTHREAD_INFO));
    
    threadInfo->id = id;
    strcpy(threadInfo->name, name);

    pthread_setspecific(tlsKey_threadInfo, threadInfo);
}

/**
 * Closes MCS services. 
 *
 * It performs the necessary clean up for applications making use of MCS.
 *
 * \warning Any application program, which has previously used mcsInit() must
 * call mcsExit() before terminating.
 *
 * \sa mcsInit
 */
void mcsExit()
{
    pthread_key_delete(tlsKey_threadInfo);
    
    if (domimpl != NULL)
    {
        /* free gdome implementation */ 
        GdomeException exc;
        gdome_di_unref(domimpl, &exc);
        domimpl = NULL;
    }
    
    /*
     * Cleanup function for the XML library (libxml2).
     * Library Clean up : must be called only once the process
     * has no more use of the XML library => main exit()
     * http://xmlsoft.org/html/libxml-parser.html
     * 
     * (valgrind check-mem)
     */
    xmlCleanupParser();    

    /* Store the application name */
    mcsStoreProcName(mcsUNKNOWN_PROC);
    mcsStoreEnvName(mcsUNKNOWN_ENV);
}

/*
 * Local functions
 */
/**
 * Stores the curent process name.
 *
 * Strips off from the process name the possible path, which could have been
 * specified to invoke the process.  For example  : "../bin/myProg" becomes
 * only "myProg". If the parameter given to mcsStoreProcName() is NULL, the name
 * of the process is set to mcsUNKNOWN_PROC.
 *
 * THIS FUNCTION IS FOR INTERNAL USE ONLY.
 *
 * \return mcsSUCCESS
 *
 * \sa mcsInit
 */
mcsCOMPL_STAT mcsStoreProcName (const char *procName)
{
    char *pchar, *path;
    pchar = (char *) procName;
    path  = strrchr(procName,'/');

    if (path != (char *) NULL)  
    { 
        pchar = path + 1; 
    }   

    strncpy((char *)mcsProcName, pchar, (sizeof(mcsProcName)-1));

    return mcsSUCCESS;
}

/**
 * Stores the curent environment name.
 *
 * If the parameter given to mcsStoreEnvName() is NULL, the name of the
 * environment is set to mcsUNKNOWN_ENV.
 *
 * THIS FUNCTION IS FOR INTERNAL USE ONLY.
 *
 * \return mcsSUCCESS
 *
 * \sa mcsInit
 */
mcsCOMPL_STAT mcsStoreEnvName (const char *envName)
{
    if (envName == (char *) NULL)
    {
        return mcsSUCCESS;
    }

    strncpy((char *)mcsEnvName, envName, (sizeof(mcsEnvName)-1));

    return mcsSUCCESS;
}

/**
 * Lock a mutex.
 *
 * If the mutex is already locked, then the caller is blocked until the mutex is
 * unlocked.
 *
 * @param mutex the mutex to lock
 *
 * @sa pthread_mutex_lock
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT mcsMutexLock(mcsMUTEX* mutex)
{
    /* Verify parameter vailidity */
    if (mutex == NULL)
    {
        return mcsFAILURE;
    }

    /* Lock the mutex */
    if (pthread_mutex_lock(mutex) != 0)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Unlock a mutex.
 *
 * @param mutex the mutex to unlock
 *
 * @sa pthread_mutex_unlock
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT mcsMutexUnlock(mcsMUTEX* mutex)
{
    /* Verify parameter vailidity */
    if (mutex == NULL)
    {
        return mcsFAILURE;
    }

    /* Unlock the mutex */
    if (pthread_mutex_unlock(mutex) != 0)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Lock global mutex to prevent concurrent access to Gdome stuff.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT mcsLockGdomeMutex(void)
{
    return mcsMutexLock(&gdomeMUTEX);
}
/**
 * Unlock global mutex to prevent concurrent access to Gdome stuff.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT mcsUnlockGdomeMutex(void)
{
    return mcsMutexUnlock(&gdomeMUTEX);
}

/**
 * Retrieves a C string containing the value of the environment variable whose name is specified as argument. 
 * If the requested variable is not part of the environment list, the function returns mcsFAILURE.
 * @param name environment variable name
 * @param buf buffer to store the value of the environment variable
 * @param buflen buffer length to avoid overflow
 * @return mcsSUCCESS or mcsFAILURE
 */
mcsCOMPL_STAT mcsGetEnv_r(const char *name, char *buf, const int buflen)
{
    int i, len, olen;
    char* envVar = NULL;
    int res = mcsFAILURE;
    
    len = strlen(name);

    if (mcsMutexLock(&env_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    for (i = 0; (envVar = environ[i]) != NULL; i++) {
        
        if ((strncmp(name, envVar, len) == 0) && (envVar[len] == '=')) {
            olen = strlen(&envVar[len+1]);
            if (olen >= buflen) {
                res = mcsFAILURE;
                goto cleanup;
            }
            strcpy(buf, &envVar[len+1]);
            res = mcsSUCCESS;
            
            goto cleanup;
        }
    }
    
cleanup:    
    if (mcsMutexUnlock(&env_mutex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }    

    return res;
}


/*___oOo___*/

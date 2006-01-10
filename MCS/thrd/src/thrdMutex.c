/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: thrdMutex.c,v 1.3 2006-01-10 14:40:39 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/12/19 16:46:30  lafrasse
 * Added log and corrected some typos
 *
 * Revision 1.1  2005/12/16 15:03:57  lafrasse
 * Added mutex support
 *
 ******************************************************************************/

/**
 * @file
 * pthread-based mutex library.
 * 
 * This library is simple. It allows you to:
 * @li create and destroy a mutex,
 * @li lock and unlock it.
 *
 * @n
 * @ex
 * A C program using a Dynamic Buffer.
 * @code
 * #include "thrdMutex.h"
 *
 * int main (int argc, char *argv[])
 * {
 *  /# Mutex creation #/
 *  thrdMUTEX myMutex;
 *  thrdMutexInit(&myMutex);
 *
 *  /# Lock the mutex #/
 *  thrdMutexLock(&myMutex);
 *
 *  /# Critical section code should go here... #/
 *
 *  /# Unlock the mutex #/
 *  thrdMutexUnlock(&myMutex);
 *
 *  /# Mutex destruction #/
 *  thrdMutexDestroy(&myMutex);
 *  exit();
 * }
 * @endcode
 *
 * @sa pthread
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: thrdMutex.c,v 1.3 2006-01-10 14:40:39 mella Exp $"; 



/* 
 * System Headers
 */
#include <errno.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/* 
 * Local Headers
 */
#include "thrdMutex.h"
#include "thrdPrivate.h"
#include "thrdErrors.h"


/*
 * Public functions definition
 */
/**
 * Initialize a new mutex.
 *
 * @warning The call to this function is MANDATORY for each new thrdMUTEX.
 *
 * @param mutex the mutex to initialize
 *
 * @sa pthread_mutex_init
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdMutexInit(thrdMUTEX *mutex)
{
    logTrace("thrdMutexInit()");

    /* Initialize the new mutex */
    if (pthread_mutex_init(mutex, NULL) != 0)
    {
        errAdd(thrdERR_MUTEX_INIT, mutex);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Destroy a mutex.
 *
 * @warning The call to this function is MANDATORY for each thrdMUTEX.
 *
 * @param mutex the mutex to destroy
 *
 * @sa pthread_mutex_destroy
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdMutexDestroy(thrdMUTEX *mutex)
{
    logTrace("thrdMutexLock()");

    /* Verify parameter vailidity */
    if (mutex == NULL)
    {
        errAdd(thrdERR_NULL_PARAM, "mutex");
        return mcsFAILURE;
    }

    /* Destroy the mutex */
    if (pthread_mutex_destroy(mutex) != 0)
    {
        /* If an eror occured, raise the corresponding error */
        switch (errno)
        {
            case EBUSY:
                /* the mutex is currently locked */
                errAdd(thrdERR_MUTEX_LOCKED);
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

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
mcsCOMPL_STAT thrdMutexLock(thrdMUTEX *mutex)
{
    logTrace("thrdMutexLock()");

    /* Verify parameter vailidity */
    if (mutex == NULL)
    {
        errAdd(thrdERR_NULL_PARAM, "mutex");
        return mcsFAILURE;
    }

    logDebug("thrdMutexLock() - waiting for the resource to be released.");

    /* Lock the mutex */
    if (pthread_mutex_lock(mutex) != 0)
    {
        /* If an eror occured, raise the corresponding error */
        switch (errno)
        {
            case EINVAL:
                /* The mutex has not been properly initialized */
                errAdd(thrdERR_MUTEX_NOT_INIT);
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    logDebug("thrdMutexLock() - resource acquired.");

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
mcsCOMPL_STAT thrdMutexUnlock(thrdMUTEX *mutex)
{
    logTrace("thrdMutexUnlock()");

    /* Verify parameter vailidity */
    if (mutex == NULL)
    {
        errAdd(thrdERR_NULL_PARAM, "mutex");
        return mcsFAILURE;
    }

    /* Unlock the mutex */
    if (pthread_mutex_unlock(mutex) != 0)
    {
        /* If an eror occured, raise the corresponding error */
        switch (errno)
        {
            case EINVAL:
                /* The mutex has not been properly initialized */
                errAdd(thrdERR_MUTEX_NOT_INIT);
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    logDebug("thrdMutexUnlock() - resource released.");

    return mcsSUCCESS;
}


/*___oOo___*/

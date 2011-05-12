/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * SystemV semaphores library.
 * 
 * This library is simple. It allows you to:
 * @li create and destroy a semaphore,
 * @li wait and signal it.
 *
 * @ex
 * A C program using a semaphore.
 * @code
 * #include "thrdSemaphore.h"
 *
 * int main (int argc, char *argv[])
 * {
 *  /# Semaphore creation #/
 *  thrdSEMAPHORE mySemaphore;
 *  thrdSemaphoreInit(&mySemaphore, 1);
 *
 *  /# Wait for the semaphore #/
 *  thrdSemaphoreWait(mySemaphore);
 *
 *  /# Critical section code should go here... #/
 *
 *  /# Signal the semaphore #/
 *  thrdSemaphoreSignal(mySemaphore);
 *
 *  /# Semaphore destruction #/
 *  thrdSemaphoreDestroy(mySemaphore);
 *  exit();
 * }
 * @endcode
 *
 * @warning if you get some 'ENOSPC' errors while trying to create new
 * semaphores, it is probably because you reached the maximun amount of
 * semaphores your OS can manage at once (maybe did you forget to call
 * thrdSemahoreDestroy() for each of your semaphore). @n
 * To remove previously unreleased semaphores, you can use the following command
 * in a Unix shell to remove any created semaphores (special thanx to Frederic
 * ROUSSEL) :
 * @code
 * for i in `ipcs -a|awk '{print $2}'`;do ipcrm -s$i;done
 * @endcode
 *
 * @sa ipcs ipcrm
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: thrdSemaphore.c,v 1.5 2007-02-09 17:02:38 lafrasse Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/* 
 * Local Headers
 */
#include "thrdSemaphore.h"
#include "thrdPrivate.h"
#include "thrdErrors.h"


/*
 * Public functions definition
 */
/**
 * Initialize a new semaphore.
 *
 * @warning The call to this function is MANDATORY for each new thrdSEMAPHORE.
 *
 * @param semaphore the semaphore to initialize
 * @param value the value to set
 *
 * @sa pthread_mutex_init
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreInit(thrdSEMAPHORE   *semaphore,
                                const mcsUINT32 value)
{
    logTrace("thrdSemaphoreInit()");

    /* Verify parameter vailidity */
    if (semaphore == NULL)
    {
        errAdd(thrdERR_NULL_PARAM, "semaphore");
        return mcsFAILURE;
    }

    /* Create one new semaphore */
    *semaphore = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT);
    if (*semaphore == -1)
    {
        const char *systemCall = "semget";

        /* If an error occured, raise it */
        switch (errno)
        {
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EEXIST:
                errAdd(thrdERR_ERRNO, systemCall, "EEXIST", strerror(errno));
                break;
    
            case ENOENT:
                errAdd(thrdERR_ERRNO, systemCall, "ENOENT", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case ENOMEM:
                errAdd(thrdERR_ERRNO, systemCall, "ENOMEM", strerror(errno));
                break;
    
            case ENOSPC:
                errAdd(thrdERR_ERRNO, systemCall, "ENOSPC", strerror(errno));
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    /* Set the new semaphore value */
    if (thrdSemaphoreSetValue(*semaphore, value) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Destroy a semaphore.
 *
 * @warning The call to this function is MANDATORY for each thrdSEMAPHORE.
 *
 * @param semaphore the semaphore to destroy
 *
 * @sa semctl
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreDestroy(const thrdSEMAPHORE   semaphore)
{
    logTrace("thrdSemaphoreDestroy()");

    union semun sem_union;

    /* Destroy the semaphore */
    if (semctl(semaphore, 0, IPC_RMID, sem_union) == -1)
    {
        const char *systemCall = "semctl";

        /* If an error occured, raise it */
        switch (errno)
        {
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EFAULT:
                errAdd(thrdERR_ERRNO, systemCall, "EFAULT", strerror(errno));
                break;
    
            case EIDRM:
                errAdd(thrdERR_ERRNO, systemCall, "EIDRM", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case EPERM:
                errAdd(thrdERR_ERRNO, systemCall, "EPERM", strerror(errno));
                break;
    
            case ERANGE:
                errAdd(thrdERR_ERRNO, systemCall, "ERANGE", strerror(errno));
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
 * Get a semaphore value.
 *
 * @param semaphore the semaphore
 * @param value the value of the semaphore
 *
 * @sa semctl
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreGetValue(const thrdSEMAPHORE semaphore,
                                    mcsUINT32           *value)
{
    logTrace("thrdSemaphoreGetValue()");

    /* Verify parameters vailidity */
    if (value == NULL)
    {
        errAdd(thrdERR_NULL_PARAM, "value");
        return mcsFAILURE;
    }

    union semun sem_union;

    /* Get the semaphore value */
    if (semctl(semaphore, 0, GETVAL, sem_union) == -1)
    {
        const char *systemCall = "semctl";

        /* If an error occured, raise it */
        switch (errno)
        {
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EFAULT:
                errAdd(thrdERR_ERRNO, systemCall, "EFAULT", strerror(errno));
                break;
    
            case EIDRM:
                errAdd(thrdERR_ERRNO, systemCall, "EIDRM", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case EPERM:
                errAdd(thrdERR_ERRNO, systemCall, "EPERM", strerror(errno));
                break;
    
            case ERANGE:
                errAdd(thrdERR_ERRNO, systemCall, "ERANGE", strerror(errno));
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    *value = sem_union.val;

    return mcsSUCCESS;
}

/**
 * Set a semaphore value.
 *
 * @param semaphore the semaphore to set
 * @param value the value to set
 *
 * @sa semctl
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreSetValue(const thrdSEMAPHORE semaphore,
                                    const mcsUINT32     value)
{
    logTrace("thrdSemaphoreSetValue()");

    /* Set the semaphore value */
    union semun sem_union;
    sem_union.val = value;
    if (semctl(semaphore, 0, SETVAL, sem_union) == -1)
    {
        const char *systemCall = "semctl";

        /* If an error occured, raise it */
        switch (errno)
        {
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EFAULT:
                errAdd(thrdERR_ERRNO, systemCall, "EFAULT", strerror(errno));
                break;
    
            case EIDRM:
                errAdd(thrdERR_ERRNO, systemCall, "EIDRM", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case EPERM:
                errAdd(thrdERR_ERRNO, systemCall, "EPERM", strerror(errno));
                break;
    
            case ERANGE:
                errAdd(thrdERR_ERRNO, systemCall, "ERANGE", strerror(errno));
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
 * Wait for a semaphore.
 *
 * If the semaphore is already locked, then the caller is blocked until the
 * semaphore is signaled.
 *
 * @param semaphore the semaphore to wait
 *
 * @sa semop
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreWait(const thrdSEMAPHORE semaphore)
{
    logTrace("thrdSemaphoreWait()");

    struct sembuf sem_b;

    logDebug("thrdSemaphoreWait() - waiting for the resource to be released.");

    /* Wait for the semaphore */
    sem_b.sem_num = 0;
    sem_b.sem_op  = -1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(semaphore, &sem_b, 1) == -1)
    {
        const char *systemCall = "semop";

        /* If an error occured, raise it */
        switch (errno)
        {
            case E2BIG:
                errAdd(thrdERR_ERRNO, systemCall, "E2BIG", strerror(errno));
                break;
    
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EAGAIN:
                errAdd(thrdERR_ERRNO, systemCall, "EAGAIN", strerror(errno));
                break;
    
            case EFAULT:
                errAdd(thrdERR_ERRNO, systemCall, "EFAULT", strerror(errno));
                break;
    
            case EFBIG:
                errAdd(thrdERR_ERRNO, systemCall, "EFBIG", strerror(errno));
                break;
    
            case EIDRM:
                errAdd(thrdERR_ERRNO, systemCall, "EIDRM", strerror(errno));
                break;
    
            case EINTR:
                errAdd(thrdERR_ERRNO, systemCall, "EINTR", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case ENOMEM:
                errAdd(thrdERR_ERRNO, systemCall, "ENOMEM", strerror(errno));
                break;
    
            case ERANGE:
                errAdd(thrdERR_ERRNO, systemCall, "ERANGE", strerror(errno));
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    logDebug("thrdSemaphoreWait() - resource acquired.");

    return mcsSUCCESS;
}

/**
 * Signal a semaphore.
 *
 * @param semaphore the semaphore to signal
 *
 * @sa pthread_mutex_unlock
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT thrdSemaphoreSignal(const thrdSEMAPHORE semaphore)
{
    logTrace("thrdSemaphoreSignal()");

    struct sembuf sem_b;

    /* Signal the semaphore */
    sem_b.sem_num = 0;
    sem_b.sem_op  = 1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(semaphore, &sem_b, 1) == -1)
    {
        const char *systemCall = "semop";

        /* If an error occured, raise it */
        switch (errno)
        {
            case E2BIG:
                errAdd(thrdERR_ERRNO, systemCall, "E2BIG", strerror(errno));
                break;
    
            case EACCES:
                errAdd(thrdERR_ERRNO, systemCall, "EACCES", strerror(errno));
                break;
    
            case EAGAIN:
                errAdd(thrdERR_ERRNO, systemCall, "EAGAIN", strerror(errno));
                break;
    
            case EFAULT:
                errAdd(thrdERR_ERRNO, systemCall, "EFAULT", strerror(errno));
                break;
    
            case EFBIG:
                errAdd(thrdERR_ERRNO, systemCall, "EFBIG", strerror(errno));
                break;
    
            case EIDRM:
                errAdd(thrdERR_ERRNO, systemCall, "EIDRM", strerror(errno));
                break;
    
            case EINTR:
                errAdd(thrdERR_ERRNO, systemCall, "EINTR", strerror(errno));
                break;
    
            case EINVAL:
                errAdd(thrdERR_ERRNO, systemCall, "EINVAL", strerror(errno));
                break;
    
            case ENOMEM:
                errAdd(thrdERR_ERRNO, systemCall, "ENOMEM", strerror(errno));
                break;
    
            case ERANGE:
                errAdd(thrdERR_ERRNO, systemCall, "ERANGE", strerror(errno));
                break;
    
            default:
                errAdd(thrdERR_ASSERT_FAILED);
                break;
        }

        return mcsFAILURE;
    }

    logDebug("thrdSemaphoreSignal() - resource released.");

    return mcsSUCCESS;
}


/*___oOo___*/

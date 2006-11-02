/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTHREAD.cpp,v 1.1 2006-11-02 07:40:19 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 *  Definition of thrdTHREAD class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: thrdTHREAD.cpp,v 1.1 2006-11-02 07:40:19 gzins Exp $"; 

/* 
 * System Headers 
 */
#include <stdio.h> 
#include <unistd.h>
#include <signal.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "thrdTHREAD.h"
#include "thrdPrivate.h"

/**
 * Class constructor
 */
thrdTHREAD::thrdTHREAD()
{
    _threadId  = 0;
    _threadArg = NULL;
}

/**
 * Class destructor
 */
thrdTHREAD::~thrdTHREAD()
{
}

/*
 * Public methods
 */
/**
 * Start execution of the thread.
 *
 * It creates a new thread that executes concurrently with the calling thread.
 * The new thread applies the method Execute() passing it \em arg as first
 * argument.
 *
 * @param arg argument passed to the thread when starts
 * 
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 *
 * @sa pthread_create man page
 */
mcsCOMPL_STAT thrdTHREAD::Start(const void *arg)
{
    SetArg(arg); // store user data
    if (pthread_create(&_threadId, NULL, thrdTHREAD::EntryPoint, this) == 0)
    {
        return mcsSUCCESS;
    }
    else
    {
        return mcsFAILURE;
    }
}

/**
 * Cancel thread execution.
 *
 * It terminates immediately the execution of the thread.
 *
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 *
 * @sa pthread_cancel man page
 */
mcsCOMPL_STAT thrdTHREAD::Cancel()
{
    if (pthread_cancel(_threadId)== 0)
    {
        return mcsSUCCESS;
    }
    else
    {
        return mcsFAILURE;
    }
}

/**
 * Wait for completion of the thread
 *
 * It suspends the execution of the calling thread until the thread identified
 * by th terminates, either by calling pthread_exit(3) or by being cancelled.
 *
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 *
 * @sa pthread_join man page
 */
mcsCOMPL_STAT thrdTHREAD::Join()
{
    if ((pthread_join (_threadId, NULL))== 0)
    {
        return mcsSUCCESS;
    }
    else
    {
        return mcsFAILURE;
    }
}

/*
 * Protected methods
 */
/**
 * Method which is called when starting thread execution
 *
 * This method is the entry point of the thread. By default, this method just
 * print out argument. It has to be overloaded in derivated class.
 *
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT thrdTHREAD::Execute(const void* arg)
{
    // Your code goes here
    printf("Thread execution - argument is '%s'\n", (char *)arg);

    return (mcsSUCCESS);
}

/*
 * Private methods
 */
/**
 * Set argument which will be passed to Execute() when the thread starts
 *
 * @param arg argument to be passed to Execute()
 */
void thrdTHREAD::SetArg(const void *arg)
{
    logTrace("thrdTHREAD::SetArg()");

    _threadArg = arg;
}

/**
 * Get argument which will be passed to Execute() when the thread starts
 *
 * @return argument to be passed to Execute().
 */
const void *thrdTHREAD::GetArg()
{
    return _threadArg;
}

/**
 * Static method calling Execute() method when starting thread.
 *
 * @param pthis pointer to thread derivated class instance.
 *
 * @return NULL.
 */
void *thrdTHREAD::EntryPoint(void * pthis)
{
    thrdTHREAD *thread = (thrdTHREAD*)pthis;
    thread->Execute(thread->GetArg());

    return (NULL);
}


/*___oOo___*/

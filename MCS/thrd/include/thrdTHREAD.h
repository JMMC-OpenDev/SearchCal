#ifndef thrdTHREAD_H
#define thrdTHREAD_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTHREAD.h,v 1.1 2006-11-02 07:40:11 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of thrdTHREAD class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System header
 */
#include <pthread.h>

/*
 * MCS header
 */
#include "mcs.h"


/*
 * Class declaration
 */

/**
 * thrdTHREAD - class to handle thread.
 * 
 * This class provides encapsulation of a thread of execution. The intention 
 * is for the user of the class to derive a new class overloading the 
 * Execute() method.
 */
class thrdTHREAD
{

public:
    // Class constructor
    thrdTHREAD();

    // Class destructor
    virtual ~thrdTHREAD();

    virtual mcsCOMPL_STAT Start(const void * arg);
    virtual mcsCOMPL_STAT Cancel();
    virtual mcsCOMPL_STAT Join();

protected:
    // Execute method; to be overloaded
    virtual mcsCOMPL_STAT Execute(const void *arg);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    thrdTHREAD(const thrdTHREAD&);
    thrdTHREAD& operator=(const thrdTHREAD&);

    // Set/Get user argument
    virtual void SetArg(const void *arg);
    virtual const void *GetArg();

    static void *EntryPoint(void *);

    pthread_t   _threadId;  /**< Identifier of the created thread */
    const void *_threadArg; /**< Argument passed to the thread when starts */
};

#endif /*!thrdTHREAD_H*/

/*___oOo___*/

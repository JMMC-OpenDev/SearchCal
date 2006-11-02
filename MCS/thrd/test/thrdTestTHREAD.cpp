/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTestTHREAD.cpp,v 1.1 2006-11-02 07:42:14 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 *
 * Test program for thrdTHREAD class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: thrdTestTHREAD.cpp,v 1.1 2006-11-02 07:42:14 gzins Exp $"; 

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "thrd.h"
#include "thrdPrivate.h"
#include "thrdTHREAD.h"


class thrdTEST_THREAD : public thrdTHREAD
{
public:
    thrdTEST_THREAD() {};
    virtual ~thrdTEST_THREAD() {};

    virtual mcsCOMPL_STAT Execute(const void *arg);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    thrdTEST_THREAD& operator=(const thrdTEST_THREAD&);
    thrdTEST_THREAD (const thrdTEST_THREAD&);

};
 
mcsCOMPL_STAT thrdTEST_THREAD::Execute(const void *arg)
{
    logTrace("thrdTEST_THREAD::Execute()");

    printf("Thread - argument is '%s'\n", (char *)arg);

    printf("Thread - wait 5 seconds\n");
    sleep(5);
    printf("Thread - done\n");

    return mcsSUCCESS;
}

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    thrdTEST_THREAD thread;

    // Start thread
    printf("Start thread execution\n");
    thread.Start("Hello world");

    // Wait for completion of the thread
    printf("Wait for completion of the thread\n");
    thread.Join();

     // Start thread
    printf("Start thread execution\n");
    thread.Start("Hello world");

    // Cancel thread
    printf("Cancel thread\n");
    thread.Cancel();

    exit(EXIT_SUCCESS);
}

/*___oOo___*/

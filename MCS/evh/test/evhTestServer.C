/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTestServer.C,v 1.1 2004-11-18 17:40:57 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     24-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Test program for evhKEY class.
 */

static char *rcsId="@(#) $Id: evhTestServer.C,v 1.1 2004-11-18 17:40:57 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#define MODULE_ID "myServer"
#include "evhCALLBACK.h"
#include "evhCMD_CALLBACK.h"
#include "evhIOSTREAM_CALLBACK.h"
#include "evhSERVER.h"
#include "evhKEY.h"
#include "evhCMD_KEY.h"
#include "evhIOSTREAM_KEY.h"

class evhTEST : public evhSERVER
{
public:
    evhTEST() {};
    virtual ~evhTEST(){};

    virtual evhCB_COMPL_STAT cb1 (msgMESSAGE &msg,void*)
    {
        printf("evhTEST::cb1()\n"); 
        msgSendReply(&msg, mcsTRUE);
        return evhCB_SUCCESS;
    };
    virtual evhCB_COMPL_STAT cb2 (const int,void*)
    {
        printf("evhTEST::cb2()\n"); 
        mcsSTRING80 msg;
        scanf("%s", msg);
        printf("Read string = %s\n", msg);
        errDisplayStack();
        return evhCB_SUCCESS;
    };
    virtual evhCB_COMPL_STAT VersionCB(msgMESSAGE &msg, void*)
    {
        logExtDbg("evhSERVER::VersionCB()");

        // Get the version string
        mcsSTRING256 version;
        strcpy(version,  GetSwVersion());

        // Set the reply buffer
        msgSetBody(&msg, version, strlen(version));

        // Send reply
        msgSendReply(&msg, mcsTRUE);

        return evhCB_NO_DELETE;
    }

protected:

private:
};


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    evhTEST myServer;

    logInfo("Server starting ...");

    // Init server
    if (myServer.Init(argc, argv) == FAILURE)
    {
        exit (EXIT_FAILURE);
    }

    // Attach callback to the SETUP command
    evhCMD_KEY key1("SETUP");
    evhCMD_CALLBACK cb1(&myServer, (evhCMD_CB_METHOD)&evhTEST::VersionCB);
    myServer.AddCallback(key1, cb1);

    // Attach callback to stdin 
    int fd;
    evhIOSTREAM_CALLBACK cb2(&myServer, (evhIOSTREAM_CB_METHOD)&evhTEST::cb2);
    fd = 0; 
    evhIOSTREAM_KEY key2(fd);
    myServer.AddCallback(key2, cb2);

    // Main loop
    if (myServer.MainLoop() == FAILURE)
    {
        errDisplayStack();
    }

    // Exit from the application with SUCCESS
    logInfo("Server exiting ...");
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

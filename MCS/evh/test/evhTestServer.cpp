/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTestServer.cpp,v 1.2 2004-12-08 17:58:42 gzins Exp $"
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

static char *rcsId="@(#) $Id: evhTestServer.cpp,v 1.2 2004-12-08 17:58:42 gzins Exp $"; 
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
    evhTEST() {i=0;};
    virtual ~evhTEST(){};

    virtual evhCB_COMPL_STAT Cb1 (msgMESSAGE &msg,void*)
    {
        printf("evhTEST::Cb1() : Command = %s, Params = %s\n",
               msg.GetCommand(), msg.GetBodyPtr()); 
        SendReply(msg);
        i++;
        printf("(i%2) = %d\n", (i%2)); 
        if ((i%2)==0)
        {
            return evhCB_SUCCESS | evhCB_DELETE;
        }
        else
        {
            return evhCB_SUCCESS;
        }
    };
    virtual evhCB_COMPL_STAT Cb2 (const int,void*)
    {
        printf("evhTEST::Cb2()\n"); 
        mcsSTRING80 msg;
        scanf("%s", msg);
        printf("Read string = %s\n", msg);
        errDisplayStack();
        return evhCB_SUCCESS | evhCB_DELETE;
    };

    virtual mcsCOMPL_STAT AppInit()
    {
        // Attach callback to the SETUP command
        evhCMD_KEY key1("SETUP");
        evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&evhTEST::Cb1);
 
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);
        AddCallback(key1, cb1);

        // Attach callback to stdin 
        int fd;
        evhIOSTREAM_CALLBACK cb2(this, (evhIOSTREAM_CB_METHOD)&evhTEST::Cb2);
        fd = 0; 
        evhIOSTREAM_KEY key2(fd);
        AddCallback(key2, cb2);
        return SUCCESS;
    }

protected:

private:
    int i;
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

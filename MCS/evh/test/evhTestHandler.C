/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTestHandler.C,v 1.1 2004-10-18 09:40:10 gzins Exp $"
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

static char *rcsId="@(#) $Id: evhTestHandler.C,v 1.1 2004-10-18 09:40:10 gzins Exp $"; 
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
#include "evhCALLBACK.h"
#include "evhCMD_CALLBACK.h"
#include "evhIOSTREAM_CALLBACK.h"
#include "evhHANDLER.h"
#include "evhKEY.h"
#include "evhCMD_KEY.h"
#include "evhIOSTREAM_KEY.h"

class evhTEST : public fndOBJECT
{
public:
    evhTEST() {};
    virtual ~evhTEST(){};

    virtual evhCB_COMPL_STAT cb1 (const msgMESSAGE &,void*)
    {
        printf("evhTEST::cb1()\n"); 
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

protected:

private:
};


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    logSetStdoutLogLevel(logEXTDBG);
    int fd;
    evhCMD_KEY key1("SETUP");
    evhTEST test;
    evhHANDLER evhHandler;
    evhCMD_CALLBACK cb1(&test, (evhCMD_CB_METHOD)&evhTEST::cb1);
    evhIOSTREAM_CALLBACK cb2(&test, (evhIOSTREAM_CB_METHOD)&evhTEST::cb2);

    msgConnect("evhTest", NULL);
    fd = 0; 

    evhIOSTREAM_KEY key2(fd);
    evhHandler.AddCallback(key1, cb1);
    evhHandler.AddCallback(key2, cb2);
    if (evhHandler.MainLoop() == FAILURE)
    {
        errDisplayStack();
    }
//    evhHandler.Run(key1, msg);
//    evhHandler.Run(key2, 1);
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

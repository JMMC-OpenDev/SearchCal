/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhTestServer.cpp,v 1.10 2006-05-11 13:04:25 mella Exp $"
 *
 * History:
 * --------
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2005/05/19 15:22:14  gzins
 * Updated to test message queue features
 *
 * Revision 1.8  2005/02/03 12:44:39  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.7  2005/01/29 20:18:20  gzins
 * Added errCloseStack when error occurs
 *
 * Revision 1.6  2005/01/29 07:24:48  gzins
 * Added CVS log as modification history
 *
 * gzins     24-Sep-2004  Created
 *
 *
 ******************************************************************************/

/**
 * \file
 * Test program for evhSERVER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: evhTestServer.cpp,v 1.10 2006-05-11 13:04:25 mella Exp $";

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
#include "evh.h"

class evhTEST : public evhSERVER
{
public:
    evhTEST();
    virtual ~evhTEST();

    virtual mcsCOMPL_STAT AppInit();
    virtual evhCB_COMPL_STAT SetupCB (msgMESSAGE &msg,void*);
    virtual evhCB_COMPL_STAT SetupReplyCB (msgMESSAGE &msg,void*);
    virtual evhCB_COMPL_STAT WaitCB (msgMESSAGE &msg,void*);
    virtual evhCB_COMPL_STAT StdinCB (const int,void*);
protected:

private:
    msgMESSAGE _msg;
    evhINTERFACE _msgManagerIf;
    evhINTERFACE _thisProcessIf;
};

// Constructor & destructor
evhTEST::evhTEST():_msgManagerIf("Message Manager", "msgManager"),
    _thisProcessIf("This process", "evhTestServer")
{
};
evhTEST::~evhTEST() 
{
};

// Application initialisation
mcsCOMPL_STAT evhTEST::AppInit()
{
    logExtDbg("evhTEST::AppInit()()"); 
    // Attach callback to the SETUP command
    evhCMD_KEY key1("SETUP");
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&evhTEST::SetupCB);
    AddCallback(key1, cb1);

     // Attach callback to the WAIT command
    evhCMD_KEY key2("WAIT");
    evhCMD_CALLBACK cb2(this, (evhCMD_CB_METHOD)&evhTEST::WaitCB);
    AddCallback(key2, cb2);

   // Attach callback to stdin 
    int fd;
    evhIOSTREAM_CALLBACK cb3(this, (evhIOSTREAM_CB_METHOD)&evhTEST::StdinCB);
    fd = 0; 
    evhIOSTREAM_KEY key3(fd);
    AddCallback(key3, cb3);
    return mcsSUCCESS;
}

// Callback for SETUP command
evhCB_COMPL_STAT evhTEST::SetupCB (msgMESSAGE &msg,void*)
{
    logExtDbg("evhTEST::SetupCB ()"); 
   
    msgMESSAGE *newMsg = new msgMESSAGE(msg);
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&evhTEST::SetupReplyCB, newMsg);
    _msgManagerIf.Forward("PING", "", cb1);

    return evhCB_SUCCESS;
};

// Callback for SETUP command reply
evhCB_COMPL_STAT evhTEST::SetupReplyCB (msgMESSAGE &, void*msg)
{
    logExtDbg("evhTEST::SetupReplyCB()"); 
    ((msgMESSAGE *)msg)->SetBody("SETUP done.");
    SendReply(*((msgMESSAGE *)msg));
    delete((msgMESSAGE *)msg);
    return evhCB_DELETE;
};

// Callback for WAIT command
evhCB_COMPL_STAT evhTEST::WaitCB (msgMESSAGE &msg,void*)
{
    logExtDbg("evhTEST::WaitCB ()"); 
    
    // Wait 5 seconds for the expected reply (it will never arrived because this
    // command is sent to the process itself).
    if (_thisProcessIf.Send("SETUP", "", 5000) == mcsFAILURE)
    {
        printf("Timeout expired ...\n");
    }
    SendReply(msg);

    return evhCB_SUCCESS;
};

// Callback for stdin
evhCB_COMPL_STAT evhTEST::StdinCB (const int,void*)
{
    logExtDbg("evhTEST::StdinCB()"); 
    mcsSTRING80 msg;
    scanf("%s", msg);
    printf("Read string = %s\n", msg);
    errCloseStack();
    return evhCB_SUCCESS;
};

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    evhTEST myServer;

    logInfo("Server starting ...");

    // Init server
    if (myServer.Init(argc, argv) == mcsFAILURE)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    // Main loop
    while (myServer.MainLoop() == mcsFAILURE)
    {
        errCloseStack();
    }

    // Exit from the application with mcsSUCCESS
    logInfo("Server exiting ...");
    exit (EXIT_SUCCESS);
}


/*___oOo___*/

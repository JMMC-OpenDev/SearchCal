/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.C,v 1.3 2004-11-23 09:15:46 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
* gzins     18-Nov-2004  Added PrintSynopsis, PrintArguments, ParseArguments,
*                        Connect, Disconnect and MainLoop methods
*                        Updated Init.
*
*******************************************************************************/

/**
 * \file
 * evhSERVER class definition.
 */

static char *rcsId="@(#) $Id: evhSERVER.C,v 1.3 2004-11-23 09:15:46 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
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
#include "evhSERVER.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */
evhSERVER::evhSERVER()
{
}

/*
 * Class destructor
 */
evhSERVER::~evhSERVER()
{
}

/**
 * Synopsys of the program.
 *
 * This method gives information about the synopsis of the program.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhSERVER::PrintSynopsis()
{
    std::cout << "Usage:" << Name() << " [OPTIONS] [<COMMAND> [<PARAMS>]]"<< endl;
    return SUCCESS;
}

/**
 * Usage of the specific arguments.
 *
 * This method gives information about the arguments of the program.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhSERVER::PrintArguments()
{
    cout <<" Argument        : COMMAND      name of command to be executed" 
        <<  endl;
    cout <<"                   PARAMS       command parameters" <<  endl;
    return SUCCESS;
}

mcsCOMPL_STAT evhSERVER::ParseArguments(mcsINT32 argc, mcsINT8 *argv[],
                                        mcsINT32 *optInd, mcsLOGICAL *optUsed)
{
    logExtDbg ("evhSERVER::ParseArguments ()");

    // If command name not yet set
    if (strlen (_msg.GetCommand()) == 0)
    {
        // If argument is an option; i.e. start by '-'
        if (argv[*optInd][0] == '-')
        {
            // Do not proces argument, and return
            *optUsed = mcsFALSE;
            return SUCCESS;
        }
        // End if

        // Set command name
        if (_msg.SetCommand(argv[*optInd]) == FAILURE)
        {
            return FAILURE;
        }
        return SUCCESS;
    }
    // Else command parameters not yet set
    else 
    {
        if (_msg.GetBodySize() == 0)
        {
            // Set command parameters 
            if (_msg.SetBody(argv[*optInd], strlen(argv[*optInd])) == FAILURE)
            {
                return FAILURE;
            }
            return SUCCESS;
        }
    }

    // Argument has not been processed
    *optUsed = mcsFALSE;

    return SUCCESS;
}
/**
 * Initialization of server.
 * It registers callback for VERSION command.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhSERVER::Init(mcsINT32 argc, char *argv[])
{
    logExtDbg("evhSERVER::Init()");

    logSetStdoutLogLevel(logQUIET);

    // Registers application to MCS services and parses the command-line
    // parameters
    if (evhTASK::Init(argc, argv) == FAILURE)
    {
        return (FAILURE);
    }

    evhCMD_KEY key("VERSION");
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&evhSERVER::VersionCB);
    AddCallback(key, cb);
    
    // If no command has been given in command-line arguments
    if (strlen(_msg.GetCommand()) == 0)
    {
        // Connection to message services
        if (Connect() == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

mcsCOMPL_STAT evhSERVER::Connect()
{
    logExtDbg("evhSERVER::Connect()");

    // Connect to message services
    if (_msgManager.Connect(Name(), NULL) == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

mcsCOMPL_STAT evhSERVER::Disconnect()
{
    logExtDbg("evhSERVER::Disconnect()");

    // Disconnect from message services
    if (_msgManager.Disconnect() == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}


mcsCOMPL_STAT evhSERVER::MainLoop(msgMESSAGE *msg)
{
    // If a message is given or no command given as argument 
    if ((msg != NULL) || strlen(_msg.GetCommand()) == 0)
    {
        // Enter in the event handler main loop
        return (evhHANDLER::MainLoop(msg));
    }
    // Else
    else
    {
        // Execute callback(s) associated to the command given as argument
        return (evhHANDLER::MainLoop(&_msg));
    }
    // End if
}

mcsCOMPL_STAT evhSERVER::SendReply(msgMESSAGE &msg, mcsLOGICAL lastReply)
{
    logExtDbg("evhSERVER::SendReply()");

    // If it is the command provided by user on command-line, just print out
    // the result
    if (&msg == &_msg)
    {
        if (errStackIsEmpty() == mcsTRUE)
        {
            printf("%s\n", msg.GetBodyPtr());
            return SUCCESS;
        }
        else
        {
            errDisplayStack();
            errCloseStack();
            return FAILURE;
        }
    }
    // Esle
    else
    {
        // Send reply to the sended
        return _msgManager.SendReply(msg, lastReply);
    }
}


/*___oOo___*/

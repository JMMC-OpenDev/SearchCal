/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.C,v 1.2 2004-11-18 17:39:03 gzins Exp $"
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

static char *rcsId="@(#) $Id: evhSERVER.C,v 1.2 2004-11-18 17:39:03 gzins Exp $"; 
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
    memset(&_msg, '\0', sizeof(msgMESSAGE)); 
    msgSetBody(&_msg, "", 0);
    _isConnected = mcsFALSE;
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
    if (strlen (msgGetCommand(&_msg)) == 0)
    {
        // If argument is an option; i.e. start by '-'
        if (argv[*optInd][0] == '-')
        {
            // Do not proces argument, and return
            *optUsed = mcsFALSE;
            return SUCCESS;
        }
        // End if

        // Get command name
        strncpy(_msg.header.command, argv[*optInd], sizeof(mcsCMD));
        return SUCCESS;
    }
    // Else command parameters not yet set
    else 
    {
        if (msgGetBodySize(&_msg) == 0)
        {
            if (msgSetBody(&_msg, argv[*optInd], strlen(argv[*optInd])) == FAILURE)
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
    if (strlen(msgGetCommand(&_msg)) == 0)
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
    if (msgConnect(Name(), NULL) == FAILURE)
    {
        return FAILURE;
    }
    _isConnected = mcsTRUE;

    return SUCCESS;
}

mcsCOMPL_STAT evhSERVER::Disconnect()
{
    logExtDbg("evhSERVER::Disconnect()");

    // If not connected, return
    if (_isConnected == mcsFALSE)
    {
        return SUCCESS;
    }
    
    // Disconnect from message services
    if (msgDisconnect() == FAILURE)
    {
        return FAILURE;
    }
    _isConnected = mcsFALSE;

    return SUCCESS;
}


mcsCOMPL_STAT evhSERVER::MainLoop(msgMESSAGE *msg)
{
    // If a message is given or no command given as argument 
    if ((msg != NULL) || strlen(msgGetCommand(&_msg)) == 0)
    {
        // Enter in the event handler main loop
        return (evhHANDLER::MainLoop(msg));
    }
    // Else
    else
    {
        // Execute callback(s) associated to the command given as argument
        if (evhHANDLER::MainLoop(&_msg) == SUCCESS)
        {
            if (errStackIsEmpty() == mcsTRUE)
            {
                printf("%s\n", msgGetBodyPtr(&_msg));
                return SUCCESS;
            }
            else
            {
                errDisplayStack();
                errCloseStack();
                return FAILURE;
            }
        }
        else
        {
            return FAILURE;
        }
    }
    // End if
}

/*___oOo___*/

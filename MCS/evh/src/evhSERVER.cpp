/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.cpp,v 1.4 2005-01-07 18:22:52 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
* gzins     18-Nov-2004  Added PrintSynopsis, PrintArguments, ParseArguments,
*                        Connect, Disconnect and MainLoop methods
*                        Updated Init.
* gzins     03-Dec-2004  Added -n command-line option  
* gzins     22-Dec-2004  Attached callback for HELP command
*                        Replaced GetBodyPtr by GetBody
* gzins     07-Jan-2005  Changed SUCESS/FAILURE to mcsSUCCESS/mcsFAILURE
*                        Removed SendCommand()
*                        Added some method documentation
*
*******************************************************************************/

/**
 * \file
 * Definition of the evhSERVER class.
 */

static char *rcsId="@(#) $Id: evhSERVER.cpp,v 1.4 2005-01-07 18:22:52 gzins Exp $"; 
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
#include "evhVERSION_CMD.h"
#include "evhHELP_CMD.h"
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
 * \return mcsSUCCESS 
 */
mcsCOMPL_STAT evhSERVER::PrintSynopsis()
{
    std::cout << "Usage:" << Name() << " [OPTIONS] [<COMMAND> [<PARAMS>]]"<< endl;
    return mcsSUCCESS;
}

/**
 * Usage of the specific arguments.
 *
 * This method gives information about the arguments of the program.
 *
 * \return mcsSUCCESS 
 */
mcsCOMPL_STAT evhSERVER::PrintArguments()
{
    cout <<" Argument        : COMMAND      name of command to be executed" 
        <<  endl;
    cout <<"                   PARAMS       command parameters" <<  endl;
    return mcsSUCCESS;
}

/**
 * Parse the specific arguments.
 *
 * This method parses the arguments (comand-line arguments which have not been
 * identified as an option). The recognized arguments are the command and its
 * associated parameters.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT evhSERVER::ParseArguments(mcsINT32 argc, char *argv[],
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
            return mcsSUCCESS;
        }
        // End if

        // Set command name
        if (_msg.SetCommand(argv[*optInd]) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        return mcsSUCCESS;
    }
    // Else 
    else 
    {
        if (_msg.GetBodySize() == 0)
        {
            // Set command parameters 
            if (_msg.SetBody(argv[*optInd], strlen(argv[*optInd])) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            return mcsSUCCESS;
        }
    }

    // Argument has not been processed
    *optUsed = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * Initialization of server.
 * It registers callback for VERSION command.
 *
 * \return mcsSUCCESS 
 */
mcsCOMPL_STAT evhSERVER::Init(mcsINT32 argc, char *argv[])
{
    logExtDbg("evhSERVER::Init()");

    logSetStdoutLogLevel(logQUIET);

    // Registers application to MCS services and parses the command-line
    // parameters
    if (evhTASK::Init(argc, argv) == mcsFAILURE)
    {
        return (mcsFAILURE);
    }

    // Add callback to VERSION command
    evhCMD_KEY key(evhVERSION_CMD_NAME, evhVERSION_CDF_NAME);
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&evhSERVER::VersionCB);
    AddCallback(key, cb);
    
    // Add callback to HELP command
    key.SetCommand(evhHELP_CMD_NAME);
    key.SetCdf(evhHELP_CDF_NAME);
    cb.SetMethod((evhCMD_CB_METHOD)&evhSERVER::HelpCB);
    AddCallback(key, cb);
    //
    // If no command has been given in command-line arguments
    if (strlen(_msg.GetCommand()) == 0)
    {
        // Connection to message services
        if (Connect() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * Connection to the MCS message service.
 *
 * Establish the connection to the MCS message service.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT evhSERVER::Connect()
{
    logExtDbg("evhSERVER::Connect()");

    // Connect to message services
    if (_msgManager.Connect(Name()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Close connection to the MCS message service.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT evhSERVER::Disconnect()
{
    logExtDbg("evhSERVER::Disconnect()");

    // Disconnect from message services
    if (_msgManager.Disconnect() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
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

/**
 * Send a reply message.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current message is the last one or
 * not
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT evhSERVER::SendReply(msgMESSAGE &msg, mcsLOGICAL lastReply)
{
    logExtDbg("evhSERVER::SendReply()");

    // If it is the command provided by user on command-line, just print out
    // the result
    if (&msg == &_msg)
    {
        if (errStackIsEmpty() == mcsTRUE)
        {
            printf("%s\n", msg.GetBody());
            return mcsSUCCESS;
        }
        else
        {
            errDisplayStack();
            errCloseStack();
            return mcsFAILURE;
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

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.cpp,v 1.3 2004-12-22 09:02:01 gzins Exp $"
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
*
*******************************************************************************/

/**
 * \file
 * evhSERVER class definition.
 */

static char *rcsId="@(#) $Id: evhSERVER.cpp,v 1.3 2004-12-22 09:02:01 gzins Exp $"; 
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
        if (Connect() == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * Connection to the MCS message service.
 *
 * It registers callback for VERSION command.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhSERVER::Connect()
{
    logExtDbg("evhSERVER::Connect()");

    // Connect to message services
    if (_msgManager.Connect(Name()) == FAILURE)
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

/**
 * Send a command message to a process.
 *
 * Send the \<command\>  to the \<destProc\> named process. The command
 * parameters (if any) has to be given in \<paramList\>. The parameter list
 * length can be specified using \<paramLen\>, if it is not given then the
 * length of the parameter list string is used.
 *
 * \param command command name
 * \param destProc remote process name
 * \param paramList parameter list stored in a string
 * \param paramsLen length of the parameter list string
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT evhSERVER::SendCommand(const char        *command,
                                     const mcsPROCNAME  destProc,
                                     const char        *paramList,  
                                     mcsINT32           paramsLen)
{
    logExtDbg("evhSERVER::SendCommand()");

    return _msgManager.SendCommand(command, destProc, paramList, paramsLen);
}
 
/**
 * Send a reply message.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current message is the last one or
 * not
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
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

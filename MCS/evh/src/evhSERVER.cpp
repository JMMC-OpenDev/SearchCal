/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhSERVER.cpp,v 1.8 2005-01-29 20:52:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/01/29 20:14:52  gzins
 * Added unique parameter to Connect() method
 *
 * Revision 1.6  2005/01/29 15:15:00  gzins
 * Attached callback for DEBUG command.
 *
 * Revision 1.5  2005/01/26 18:19:25  gzins
 * Implement methods related to state/sub-state handling
 * Attached callback for STATE command
 *
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
 ******************************************************************************/

/**
 * \file
 * Definition of the evhSERVER class.
 */

static char *rcsId="@(#) $Id: evhSERVER.cpp,v 1.8 2005-01-29 20:52:00 gzins Exp $"; 
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
#include "evhDEBUG_CMD.h"
#include "evhHELP_CMD.h"
#include "evhSTATE_CMD.h"
#include "evhErrors.h"
#include "evhPrivate.h"

/**
 * Class constructor
 *
 * \param unique  if mcsTRUE, only one instance will be allowed to be
 * connected to the message service. This is the default behaviour for server
 * process. 
 */
evhSERVER::evhSERVER(mcsLOGICAL unique)
{
    // Add state definitions
    AddState(evhSTATE_UNKNOWN, evhSTATE_STR_UNKNOWN);
    AddState(evhSTATE_OFF, evhSTATE_STR_OFF);
    AddState(evhSTATE_STANDBY, evhSTATE_STR_STANDBY);
    AddState(evhSTATE_ONLINE, evhSTATE_STR_ONLINE);

    // Add sub-state definitions
    AddSubState(evhSUBSTATE_UNKNOWN, evhSUBSTATE_STR_UNKNOWN);
    AddSubState(evhSUBSTATE_ERROR, evhSUBSTATE_STR_ERROR);
    AddSubState(evhSUBSTATE_IDLE, evhSUBSTATE_STR_IDLE);
    AddSubState(evhSUBSTATE_WAITING, evhSUBSTATE_STR_WAITING);
    AddSubState(evhSUBSTATE_BUSY, evhSUBSTATE_STR_BUSY);

    // Unicity flag
    _unique = unique;
}

/*
 * Class destructor
 */
evhSERVER::~evhSERVER()
{
    _stateList.clear();
    _subStateList.clear();
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
            if (_msg.SetBody(argv[*optInd], 
                             strlen(argv[*optInd])) == mcsFAILURE)
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
 * 
 * It registers callbacks for DEBUG, HELP, STATE and VERSION commands. Then, it
 * establishes connection with the message servivices (only if the command has
 * not been given on the command-line). When completed, the state and sub-state
 * are set to ONLINE/IDLE.
 *
 * \return mcsSUCCESS on successful completion or mcsFAILURE otherwise. 
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
    
    // Add callback to DEBUG command
    key.SetCommand(evhDEBUG_CMD_NAME);
    key.SetCdf(evhDEBUG_CDF_NAME);
    cb.SetMethod((evhCMD_CB_METHOD)&evhSERVER::DebugCB);
    AddCallback(key, cb);
    
    // Add callback to HELP command
    key.SetCommand(evhHELP_CMD_NAME);
    key.SetCdf(evhHELP_CDF_NAME);
    cb.SetMethod((evhCMD_CB_METHOD)&evhSERVER::HelpCB);
    AddCallback(key, cb);
    
    // Add callback to STATE command
    key.SetCommand(evhSTATE_CMD_NAME);
    key.SetCdf(evhSTATE_CDF_NAME);
    cb.SetMethod((evhCMD_CB_METHOD)&evhSERVER::StateCB);
    AddCallback(key, cb);
    
    // If no command has been given in command-line arguments
    if (strlen(_msg.GetCommand()) == 0)
    {
        // Connection to message services
        if (Connect() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Set state/sub-state to ONLINE/IDLE
    SetState(evhSTATE_ONLINE);
    SetSubState(evhSUBSTATE_IDLE);

    return mcsSUCCESS;
}

/**
 * Set server state
 *
 * \return mcsSUCCESS
 */
mcsCOMPL_STAT evhSERVER::SetState(mcsINT32 state)
{
    logExtDbg("evhSERVER::SetState()");

    _state = state;

    return SUCCESS;
}
/**
 * Get server state
 *
 * \return server state
 */
mcsINT32 evhSERVER::GetState(void)
{
    logExtDbg("evhSERVER::GetState()");

    return _state;
}

/**
 * Get server state as string
 *
 * \return server state
 */
const char *evhSERVER::GetStateStr(void)
{
    logExtDbg("evhSERVER::GetStateStr()");
    
    // If state is not defined
    map<mcsINT32, string> ::iterator iterator;
    iterator = _stateList.find(_state);
    if (iterator == _stateList.end())
    {
        // Return 'UNKNOWN'
        return evhSTATE_STR_UNKNOWN;
    }
    // Else
    else
    {
        // Return string corresponding to the current state
        return (iterator->second.c_str());
    }
    // End if
}

/**
 * Set server sub-state
 *
 * \return mcsSUCCESS
 */
mcsCOMPL_STAT evhSERVER::SetSubState(mcsINT32 subState)
{
    logExtDbg("evhSERVER::SetSubState()");

    _subState = subState;

    return SUCCESS;
}

/**
 * Get server sub-state
 *
 * \return server sub-state
 */
mcsINT32 evhSERVER::GetSubState(void)
{
    logExtDbg("evhSERVER::GetSubState()");

    return _subState;
}

/**
 * Get server sub-state as string
 *
 * \return server sub-state
 */
const char *evhSERVER::GetSubStateStr(void)
{
    logExtDbg("evhSERVER::GetSubStateStr()");
    
    // If state is not defined
    map<mcsINT32, string> ::iterator iterator;
    iterator = _subStateList.find(_subState);
    if (iterator == _subStateList.end())
    {
        // Return 'UNKNOWN'
        return evhSUBSTATE_STR_UNKNOWN;
    }
    // Else
    else
    {
        // Return string corresponding to the current sub-state
        return (iterator->second.c_str());
    }
    // End if
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
    if (_msgManager.Connect(Name(), _unique) == mcsFAILURE)
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

/* 
 * Protected methods
 */
/**
 * Add a server state in the state definition list.
 *
 * \param id   state identifier
 * \param name state name
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 *
 * \b Error codes:\n
 * The possible error is :
 * \li evhERR_DUPLICATED_STATE
 */
mcsCOMPL_STAT evhSERVER::AddState(mcsINT32  id, char *name)
{
    logExtDbg("evhSERVER::AddState()");

    if (_stateList.find(id) != _stateList.end())
    {
        errAdd(evhERR_DUPLICATED_STATE, id, name);
        return mcsFAILURE;
    }
    _stateList[id] = name;

    return SUCCESS;
}

/**
 * Add a server sub-state in the state definition list.
 *
 * \param id   sub-state identifier
 * \param name sub-state name
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 *
 * \b Error codes:\n
 * The possible error is :
 * \li evhERR_DUPLICATED_SUBSTATE
 */
mcsCOMPL_STAT evhSERVER::AddSubState(mcsINT32 id, char *name)
{
    logExtDbg("evhSERVER::AddSubState()");

    if (_subStateList.find(id) != _subStateList.end())
    {
        errAdd(evhERR_DUPLICATED_SUBSTATE, id, name);
        return mcsFAILURE;
    }
    _subStateList[id] = name;

    return SUCCESS;
}


/*___oOo___*/

#ifndef evhSERVER_H
#define evhSERVER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of the evhSERVER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
#include <map>

#include "evhTASK.h"
#include "evhHANDLER.h"
#include "evhStates.h"

/*
 * Class declaration
 */

/**
 * To be written. 
 * 
 */
class evhSERVER : public evhTASK, public evhHANDLER
{
public:
    evhSERVER(mcsLOGICAL unique=mcsTRUE);
    virtual ~evhSERVER();

    // Usage of the application 
    virtual mcsCOMPL_STAT PrintSynopsis();
    virtual mcsCOMPL_STAT PrintArguments();

    // Parse command-line arguments 
    virtual mcsCOMPL_STAT ParseArguments(mcsINT32 argc, char *argv[],
                                         mcsINT32 *optInd,
                                         mcsLOGICAL *optUsed);

    // Init method
    virtual mcsCOMPL_STAT AddionalInit();

    // Server state and sub-state
    virtual mcsCOMPL_STAT SetState(mcsINT32 state);
    virtual mcsINT32      GetState(void);
    virtual const char   *GetStateStr(void);
    virtual mcsCOMPL_STAT SetSubState(mcsINT32 subState);
    virtual mcsINT32      GetSubState(void);
    virtual const char   *GetSubStateStr(void);
    
    // Command callbacks
    virtual evhCB_COMPL_STAT DebugCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT HelpCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT StateCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT VersionCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT ExitCB(msgMESSAGE &msg, void*);

    // Connection to MCS message manager
    virtual mcsCOMPL_STAT Connect();
    virtual mcsCOMPL_STAT Disconnect();

    // Main loop
    virtual mcsCOMPL_STAT MainLoop(msgMESSAGE *msg=NULL);

    // Send reply
    virtual mcsCOMPL_STAT SendReply(msgMESSAGE &msg, 
                                    mcsLOGICAL lastReply=mcsTRUE);

protected:
    // Method to add a new state in the state definition list.
    mcsCOMPL_STAT AddState(mcsINT32 id, char *name);

    // Method to add a new state in the state definition list.
    mcsCOMPL_STAT AddSubState(mcsINT32 id, char *name);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhSERVER& operator=(const evhSERVER&);
    evhSERVER (const evhSERVER&);

    // Interface to msgManager process
    msgMANAGER_IF   _msgManager;

    // Command given as command-line argument (is any)
    msgMESSAGE _msg;

    // Current server state and sub-state
    mcsINT32 _state;
    mcsINT32 _subState;

    // List of defined states with the corresponding name
    map<mcsINT32, string> _stateList;
    map<mcsINT32, string> _subStateList;

    // Unicity flag; i.e only one or more server instances allowed
    mcsLOGICAL _unique;
};

#endif /*!evhSERVER_H*/

/*___oOo___*/

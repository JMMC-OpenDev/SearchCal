#ifndef evhSERVER_H
#define evhSERVER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhSERVER.h,v 1.9 2005-01-29 20:14:52 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/01/29 15:15:23  gzins
 * Attached callback for DEBUG command.
 *
 * Revision 1.7  2005/01/26 18:09:49  gzins
 * Added methods related to state and sub-state hanlding
 *
 * gzins     09-Nov-2004  Created
 * gzins     23-Nov-2004  Used new msg C++ library.
 *                        Added SendReply method
 * gzins     03-Dec-2004  Added SendCommand
 * gzins     22-Dec-2004  Added HelpCB()
 * gzins     07-Jan-2005  Remove SendCommand(); moved to evhINTERFACE 
 *
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
    virtual mcsCOMPL_STAT Init(mcsINT32 argc, char *argv[]);

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

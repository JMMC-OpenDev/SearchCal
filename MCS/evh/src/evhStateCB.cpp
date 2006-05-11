/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhStateCB.cpp,v 1.3 2006-05-11 13:04:18 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/03 06:54:42  gzins
 * Removed useless SendReply when error occurs (now done automatically by the evhHandler).
 *
 * Revision 1.1  2005/01/26 18:19:54  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of StateCB callback.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: evhStateCB.cpp,v 1.3 2006-05-11 13:04:18 mella Exp $";
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
#include "evhSTATE_CMD.h"
#include "evhPrivate.h"

/**
 * Callback method for STATE command.
 * 
 * It returns the state and the sub-state of the server. The reply format is:\n
 * \<state>/\<substate>
 *
 * \return evhCB_COMPL_STAT.
 */
evhCB_COMPL_STAT evhSERVER::StateCB(msgMESSAGE &msg, void*)
{
    logExtDbg("evhSERVER::HelpCB()");

    // Parse command parameters
    evhSTATE_CMD stateCmd(msg.GetCommand(), msg.GetBody());
    if (stateCmd.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE|evhCB_FAILURE;
    }

    // Get state and sub-state of server and prepare reply
    mcsSTRING256 reply;
    sprintf(reply, "%s/%s", GetStateStr(), GetSubStateStr());
    if (msg.SetBody(reply) == mcsFAILURE)
    {
        return (evhCB_NO_DELETE|evhCB_FAILURE);
    }

    // Send reply
    SendReply(msg);

    return evhCB_NO_DELETE;
}

/*___oOo___*/

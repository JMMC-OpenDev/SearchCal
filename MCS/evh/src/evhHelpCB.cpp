/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhHelpCB.cpp,v 1.1 2004-12-22 08:57:07 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Definition of the VERSION callback.
 */

static char *rcsId="@(#) $Id: evhHelpCB.cpp,v 1.1 2004-12-22 08:57:07 gzins Exp $"; 
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

/**
 * Callback method for HELP command.
 * 
 * It returns a short description of the commands supported by the
 * application, or a description of a given command. It recognizes the
*  following command parameter:
*       -command <command>  specifies the command name to get detailed help on

 *
 * \return evhCB_NO_DELETE.
 */
evhCB_COMPL_STAT evhSERVER::HelpCB(msgMESSAGE &msg, void*)
{
    logExtDbg("evhSERVER::HelpCB()");

    // Get help 
    if (GetHelp(msg) == FAILURE)
    {
        SendReply(msg);
        return (evhCB_NO_DELETE|evhCB_FAILURE);
    }
    
    // Send reply
    SendReply(msg);

    return evhCB_NO_DELETE;
}

/*___oOo___*/

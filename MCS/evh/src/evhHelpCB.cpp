/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhHelpCB.cpp,v 1.2 2005-01-07 18:19:24 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
* gzins     07-Jan-2005  Changed SUCESS/FAILURE to mcsSUCCESS/mcsFAILURE
*                        Small changes in documentation
*******************************************************************************/

/**
 * \file
 * Definition of the VERSION callback.
 */

static char *rcsId="@(#) $Id: evhHelpCB.cpp,v 1.2 2005-01-07 18:19:24 gzins Exp $"; 
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
 *   \li command \em &lt;command&gt;  specifies the command name to get detailed
 *        help on
 *
 * \return evhCB_COMPL_STAT.
 */
evhCB_COMPL_STAT evhSERVER::HelpCB(msgMESSAGE &msg, void*)
{
    logExtDbg("evhSERVER::HelpCB()");

    // Get help 
    if (HandeHelpCmd(msg) == mcsFAILURE)
    {
        SendReply(msg);
        return (evhCB_NO_DELETE|evhCB_FAILURE);
    }
    
    // Send reply
    SendReply(msg);

    return evhCB_NO_DELETE;
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhVersionCB.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"
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

static char *rcsId="@(#) $Id: evhVersionCB.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"; 
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
 * Callback method for VERSION command.
 * 
 * It returns the current version of the software module.
 *
 * \return evhCB_NO_DELETE.
 */
evhCB_COMPL_STAT evhSERVER::VersionCB(msgMESSAGE &msg, void*)
{
    logExtDbg("evhSERVER::VersionCB()");

    // Get the version string
    mcsSTRING256 version;
    strcpy(version,  GetSwVersion());

    // Set the reply buffer
    msg.SetBody(version, strlen(version));

    // Send reply
    SendReply(msg);

    return evhCB_NO_DELETE;
}

/*___oOo___*/

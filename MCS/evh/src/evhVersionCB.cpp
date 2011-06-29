/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of the VERSION callback.
 */

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
    strcpy(version, GetSwVersion());

    // Set the reply buffer
    msg.SetBody(version);

    // Send reply
    SendReply(msg);

    return evhCB_NO_DELETE;
}

/*___oOo___*/

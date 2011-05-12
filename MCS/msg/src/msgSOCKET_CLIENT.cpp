/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Interface class providing client-side specialized socket functionnalities.
 *
 * \sa msgSOCKET_CLIENT
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgSOCKET_CLIENT.cpp,v 1.11 2006-05-11 13:04:56 mella Exp $";

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
#include "msgSOCKET_CLIENT.h"
#include "msgPrivate.h"

/**
 * Class constructor
 */
msgSOCKET_CLIENT::msgSOCKET_CLIENT()
{
}

/**
 * Class destructor
 */
msgSOCKET_CLIENT::~msgSOCKET_CLIENT()
{
}

/*
 * Public methods
 */

/**
 * Create and connect a new socket to the given host name and port number.
 *
 * \param host the remote machine host name to which the socket should connect
 * \param port the remote machine port number to which the socket should connect
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgSOCKET_CLIENT::Open(std::string host, mcsUINT16 port)
{
    logExtDbg("msgSOCKET_CLIENT::Open()");

    // Try to create a new socket
    if (Create() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Try to connect the new socket to the remote host and port
    if (Connect(host, port) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
        
    return mcsSUCCESS;
}

/*___oOo___*/

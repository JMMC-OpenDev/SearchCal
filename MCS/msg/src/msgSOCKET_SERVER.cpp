/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Interface class providing server-side specialized socket functionnalities.
 *
 * \sa msgSOCKET_SERVER
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
#include "msgSOCKET.h"
#include "msgSOCKET_SERVER.h"
#include "msgPrivate.h"

/*
 * Class constructor
 */
msgSOCKET_SERVER::msgSOCKET_SERVER()
{
}

/*
 * Class destructor
 */
msgSOCKET_SERVER::~msgSOCKET_SERVER()
{
}


/*
 * Public methods
 */

/**
 * Create a new socket, bind it on the given port number and start listening.
 *
 * \param port the local port number on which the socket should listen
 *
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET_SERVER::Open(mcsUINT16 port)
{
    // Create a new socket
    if (Create() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Bind the new socket to the given port number
    if (Bind(port) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Start listening the network with the new socket
    if (Listen() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

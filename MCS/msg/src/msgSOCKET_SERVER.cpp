/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_SERVER.cpp,v 1.5 2005-01-07 18:38:46 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
* lafrasse  23-Nov-2004  Comment refinments, and includes cleaning
* lafrasse  03-Dec-2004  Changed port number type from mcsINT32 to mcsUINT16
* gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET_SERVER class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET_SERVER.cpp,v 1.5 2005-01-07 18:38:46 gzins Exp $"; 
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
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise
 */
mcsCOMPL_STAT msgSOCKET_SERVER::Open(mcsUINT16 port)
{
    logExtDbg("msgSOCKET_SERVER::Open()");

    // Try to create a new socket
    if (Create() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Try to bind the new socket to the given port number
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

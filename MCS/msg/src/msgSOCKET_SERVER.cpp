/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_SERVER.cpp,v 1.1 2004-11-23 08:25:52 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET_SERVER class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET_SERVER.cpp,v 1.1 2004-11-23 08:25:52 scetre Exp $"; 
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
#include "msgMESSAGE.h"
#include "msgSOCKET_SERVER.h"
#include "msgPrivate.h"

/*
 * Class constructor
 */



/*
 * Class destructor
 */



/*
 * Public methods
 */
mcsCOMPL_STAT msgSOCKET_SERVER::Open(std::string host,
                   mcsINT32 port)
{
    // Create the socket
    if (Create() == FAILURE)
    {
        return FAILURE;
    }
    // Bind the socket on a specific port
    if (Bind(port) == FAILURE)
    {
        return FAILURE;
    }
    // Listen the socket
    if (Listen() == FAILURE)
    {
        return FAILURE;
    }
        
    return SUCCESS;
}



/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

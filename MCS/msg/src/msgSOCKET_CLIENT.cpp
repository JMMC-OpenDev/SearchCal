/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_CLIENT.cpp,v 1.1 2004-11-23 08:25:52 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET_CLIENT class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET_CLIENT.cpp,v 1.1 2004-11-23 08:25:52 scetre Exp $"; 
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
#include "msgSOCKET_CLIENT.h"
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

/**
 * Open a socket 
 *
 * @param host 
 * @param port the port where the socket will be bind
 *
 * @return SUCCESS on successfull completion otherwise FAILURE is return 
 **/
mcsCOMPL_STAT msgSOCKET_CLIENT::Open(std::string host,
                   mcsINT32 port)
{
    // Create the socket
    if (Create()==FAILURE)
    {
        return FAILURE;
    }
    // Connect the socket
    if (Connect(host, port)==FAILURE)
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

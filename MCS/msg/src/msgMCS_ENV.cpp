/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMCS_ENV.cpp,v 1.1 2004-12-03 17:05:50 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgMCS_ENV class definition.
 */

static char *rcsId="@(#) $Id: msgMCS_ENV.cpp,v 1.1 2004-12-03 17:05:50 lafrasse Exp $"; 
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
#include "misc.h"


/*
 * Local Headers 
 */
#include "msgMCS_ENV.h"
#include "msgPrivate.h"

/*
 * Class constructor
 */
msgMCS_ENV::msgMCS_ENV()
{
    _initialized = mcsFALSE;

    memset(_hostName, 0, sizeof(_hostName));

    _portNumber = 0;
}


/*
 * Class destructor
 */
msgMCS_ENV::~msgMCS_ENV()
{
    _initialized = mcsFALSE;

    memset(_hostName, 0, sizeof(_hostName));

    _portNumber = 0;
}


/*
 * Public methods
 */
/**
 * Return the msgManager host name.
 *
 * \return the address of the host name, or NULL if an error occured
 */
const char* msgMCS_ENV::GetHostName(void)
{
    logExtDbg("msgMCS_ENV::GetHostName()");

    // If the host name and the port number have not been retrieved yet
    if (_initialized == mcsFALSE)
    {
        // Try to retrieve them
        if (RetrieveHostNameAndPortNumber() == FAILURE)
        {
            return ((char*)NULL);
        }

        _initialized = mcsTRUE;
    }

    // Return the host name
    return _hostName;
}

/**
 * Return the msgManager port number.
 *
 * \return the port number, or 0 if an error occured
 */
const mcsUINT16 msgMCS_ENV::GetPortNumber(void)
{
    logExtDbg("msgMCS_ENV::GetPortNumber()");

    // If the host name and the port number have not been retrieved yet
    if (_initialized == mcsFALSE)
    {
        // Try to retrieve them
        if (RetrieveHostNameAndPortNumber() == FAILURE)
        {
            return 0;
        }

        _initialized = mcsTRUE;
    }

    // Return the port number
    return _portNumber;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */
/**
 * Retrieve the msgManager host name and port number from the $MCSENV
 * environment variable, and the mcsEnvList file (located in
 * $MCSROOT/etc/mcsEnvList).
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMCS_ENV::RetrieveHostNameAndPortNumber(void)
{
    // Get the MCS environment name
    const char* searchedEnvName = mcsGetEnvName();

    // Try to load the MCS environment list file
    miscDYN_BUF envList;
    if (miscDynBufLoadFile(&envList, miscResolvePath("$MCSROOT/etc/mcsEnvList"),
                           "#")
        == FAILURE)
    {
        return FAILURE;
    }

    /* Jump all the headers and empty lines, and try to find a host name and
     * port number corresponding to the MCS environment name
     */
    mcsENVNAME parsedEnvName;
    memset(parsedEnvName, 0, sizeof(parsedEnvName));
    char* currentLine = miscDynBufGetNextLinePointer(&envList, NULL, mcsTRUE);
    do
    {
        // If the current line is not empty
        if ((currentLine != NULL) && (strlen(currentLine) != 0))
        {
            // Try to read the line values
            mcsINT32 readValueNumber = sscanf(currentLine, "%s %s %d",
                                              parsedEnvName, _hostName,
                                              &_portNumber);
    
            // If the sscanf didn't read the right number of values
            if (readValueNumber != 3)
            {
                return FAILURE;
            }
        }

        currentLine = miscDynBufGetNextLinePointer(&envList, currentLine,
                                                   mcsTRUE);
    }
    while ((strcmp(parsedEnvName, searchedEnvName) != 0) &&
           (currentLine != NULL));

    return SUCCESS;
}



/*___oOo___*/

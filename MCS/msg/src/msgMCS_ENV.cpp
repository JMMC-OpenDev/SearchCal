/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMCS_ENV.cpp,v 1.2 2004-12-05 19:14:48 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Dec-2004  Created
* gzins     05-Dec-2004  Changed method prototypes and class members
*
*******************************************************************************/

/**
 * \file
 * msgMCS_ENV class definition.
 */

static char *rcsId="@(#) $Id: msgMCS_ENV.cpp,v 1.2 2004-12-05 19:14:48 gzins Exp $"; 
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
    _envListFileLoaded = mcsFALSE;
    miscDynBufInit(&_envList);
}


/*
 * Class destructor
 */
msgMCS_ENV::~msgMCS_ENV()
{
    miscDynBufDestroy(&_envList);    
}


/*
 * Public methods
 */
/**
 * Return the host name of the MCS environment.
 *
 * If the environment name is not given, the current environment, defined by
 * MCSENV environment variable, is used.
 *
 * \return the host name of a given environment or NULL if environment name is
 * unknown.
 */
const char* msgMCS_ENV::GetHostName(char *envName)
{
    logExtDbg("msgMCS_ENV::GetHostName()");

    // If the file has not been loaded yet
    if (_envListFileLoaded == mcsFALSE)
    {
        // Try to retrieve them
        if (LoadEnvListFile() == FAILURE)
        {
            return NULL;
        }
    }

    // Retrieve the environment name in the list
    mcsLOGICAL found=mcsFALSE;
    
    // If found
    if (found == mcsTRUE)
    {
        // Return host name
        return NULL;
    }
    else
    {
        // Add error and return NULL
        return NULL;
    }
}

/**
 * Return the port number of the MCS environment.
 *
 * This method returns the port number of the given MCS environment; i.e. the
 * connection port number with the manager process of the MCS message service.
 * If the environment name is not given, the current environment, defined by
 * MCSENV environment variable, is used.
 *
 * \return the port number or -1 if environment name is unknown.
 */
const mcsINT32 msgMCS_ENV::GetPortNumber(char *envName)
{
    logExtDbg("msgMCS_ENV::GetPortNumber()");

    // If the file has not been loaded yet
    if (_envListFileLoaded == mcsFALSE)
    {
        // Try to retrieve them
        if (LoadEnvListFile() == FAILURE)
        {
            return -1;
        }
    }

   // Retrieve the environment name in the list
    mcsLOGICAL found=mcsFALSE;
    
    // If found
    if (found == mcsTRUE)
    {
        // Return the port number
        return -1;
    }
    else
    {
        // Add error and return -1
        return -1;
    }
}

/*
 * Protected methods
 */

/*
 * Private methods
 */
/**
 * Load file containing the environment list definition.
 *
 * This method loads in an internal buffer the file, named mcsEnvList and
 * located in $MCSROOT/etc directory, containing informations about the
 * defined MCS environments. A environment is defined by the host name on
 * which it is running, and the connection port number to the message manager.
 * This file has one entry (line) for each defined environment. The format of
 * each line is :
 *   <envName>   <hostName>  <portNumber>
 *
 * The field are separated by spaces; one or more spaces between fields.
 *
 * \return SUCCESS on successfull completion, or FAILURE otherwise.
 *
 * \err
 * The possible errors are :
 * \errname msgERR_UNKNOWN_ENV
 * \errname msgERR_FORMAT_ENVLIST
 */
mcsCOMPL_STAT msgMCS_ENV::LoadEnvListFile(void)
{
    // Resolve path of MCS environment list file
    char *fullPath;
    fullPath = miscResolvePath("$MCSROOT/etc/mcsEnvList");
    if (fullPath == NULL);
    {
        return FAILURE;
    }

    // Load the MCS environment list file
    if (miscDynBufLoadFile(&_envList, fullPath, "#") == FAILURE)
    {
        return FAILURE;
    }
#if 0
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
#endif
    return SUCCESS;
}



/*___oOo___*/

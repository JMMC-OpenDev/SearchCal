/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMCS_ENVS.cpp,v 1.3 2004-12-07 07:55:35 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Dec-2004  Created
* gzins     05-Dec-2004  Changed method prototypes and class members
* gzins     06-Dec-2004  Renamed msgMCS_ENV to msgMCS_ENVS
* lafrasse  03-Dec-2004  Added GetEnvLine() method and completed GetHostName()
*                        and GetPortNumber() methods
* gzins     07-Dec-2004  Improved error messages
*                        Fixed bug when testing result of miscResolvePath in
*                        LoadEnvListFile method
*
*******************************************************************************/

/**
 * \file
 * msgMCS_ENVS class definition.
 */

static char *rcsId="@(#) $Id: msgMCS_ENVS.cpp,v 1.3 2004-12-07 07:55:35 gzins Exp $"; 
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
#include "msgMCS_ENVS.h"
#include "msgErrors.h"
#include "msgPrivate.h"

/*
 * Class constructor
 */
msgMCS_ENVS::msgMCS_ENVS()
{
    _envListFileLoaded = mcsFALSE;
    miscDynBufInit(&_envList);
    memset(_hostName, 0, sizeof(_hostName));
}


/*
 * Class destructor
 */
msgMCS_ENVS::~msgMCS_ENVS()
{
    miscDynBufDestroy(&_envList);    
    memset(_hostName, 0, sizeof(_hostName));
}


/*
 * Public methods
 */
/**
 * Return the host name of the MCS environment.
 *
 * If the environment name is not given, the current environment, defined by
 * MCSENV environment variable, is used. If the MCSENV environment variable is
 * set to mcsUNKNOWN_ENV, then the local host name is returned.
 *
 * \return the host name of a given environment or NULL if an error occured.
 *
 * \err
 * The possible error id :
 * \errname msgERR_FORMAT_ENVLIST
 */
const char* msgMCS_ENVS::GetHostName(const char *envName)
{
    logExtDbg("msgMCS_ENVS::GetHostName()");

    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*)envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined
        searchedEnvName = (char*)mcsGetEnvName();
        if (strcmp(searchedEnvName, mcsUNKNOWN_ENV) == 0)
        {
            // Get the local host name and return it
            if (miscGetHostName(_hostName, sizeof(_hostName)) == FAILURE)
            {
                return ((char*)NULL);
            }

            return _hostName;
        }
    }

    // If the MCS Env. List file has not been loaded yet
    if (_envListFileLoaded == mcsFALSE)
    {
        // Load it
        if (LoadEnvListFile() == FAILURE)
        {
            return ((char*)NULL);
        }
    }

    // Get the environment file line containing the searched env. name
    char* envLine;
    envLine = GetEnvLine(searchedEnvName);
    if (envLine == NULL)
    {
        return ((char*)NULL);
    }

    // Read the line host name value
    mcsINT32 nbReadValue = 0;
    nbReadValue = sscanf(envLine, "%*s %s %*d", _hostName);

    // If the sscanf didn't read the right number of values
    if (nbReadValue != 1)
    {
        errAdd(msgERR_FORMAT_ENVLIST, envLine, "$MCSROOT/etc/mcsEnvList");
        return ((char*)NULL);
    }

    return _hostName;
}

/**
 * Return the port number of the MCS environment.
 *
 * This method returns the port number of the given MCS environment; i.e. the
 * connection port number with the manager process of the MCS message service.
 * If the environment name is not given, the current environment, defined by
 * MCSENV environment variable, is used. If the MCSENV environment variable is
 * set to mcsUNKNOWN_ENV, then the default msgMANAGER_PORT_NUMBER port number
 * is returned.
 *
 * \return the port number or -1 if an error occured.
 *
 * \err
 * The possible error id :
 * \errname msgERR_FORMAT_ENVLIST
 */
const mcsINT32 msgMCS_ENVS::GetPortNumber(const char *envName)
{
    logExtDbg("msgMCS_ENVS::GetPortNumber()");

    mcsINT32 portNumber;
    portNumber = msgMANAGER_PORT_NUMBER;

    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*)envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined
        searchedEnvName = (char*)mcsGetEnvName();
        if (strcmp(searchedEnvName, mcsUNKNOWN_ENV) == 0)
        {
            // Return the default msgManager port number
            return portNumber;
        }
    }
    
    // If the MCS Env. List file has not been loaded yet
    if (_envListFileLoaded == mcsFALSE)
    {
        // Load it
        if (LoadEnvListFile() == FAILURE)
        {
            return -1;
        }
    }

    // Get the environment file line containing the searched env. name
    char* envLine;
    envLine = GetEnvLine(searchedEnvName);
    if (envLine == NULL)
    {
        return -1;
    }

    // Read the line host name value
    mcsINT32 nbReadValue = 0;
    nbReadValue = sscanf(envLine, "%*s %*s %d", &portNumber);

    // If the sscanf didn't read the right number of values
    if (nbReadValue != 1)
    {
        errAdd(msgERR_FORMAT_ENVLIST, envLine, "$MCSROOT/etc/mcsEnvList");
        return -1;
    }

    return portNumber;
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
 */
mcsCOMPL_STAT msgMCS_ENVS::LoadEnvListFile(void)
{
    logExtDbg("msgMCS_ENVS::LoadEnvListFile()"); 
    // Resolve path of MCS environment list file
    char *fullPath;
    fullPath = miscResolvePath("$MCSROOT/etc/mcsEnvList");
    if (fullPath == NULL)
    {
        return FAILURE;
    }

    // Load the MCS environment list file
    if (miscDynBufLoadFile(&_envList, fullPath, "#") == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Return a pointer to the line containing the given MCS environment name.
 *
 * \return NULL if no lines contain the searched environment name.
 *
 * \err
 * The possible errors are :
 * \errname msgERR_UNKNOWN_ENV
 * \errname msgERR_FORMAT_ENVLIST
 */
char* msgMCS_ENVS::GetEnvLine(const char *envName)
{
    /* Jump all the headers and empty lines, and try to find the line containing
     * the hostname & port number corresponding to the given MCS env. name
     */
    mcsENVNAME parsedEnvName;
    memset(parsedEnvName, 0, sizeof(parsedEnvName));
    mcsINT32 nbReadValue = 0;
    char* currentLine = miscDynBufGetNextLinePointer(&_envList, NULL, mcsTRUE);
    do
    {
        // If the current line is not empty
        if ((currentLine != NULL) && (strlen(currentLine) != 0))
        {
            // Try to read the line values
            nbReadValue = sscanf(currentLine, "%s %*s %*d", parsedEnvName);
    
            // If the sscanf didn't read the right number of values
            if (nbReadValue != 1)
            {
                errAdd(msgERR_FORMAT_ENVLIST, currentLine, "$MCSROOT/etc/mcsEnvList");
                return ((char*)NULL);
            }

            // If the searched environmnt name is in the current line
            if (strcmp(parsedEnvName, envName) == 0)
            {
                return currentLine;
            }
        }

        currentLine = miscDynBufGetNextLinePointer(&_envList, currentLine,
                                                   mcsTRUE);
    }
    while (currentLine != NULL);

    errAdd(msgERR_UNKNOWN_ENV, envName, "$MCSROOT/etc/mcsEnvList");

    return ((char*)NULL);
}


/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envLIST.cpp,v 1.1 2004-12-07 16:45:56 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  07-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * envLIST class definition.
 */

static char *rcsId="@(#) $Id: envLIST.cpp,v 1.1 2004-12-07 16:45:56 lafrasse Exp $"; 
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
#include "envLIST.h"
#include "envPrivate.h"
#include "envErrors.h"

/**
 * Class constructor
 */
envLIST::envLIST()
{
    _fileAlreadyLoaded = mcsFALSE;
    memset(_hostName, 0, sizeof(_hostName));
}

/**
 * Class destructor
 */
envLIST::~envLIST()
{
    _fileAlreadyLoaded = mcsFALSE;
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
 */
const char* envLIST::GetHostName(const char *envName)
{
    logExtDbg("envLIST::GetHostName()");

    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*)envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined
        searchedEnvName = (char*)mcsGetEnvName();
        if (strcmp(searchedEnvName, mcsUNKNOWN_ENV) == 0)
        {
            memset(_hostName, 0, sizeof(_hostName));

            // Get the local host name and return it
            if (miscGetHostName(_hostName, sizeof(_hostName)) == FAILURE)
            {
                return ((char*)NULL);
            }

            return _hostName;
        }
    }

    // If the MCS Env. List file has not been loaded yet
    if (_fileAlreadyLoaded == mcsFALSE)
    {
        // Load it
        if (LoadEnvListFile() == FAILURE)
        {
            return ((char*)NULL);
        }
        _fileAlreadyLoaded = mcsTRUE;
    }

    // Find the searched environment name in the internal map
    map<string,pair<string,int> > ::iterator i;
    if (_map.find(searchedEnvName) == _map.end())
    {
        errAdd(envERR_UNKNOWN_ENV, searchedEnvName, "$MCSROOT/etc/mcsEnvList");
        return ((char*)NULL);
    }

    return _map[searchedEnvName].first.c_str();
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
 */
const mcsINT32 envLIST::GetPortNumber(const char *envName)
{
    logExtDbg("envLIST::GetPortNumber()");

    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*)envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined
        searchedEnvName = (char*)mcsGetEnvName();
        if (strcmp(searchedEnvName, mcsUNKNOWN_ENV) == 0)
        {
            // Return the default message manager port number
            return envDEFAULT_MESSAGE_MANAGER_PORT_NUMBER;
        }
    }
    
    // If the MCS Env. List file has not been loaded yet
    if (_fileAlreadyLoaded == mcsFALSE)
    {
        // Load it
        if (LoadEnvListFile() == FAILURE)
        {
            return -1;
        }
        _fileAlreadyLoaded = mcsTRUE;
    }

    // Find the searched environment name in the internal map
    map<string,pair<string,int> > ::iterator i;
    if (_map.find(searchedEnvName) == _map.end())
    {
        errAdd(envERR_UNKNOWN_ENV, searchedEnvName, "$MCSROOT/etc/mcsEnvList");
        return -1;
    }

    return _map[searchedEnvName].second;
}

/**
 * Show all the environment list content.
 */
void envLIST::Show(void)
{
    logExtDbg("envLIST::Show()");

    // If the MCS Env. List file has not been loaded yet
    if (_fileAlreadyLoaded == mcsFALSE)
    {
        // Load it
        if (LoadEnvListFile() == FAILURE)
        {
            cout << "Could not load '$MCSROOT/etc/mcsEnvList' file" << endl;
            return;
        }
    }

    // Show all the map content
    cout << "'environment name' = {'host name', 'port number'}" << endl << endl;
    map<string,pair<string,int> > ::iterator i;
    for (i = _map.begin(); i != _map.end(); i++)
    {
        cout << "'" << (*i).first         << "' = {"
             << "'" << (*i).second.first  << "', "
             << "'" << (*i).second.second << "'}" << endl;
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
 * This method loads in an internal map the file, named mcsEnvList and
 * located in $MCSROOT/etc directory, containing informations about the
 * defined MCS environments. A environment is defined by the host name on
 * which it is running, and the connection port number to its own message
 * manager. This file has one entry (line) for each defined environment. The
 * format of each line is :
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
mcsCOMPL_STAT envLIST::LoadEnvListFile(void)
{
    logExtDbg("msgMCS_ENVS::LoadEnvListFile()"); 
    // Resolve path of MCS environment list file
    char *fullPath;
    fullPath = miscResolvePath("$MCSROOT/etc/mcsEnvList");
    if (fullPath == NULL)
    {
        return FAILURE;
    }

    /* Load the MCS environment list file in a misc Dynamic Buffer for line by
     * line parsing.
     */
    miscDYN_BUF envList;
    miscDynBufInit(&envList);
    if (miscDynBufLoadFile(&envList, fullPath, "#") == FAILURE)
    {
        return FAILURE;
    }

    /* Jump all the headers and empty lines, and feed the map with the
     * environments data found the mcsEnvList file read line by line.
     */
    mcsINT32     nbReadValue = 0;
    mcsINT32     portNumber  = 0;
    mcsENVNAME   parsedEnvName;
    mcsSTRING256 hostName;
    memset(parsedEnvName, 0, sizeof(parsedEnvName));
    memset(hostName, 0, sizeof(hostName));
    char* currentLine = miscDynBufGetNextLinePointer(&envList, NULL, mcsTRUE);
    do
    {
        // If the current line is not empty
        if ((currentLine != NULL) && (strlen(currentLine) != 0))
        {
            // Try to read the line values
            nbReadValue = sscanf(currentLine, "%s %s %d", parsedEnvName,
                                 hostName, &portNumber);
    
            // If the sscanf didn't read the right number of values
            if (nbReadValue != 3)
            {
                errAdd(envERR_FORMAT_ENVLIST, currentLine,
                       "$MCSROOT/etc/mcsEnvList");
                return FAILURE;
            }

            // Verify that there is not a 'parsedEnvName' element in the map
            map<string,pair<string,int> > ::iterator i;
            if (_map.find(parsedEnvName) != _map.end())
            {
                errAdd(envERR_DUPLICATE_ENV, parsedEnvName,
                       "$MCSROOT/etc/mcsEnvList");
                return FAILURE;
            }

            _map[parsedEnvName] = pair<string,int>(hostName, portNumber);
        }

        currentLine = miscDynBufGetNextLinePointer(&envList, currentLine,
                                                   mcsTRUE);
    }
    while (currentLine != NULL);

    /* Verify that there is not to differnet environments using the same port
     * number on the same host
     */
    map<string,pair<string,int> > ::iterator i;
    for (i = _map.begin(); i != _map.end(); i++)
    {
        map<string,pair<string,int> > ::iterator j;
        for (j = i, j++; j != _map.end(); j++)
        {
            // If the host name and port number couples are the same
            if ((*j).second == (*i).second)
            {
                errAdd(envERR_PORT_ALREADY_USED, (*j).second.second,
                        (*j).first.c_str(), (*i).first.c_str(),
                        (*j).second.first.c_str());

                return FAILURE;
            }
        }
    }

    return SUCCESS;
}


/*___oOo___*/

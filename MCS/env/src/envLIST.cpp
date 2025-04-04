/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * envLIST class definition.
 */

/*
 * System Headers
 */
#include <iostream>
#include <iomanip>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
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

    memset(_hostName, 0, sizeof (_hostName));
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
 * Return the host name of the given MCS environment.
 *
 * It returns the host name of the given environment.
 * If the environment name is not given, it returns the host name of the
 * current environment (defined by MCSENV environment variable) or the local
 * host name if the MCSENV is not set.
 *
 * \param envName the MCS environment for which the host name should be returned
 *
 * \return the host name of a given environment or NULL if an error occurred.
 */
const char* envLIST::GetHostName(const char *envName)
{
    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*) envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined, set it to the default one
        searchedEnvName = (char*) mcsGetEnvName();
    }

    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        return ((char*) NULL);
    }

    // Find the searched environment name in the internal map
    map<string, pair<string, int> > ::iterator i;
    if (_map.find(searchedEnvName) == _map.end())
    {
        errAdd(envERR_UNKNOWN_ENV, searchedEnvName, "$MCSDATA/mcscfgEnvList");
        return ((char*) NULL);
    }

    return _map[searchedEnvName].first.c_str();
}

/**
 * Return the port number of the given MCS environment.
 *
 * This method returns the port number of the given MCS environment; i.e. the
 * connection port number with the manager process of the MCS message service.
 * If the environment name is not given, it returns the port number associated
 * the current environment (defined by MCSENV environment variable) or the
 * default port number.
 *
 * \param envName the MCS env. for which the port number should be returned
 *
 * \return the port number or -1 if an error occurred.
 */
mcsINT32 envLIST::GetPortNumber(const char *envName)
{
    // If no environment name was specified
    char* searchedEnvName;
    searchedEnvName = (char*) envName;
    if (searchedEnvName == NULL)
    {
        // If $MCSENV is not defined, set it to the default one
        searchedEnvName = (char*) mcsGetEnvName();
    }

    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        return -1;
    }

    // Find the searched environment name in the internal map
    map<string, pair<string, int> > ::iterator i;
    if (_map.find(searchedEnvName) == _map.end())
    {
        errAdd(envERR_UNKNOWN_ENV, searchedEnvName, "$MCSDATA/mcscfgEnvList");
        return -1;
    }

    return _map[searchedEnvName].second;
}

/**
 * Show all the environment list content.
 */
void envLIST::Show(void)
{
    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        cout << "Could not load '$MCSDATA/mcscfgEnvList' file" << endl;
        return;
    }

    // Show all the map content
    cout << "+--------------------+--------------------+-------------+" << endl
            << "|   ENVIRONMENT NAME |          HOST NAME | PORT NUMBER |" << endl
            << "+--------------------+--------------------+-------------+" << endl;
    map<string, pair<string, int> > ::iterator i;
    for (i = _map.begin(); i != _map.end(); i++)
    {
        cout << "| " << setw(18) << (*i).first         << " "
                << "| " << setw(18) << (*i).second.first  << " "
                << "| " << setw(11) << (*i).second.second << " |" << endl;
    }
    cout << "+--------------------+--------------------+-------------+" << endl;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */

/**
 * Load the MCS file containing the environment list definition.
 *
 * This method loads in an internal map the file, named mcscfgEnvList and
 * located in $MCSDATA directory, containing informations about the
 * defined MCS environments. A environment is defined by the host name on
 * which it is running, and the connection port number to its own message
 * manager. This file has one entry (line) for each defined environment. The
 * format of each line is :
 *   <envName>   <hostName>  <portNumber>
 *
 * The field are separated by spaces; one or more spaces between fields.
 * If no file exists, only the default MCS env. will be added.
 *
 * \return mcsSUCCESS on successful completion, or mcsFAILURE otherwise.
 */
mcsCOMPL_STAT envLIST::LoadEnvListFile(void)
{
    // If the MCS Env. List file has not been loaded yet
    if (_fileAlreadyLoaded == mcsTRUE)
    {
        return mcsSUCCESS;
    }
    _fileAlreadyLoaded = mcsTRUE;

    // Resolve path of MCS environment list file
    char *fullPath;
    fullPath = miscResolvePath("$MCSDATA/mcscfgEnvList");
    if (fullPath == NULL)
    {
        return mcsFAILURE;
    }

    /* Load the MCS environment list file in a misc Dynamic Buffer for line by
     * line parsing */
    miscDYN_BUF envList;
    FAIL(miscDynBufInit(&envList));
    if (miscDynBufLoadFile(&envList, fullPath, "#") == mcsFAILURE)
    {
        free(fullPath);
        return mcsSUCCESS;
    }
    free(fullPath);

    /* Jump all the headers and empty lines, and feed the map with the
     * environments data found in the mcscfgEnvList file read line by line */
    mcsINT32     nbReadValue = 0;
    mcsINT32     portNumber  = 0;
    mcsSTRING32  parsedEnvName;
    mcsSTRING256 hostName;
    
    memset(parsedEnvName, 0, mcsENVNAME_LEN);
    memset(hostName, 0, sizeof (hostName));
    mcsSTRING1024 currentLine;
    mcsUINT32     currentLineLength = sizeof (currentLine);
    const char* currentPos = miscDynBufGetNextLine(&envList, NULL, currentLine,
                                                   currentLineLength, mcsTRUE);
    do
    {
        // If the current line is not empty
        if ((currentPos != NULL) && (strlen(currentLine) != 0))
        {
            // Read the line values
            nbReadValue = sscanf(currentLine, "%s %s %d", parsedEnvName,
                                 hostName, &portNumber);

            // If the sscanf didn't read the right number of values
            if (nbReadValue != 3)
            {
                errAdd(envERR_FORMAT_ENVLIST, currentLine,
                       "$MCSDATA/mcscfgEnvList");
                return mcsFAILURE;
            }

            // If 'hostName' contains the 'localhost' value
            if (strcmp(hostName, "localhost") == 0)
            {
                // Replace it by the local host IP address
                if (miscGetHostName(hostName, sizeof (hostName)) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }

            // Verify that there is not a 'parsedEnvName' element in the map
            map<string, pair<string, int> > ::iterator i;
            if (_map.find(parsedEnvName) != _map.end())
            {
                errAdd(envERR_DUPLICATE_ENV, parsedEnvName,
                       "$MCSDATA/mcscfgEnvList");
                return mcsFAILURE;
            }

            _map[parsedEnvName] = pair<string, int>(hostName, portNumber);
        }

        currentPos = miscDynBufGetNextLine(&envList, currentPos, currentLine,
                                           currentLineLength, mcsTRUE);
    }
    while (currentPos != NULL);

    // Destroy the temp Dynamic Buffer
    miscDynBufDestroy(&envList);

    // Verify that there is not two different environments using the same port
    // number on the same host
    map<string, pair<string, int> > ::iterator i;
    for (i = _map.begin(); i != _map.end(); i++)
    {
        map<string, pair<string, int> > ::iterator j;
        for (j = i, j++; j != _map.end(); j++)
        {
            // If the host name and port number pairs are the same
            if ((*j).second == (*i).second)
            {
                errAdd(envERR_PORT_ALREADY_USED, (*j).second.second,
                       (*j).first.c_str(), (*i).first.c_str(),
                       (*j).second.first.c_str());

                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/

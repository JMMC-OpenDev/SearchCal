/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: envLIST.cpp,v 1.7 2005-02-13 17:26:51 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/13 16:53:13  gzins
 * Added CVS log as modification history
 *
 * lafrasse  07-Dec-2004  Created
 * lafrasse  08-Dec-2004  Comment refinments, added the default MCS env in the
 *                        internal map by default, factorized the 'file already
 *                        loaded' detection code from GetHostName(), Show() and
 *                        GetPortNumber() to LoadEnvListFile(), and refined the
 *                        output format of Show()
 * gzins     05-Jan-2005  Minor changes in documentation
 *
 ******************************************************************************/

/**
 * \file
 * envLIST class definition.
 */

static char *rcsId="@(#) $Id: envLIST.cpp,v 1.7 2005-02-13 17:26:51 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
#include <iomanip>
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
 * Return the host name of the given MCS environment.
 *
 * It returns the host name of the given environment. 
 * If the environment name is not given, it returns the host name of the
 * current environment (defined by MCSENV environment variable) or the local
 * host name if the MCSENV is not set.
 *
 * \param envName the MCS environment for which the host name should be returned
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
        // If $MCSENV is not defined, set it to the default one
        searchedEnvName = (char*)mcsGetEnvName();
    }

    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        return ((char*)NULL);
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
        // If $MCSENV is not defined, set it to the default one
        searchedEnvName = (char*)mcsGetEnvName();
    }
    
    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        return -1;
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

    // Load the MCS Env. List file
    if (LoadEnvListFile() == mcsFAILURE)
    {
        cout << "Could not load '$MCSROOT/etc/mcsEnvList' file" << endl;
        return;
    }

    // Show all the map content
    cout << "+--------------------+--------------------+-------------+" << endl
         << "|   ENVIRONMENT NAME |          HOST NAME | PORT NUMBER |" << endl
         << "+--------------------+--------------------+-------------+" << endl;
    map<string,pair<string,int> > ::iterator i;
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
 * This method loads in an internal map the file, named mcsEnvList and
 * located in $MCSROOT/etc directory, containing informations about the
 * defined MCS environments. A environment is defined by the host name on
 * which it is running, and the connection port number to its own message
 * manager. This file has one entry (line) for each defined environment. The
 * format of each line is :
 *   <envName>   <hostName>  <portNumber>
 *
 * The field are separated by spaces; one or more spaces between fields.
 * If no file exists, only the default MCS env. will be added.
 *
 * \return mcsSUCCESS on successfull completion, or mcsFAILURE otherwise.
 */
mcsCOMPL_STAT envLIST::LoadEnvListFile(void)
{
    logExtDbg("msgMCS_ENVS::LoadEnvListFile()"); 

    // If the MCS Env. List file has not been loaded yet
    if (_fileAlreadyLoaded == mcsTRUE)
    {
        return mcsSUCCESS;
    }
    _fileAlreadyLoaded = mcsTRUE;

    // Put the default MCS env. host name and port number in the internal map
    if (miscGetHostName(_hostName, sizeof(_hostName)) == mcsFAILURE)
    {
        strncpy(_hostName, "localhost", sizeof(_hostName));
    }
    _map[mcsUNKNOWN_ENV] = pair<string,int>(_hostName,
                               envDEFAULT_MESSAGE_MANAGER_PORT_NUMBER);

    // Resolve path of MCS environment list file
    char *fullPath;
    fullPath = miscResolvePath("$MCSROOT/etc/mcsEnvList");
    if (fullPath == NULL)
    {
        return mcsFAILURE;
    }

    // Load the MCS environment list file in a misc Dynamic Buffer for line by
    // line parsing.
    miscDYN_BUF envList;
    miscDynBufInit(&envList);
    if (miscDynBufLoadFile(&envList, fullPath, "#") == mcsFAILURE)
    {
        return mcsSUCCESS;
    }

    // Jump all the headers and empty lines, and feed the map with the
    // environments data found in the mcsEnvList file read line by line.
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
            // Read the line values
            nbReadValue = sscanf(currentLine, "%s %s %d", parsedEnvName,
                                 hostName, &portNumber);
    
            // If the sscanf didn't read the right number of values
            if (nbReadValue != 3)
            {
                errAdd(envERR_FORMAT_ENVLIST, currentLine,
                       "$MCSROOT/etc/mcsEnvList");
                return mcsFAILURE;
            }

            // Verify that there is not a 'parsedEnvName' element in the map
            map<string,pair<string,int> > ::iterator i;
            if (_map.find(parsedEnvName) != _map.end())
            {
                errAdd(envERR_DUPLICATE_ENV, parsedEnvName,
                       "$MCSROOT/etc/mcsEnvList");
                return mcsFAILURE;
            }

            _map[parsedEnvName] = pair<string,int>(hostName, portNumber);
        }

        currentLine = miscDynBufGetNextLinePointer(&envList, currentLine,
                                                   mcsTRUE);
    }
    while (currentLine != NULL);

    // Destroy the temp Dynamic Buffer
    miscDynBufDestroy(&envList);

    // Verify that there is not two different environments using the same port
    // number on the same host
    map<string,pair<string,int> > ::iterator i;
    for (i = _map.begin(); i != _map.end(); i++)
    {
        map<string,pair<string,int> > ::iterator j;
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

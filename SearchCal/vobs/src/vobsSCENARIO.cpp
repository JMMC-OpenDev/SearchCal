/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.cpp,v 1.48 2010-01-29 13:06:51 lafrasse Exp $"
*
* History
* ------- 
* $Log: not supported by cvs2svn $
* Revision 1.47  2009/04/17 15:29:48  lafrasse
* Updated log level from Info to Test to clean sclws log output.
*
* Revision 1.46  2007/10/31 11:18:21  gzins
* Changed format of the execution progress status
*
* Revision 1.45  2007/02/04 20:23:11  lafrasse
* Added Init() API to enable polymorphism.
* Added GetCatalogIndex() API to get the index of the catalog being queried.
* Added GetNbOfCatalogs() API to get the number of catalogs of the scenario.
*
* Revision 1.44  2006/12/21 15:10:46  lafrasse
* Updated progression monitoring code (moved from static-based to instance-based).
*
* Revision 1.43  2006/04/10 11:49:12  gzins
* Added missing breaks in action switch case in Execute() method
*
* Revision 1.42  2006/04/07 08:23:00  gzins
* Removed useless \n in log messages
*
* Revision 1.41  2006/03/22 10:42:57  scetre
* Changed Ambiguous catalog name variable for debug
*
* Revision 1.40  2006/03/03 15:03:28  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.39  2006/03/01 12:38:44  lafrasse
* Streamlined code with systematic use of temporary variables
*
* Revision 1.38  2006/02/21 16:32:00  scetre
* Updated documentation
*
* Revision 1.37  2006/01/18 08:46:49  scetre
* Managed the new option which could be gave to a catalog
*
* Revision 1.36  2005/12/22 14:14:17  lafrasse
* Added a catalog counter and index to further report progression in the GUI status message
*
* Revision 1.35  2005/12/21 10:35:15  lafrasse
* Added queried catalog name logging as action status
*
* Revision 1.34  2005/12/14 15:08:10  scetre
* Added log Information
*
* Revision 1.33  2005/12/12 14:06:51  scetre
* Added test in empty or null list to when scenario is executing
*
* Revision 1.32  2005/12/07 11:47:45  gzins
* Improved error handling
*
* Revision 1.31  2005/11/29 10:35:52  gzins
* Updated to split treatment (catalog querying, list merging and list filtering) performed for each scenario step
*
* Revision 1.30  2005/11/24 08:13:50  scetre
* Changed mother class of filter from vobsFILTER to vobsFILTER
*
* Revision 1.29  2005/11/17 08:05:52  scetre
* Added possibility to add no catalog in an entry
*
* Revision 1.28  2005/11/16 12:53:18  scetre
* Updated documentation
*
* Revision 1.27  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.26  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.25  2005/11/15 15:21:02  scetre
* Removed unused printf
*
* Revision 1.24  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.23  2005/10/07 12:18:48  scetre
* add timlog for catalog query
*
* Revision 1.22  2005/06/01 14:16:56  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.21  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.20  2005/03/04 16:28:28  scetre
* Changed Call to Save method
*
* Revision 1.19  2005/03/04 16:05:51  scetre
* Updated call to Save method
*
* Revision 1.18  2005/03/04 15:35:02  gzins
* Removed printf
*
* Revision 1.17  2005/02/22 10:44:51  gzins
* Fixed bug related to the saved files; environment variables was not resolved
*
* Revision 1.16  2005/02/11 14:23:07  gluck
* Suppressed misc.h file inclusion
*
* Revision 1.15  2005/02/10 06:38:58  gzins
* Added logInfo; name of consulted catalog and number of stars found.
*
* Revision 1.14  2005/02/09 06:10:44  gzins
* Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
*
* Revision 1.13  2005/02/08 20:42:06  gzins
* Used new vobsCATALOG::GetName() method
*
* Revision 1.12  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.11  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.10  2005/02/08 08:38:06  scetre
* Removed printf used for debug purpose
*
* Revision 1.9  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
*
* Revision 1.8  2005/02/07 17:36:53  scetre
* added printf to resolve bug problem in multiple query
*
* Revision 1.7  2005/01/27 15:56:04  scetre
* scenario became a friend class of scenario entry to be able to access private members
*
* Revision 1.6  2005/01/27 13:44:04  scetre
* remove vobsSCENARIO_ENTRY class
*
* Revision 1.5  2005/01/26 08:50:25  scetre
* upated documentation
*
* Revision 1.4  2005/01/26 08:20:39  scetre
* change history
*
* scetre    08-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * @file
 * vobsSCENARIO class definition.
 * 
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsSCENARIO.cpp,v 1.48 2010-01-29 13:06:51 lafrasse Exp $"; 


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include<list>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"
#include "timlog.h"
#include "sdb.h"

/*
 * Local Headers 
 */
#include "vobsDISTANCE_FILTER.h"
#include "vobsSCENARIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/*
 * Class constructor
 */
vobsSCENARIO::vobsSCENARIO(sdbENTRY* progress)
{
    _entryIterator = _entryList.begin();
    // affect to NULL the catalog list
    _catalogList = NULL;
    _nbOfCatalogs = 0;
    _catalogIndex = 0;

    _progress = progress;
}

/*
 * Class destructor
 */
vobsSCENARIO::~vobsSCENARIO()
{
    _entryList.clear();
}

/*
 * Public methods
 */

/**
 * Return the total number of catalog queried by the scenario.
 *
 * @return an mcsUINT32 
 */
mcsUINT32 vobsSCENARIO::GetNbOfCatalogs()
{
    logTrace("vobsSCENARIO::GetNbOfCatalogs()");

    return _nbOfCatalogs;
}

/**
 * Return the current index of the catalog being queried.
 *
 * @return an mcsUINT32 
 */
mcsUINT32 vobsSCENARIO::GetCatalogIndex()
{
    logTrace("vobsSCENARIO::GetCatalogIndex()");

    return _catalogIndex;
}

/**
 * Initialize the scenario.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT vobsSCENARIO::Init(vobsREQUEST * request)
{
    logTrace("vobsSCENARIO::Init()");
    return mcsFAILURE;
}

/**
 * Adds the element at the end of the list.
 *
 * The method create a entry from the parameters and put it in the list.
 * @param catalogName the catalog name to ask corresponding to the entry
 * @param request request associated with the scenario
 * @param listInput list of star in enter of the research
 * @param listOutput list of star resulting of the asking of the catalog
 * @param action enumerate representing the action to do
 * @param criteriaList list of comparaison criteria
 * @param filter filter
 * @param queryOption query option
 * 
 * @return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSCENARIO::AddEntry(mcsSTRING32                   catalogName,
                                     vobsREQUEST*                  request,
                                     vobsSTAR_LIST*                listInput,
                                     vobsSTAR_LIST*                listOutput,
                                     vobsACTION                    action,
                                     vobsSTAR_COMP_CRITERIA_LIST*  criteriaList,
                                     vobsFILTER*                   filter,
                                     string                        queryOption)
{
    logTrace("vobsSCENARIO::AddEntry()");
    
    // Create a new entry
    // Affect in this entry the catalogName, the list input, the list output,
    // the action to do, and the criteria list
    vobsSCENARIO_ENTRY entry(catalogName,
                             request,
                             listInput,
                             listOutput,
                             action,
                             criteriaList,
                             filter);
    // Set query option
    entry.SetQueryOption(queryOption);
    
    // Increment true catalog counter (if not a filter)
    if (strcmp(catalogName, vobsNO_CATALOG_ID) != 0)
    {
        _nbOfCatalogs++;
    }

    // Put element in the list    
    _entryList.push_back(entry);

    return mcsSUCCESS;
}

/**
 * Execute the scenario
 *
 * The methods execute the scenario which had been loaded before. It will
 * read each entry and query the specific catalog.
 * The scenario execution progress is reported using sdbENTRY instance given to
 * constructor. It writes a message having the following format:
 *      &lt;status&gt; &lt;catalog name&gt; &lt;catalog number&gt; 
 *      &lt;number of catalogs&gt;
 * where
 *  <li> &lt;status&gt; is 1 meaning 'In progress'
 *  <li> &lt;catalog name&gt; is the name of catalog currently consulted
 *  <li> &lt;catalog number&gt; is the catalog number in the list
 *  <li> &;number of catalogs&gt; is the number of catalogs in the list
 *
 * @param starList vobsSTAR_LIST which is the result of the interrogation,
 * this is the last list return of the last interrogation.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT vobsSCENARIO::Execute(vobsSTAR_LIST &starList)
{
    logTrace("vobsSCENARIO::Execute()");

    // Create a temporary list of star in xhich will be store the lst input
    vobsSTAR_LIST tempList;
    // Create an iterator for this temporary list
    _entryIterator = _entryList.begin();
    // For each entry
    while (_entryIterator != _entryList.end())
    {
        vobsSTAR_COMP_CRITERIA_LIST *criteriaList;
        criteriaList = _entryIterator->_criteriaList;

        // Begin to clean the temporary list
        if (tempList.Clear() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Copy the list input into it
        vobsSTAR_LIST* inputList = _entryIterator->_listInput;
        if (inputList != NULL)
        {
            tempList.Copy(*inputList);
        }


        // **** CATALOG QUERYING ****
        // If there is a catalog to query
        char* catalogName = _entryIterator->_catalogName;
        if ((strcmp(catalogName, vobsNO_CATALOG_ID) != 0) &&
            ((inputList == NULL) ||
             ((inputList != NULL) && (inputList->Size() != 0))))
        {
            // Start research in entry's catalog
            logTest("Consulting %s ...", catalogName);

            // define action for timlog trace
            mcsSTRING256 timLogActionName;
            // Get catalog name, and replace '/' by '_'
            mcsSTRING32 catalog;
            strcpy(catalog, catalogName);
            if (miscReplaceChrByChr(catalog, '/', '_') == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            strcpy(timLogActionName, catalog);
            // Add request type (primary or not)
            if (inputList == NULL)
            {
                strcat(timLogActionName, "_1");
            }
            else
            {
                strcat(timLogActionName, "_2");
            }

            // Start time counter
            timlogTestStart(timLogActionName);

            // Write the current action in the shared database
            mcsSTRING256 message;
            snprintf(message, sizeof(message),
                     "1\t%s\t%d\t%d",
                     catalogName, (_catalogIndex + 1), _nbOfCatalogs);
            if (_progress->Write(message) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Check if the list is not NULL, i.e the SetCatalogList has
            // ever been executed one time
            if (_catalogList == NULL)
            {
                errAdd(vobsERR_CATALOG_LIST_EMPTY);
                return mcsFAILURE;
            }

            // Get catalog from list
            vobsCATALOG *tempCatalog = _catalogList->Get(catalogName);
            if (tempCatalog == NULL)
            {
                errAdd(vobsERR_UNKNOWN_CATALOG);
                return mcsFAILURE;
            }

            // Get query option from scenario entry
            string queryOption = _entryIterator->GetQueryOption();
            tempCatalog->SetOption(queryOption);
            
            vobsREQUEST* request = _entryIterator->_request;
            // if research failed, return mcsFAILURE
            if ((tempCatalog)->Search(*request, tempList) == mcsFAILURE )
            {
                return mcsFAILURE;
            }
            logTest("...number of stars return = %d", tempList.Size());
            _catalogIndex++;

            // Clean the catalog option
            tempCatalog->SetOption("");

            // Stop time counter
            timlogStop(timLogActionName);

            // If the verbose level is higher or equal to debug level, the back
            // result will be stored in file
            if (logGetStdoutLogLevel() >= logDEBUG)
            {
                // This file will be stored in the $MCSDATA/tmp repository
                mcsSTRING256 logFileName;
                const char*  resolvedPath;
                strcpy(logFileName, "$MCSDATA/tmp/");

                // Get band used for search
                const char *band;
                band = request->GetSearchBand();
                strcat(logFileName, band);

                // Get catalog name, and replace '/' by '_'
                mcsSTRING32 catName;
                strcpy(catName, catalogName);
                if (miscReplaceChrByChr(catName, '/', '_') == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                strcat(logFileName, "_");
                strcat(logFileName, catName);

                // Add request type (primary or not)
                if (inputList == NULL)
                {
                    strcat(logFileName, "_1.log");
                }
                else
                {
                    strcat(logFileName, "_2.log");
                }

                // Resolve path
                resolvedPath = miscResolvePath(logFileName);
                if (resolvedPath != NULL)
                {
                    // Save resulting list
                    if (tempList.Save(resolvedPath) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
            }
        }

        // **** LIST COPYING/MERGING ****
        // There are 3 different action to do when the scenario is
        // executed
        vobsSTAR_LIST* outputList = _entryIterator->_listOutput;
        switch(_entryIterator->_action)
        {
            // First action is vobsCOPY. The list output will be clear and
            // it will be merge from the temporary list which contain 
            // the list input
            case vobsCOPY:
            {
                if (outputList->Clear() == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                if (outputList->Merge(tempList, criteriaList) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                logTest("after COPY, star list size = %d", outputList->Size());
                
                break;
            }

            // Second action is vobsMERGE. The list output will be 
            // merge from the temporary list whitout being clear.
            // The information which is stored in the the list 
            // output is preserved and can be modified
            case vobsMERGE:
            {
                if (outputList->Merge(tempList, criteriaList) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                logTest("after MERGE, star list size = %d", outputList->Size());
            
                break;
            }

            // Third action is vobsUPDATE_ONLY. The list output will
            // be merge from thetemporary list, but this merge will
            // not modified the existant information of the list output
            case vobsUPDATE_ONLY:
            {
                if (outputList->Merge(tempList, criteriaList, mcsTRUE)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                logTest("after UPDATE_ONLY, star list size = %d",
                        outputList->Size());
                
                break;
            }

            default:
                break;
        }

        // **** LIST FILTERING ****
        // Apply filter if defined
        vobsFILTER* filter = _entryIterator->_filter;
        if (filter != NULL)
        {
            filter->Apply(outputList);
            logTest("after FILTER, star list size = %d", outputList->Size());
        }
        
        // Next entry
        _entryIterator++;
    }

    // Copy resulting list
    if (starList.Copy(*_entryList.back()._listOutput) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logTest("%d star(s) found.", starList.Size()); 

    _catalogIndex = 0;
    return mcsSUCCESS;
}

/**
 * Set catalog List
 *
 * This method affect to the pointer of catalog list the value of the pointer
 * gave as parmameter
 *
 * @param catalogList a catalog list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSCENARIO::SetCatalogList(vobsCATALOG_LIST *catalogList)
{
    logTrace("vobsSCENARIO::SetCatalogList()");

    // equal the two pointer
    _catalogList = catalogList;

    return mcsSUCCESS;
}

/**
 * Clear the scenario
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSCENARIO::Clear(void)
{
    logTrace("vobsSCENARIO::Clear()");

    _catalogList = NULL;
    _entryList.erase(_entryList.begin(), _entryList.end());
    _nbOfCatalogs = 0;
    _catalogIndex = 0;

    return mcsSUCCESS;
}
/*___oOo___*/

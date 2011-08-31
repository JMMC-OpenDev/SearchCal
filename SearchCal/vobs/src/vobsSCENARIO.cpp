/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSCENARIO class definition.
 */

/* 
 * System Headers 
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
mcsCOMPL_STAT vobsSCENARIO::Init(vobsREQUEST* request)
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
mcsCOMPL_STAT vobsSCENARIO::AddEntry(const mcsSTRING32             catalogName,
                                     vobsREQUEST*                  request,
                                     vobsSTAR_LIST*                listInput,
                                     vobsSTAR_LIST*                listOutput,
                                     vobsACTION                    action,
                                     vobsSTAR_COMP_CRITERIA_LIST*  criteriaList,
                                     vobsFILTER*                   filter,
                                     const char*                   queryOption)
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
    if (queryOption != NULL)
    {
        entry.SetQueryOption(string(queryOption));
    }
    
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
    logInfo("vobsSCENARIO::Execute() - start");

    mcsINT64 elapsedTime;       // current search time
    mcsINT64 sumSearchTime = 0; // cumulative search time 
    
    // define action for timlog trace
    mcsSTRING256 timLogActionName;

    // Create a temporary list of star in which will be store the list input
    vobsSTAR_LIST tempList;
    
    // Create an iterator on the scenario entries
    _entryIterator = _entryList.begin();
    
    // For each entry
    while (_entryIterator != _entryList.end())
    {
        // Begin to clean the temporary list
        if (tempList.Clear() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Copy the list input into the temporary list
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
                strcat(timLogActionName, "_PRIMARY");
            }
            else
            {
                strcat(timLogActionName, "_SECONDARY");
            }

            // Write the current action in the shared database
            mcsSTRING256 message;
            snprintf(message, sizeof(message), "1\t%s\t%d\t%d", catalogName, (_catalogIndex + 1), _nbOfCatalogs);
            
            if (_progress->Write(message) == mcsFAILURE)
            {
                goto errCond;
            }

            // Check if the list is not NULL, i.e the SetCatalogList has
            // ever been executed one time
            if (_catalogList == NULL)
            {
                errAdd(vobsERR_CATALOG_LIST_EMPTY);

                goto errCond;
            }

            // Get catalog from list
            vobsCATALOG* tempCatalog = _catalogList->Get(catalogName);
            if (tempCatalog == NULL)
            {
                errAdd(vobsERR_UNKNOWN_CATALOG);

                goto errCond;
            }

            // Get query option from scenario entry
            tempCatalog->SetOption(_entryIterator->GetQueryOption());
            
            vobsREQUEST* request = _entryIterator->_request;

            // Start time counter
            timlogWarningStart(timLogActionName);
            
            // if research failed, return mcsFAILURE and tempList is empty
            if (tempCatalog->Search(*request, tempList) == mcsFAILURE )
            {
                goto errCond;
            }

            // Stop time counter
            timlogStopTime(timLogActionName, &elapsedTime);
            
            sumSearchTime += elapsedTime;
            
            logTest("...number of stars return = %d", tempList.Size());
            
            _catalogIndex++;

            // Clean the catalog option
            tempCatalog->SetOption("");

            // If the verbose level is higher or equal to debug level, the back
            // result will be stored in file
            if (logIsStdoutLogLevel(logDEBUG) == mcsTRUE)
            {
                // This file will be stored in the $MCSDATA/tmp repository
                mcsSTRING256 logFileName;
                strcpy(logFileName, "$MCSDATA/tmp/");

                // Get band used for search
                const char* band = request->GetSearchBand();
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
                char* resolvedPath = miscResolvePath(logFileName);
                if (resolvedPath != NULL)
                {
                    // Save resulting list
                    if (tempList.Save(resolvedPath) == mcsFAILURE)
                    {
                        free(resolvedPath);
                        return mcsFAILURE;
                    }
                    free(resolvedPath);
                }
            }
        }

        // **** LIST COPYING/MERGING ****
        // There are 3 different action to do when the scenario is executed
        
        vobsSTAR_COMP_CRITERIA_LIST* criteriaList = _entryIterator->_criteriaList;
        vobsSTAR_LIST* outputList = _entryIterator->_listOutput;
        
        switch(_entryIterator->_action)
        {
            // First action is vobsCOPY. The list output will be clear and
            // it will be merge from the temporary list which contain 
            // the list input
            case vobsCOPY:
            {
                logTest("Performing COPY action on star list size = %d", tempList.Size());
        
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
                logTest("Performing MERGE action on star list size = %d", tempList.Size());
        
                if (outputList->Merge(tempList, criteriaList) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                logTest("after MERGE, star list size = %d", outputList->Size());
            
                break;
            }

            // Third action is vobsUPDATE_ONLY. The list output will
            // be merge from the temporary list, but this merge will
            // not modified the existant information of the list output
            case vobsUPDATE_ONLY:
            {
                logTest("Performing UPDATE_ONLY action on star list size = %d", tempList.Size());
        
                if (outputList->Merge(tempList, criteriaList, mcsTRUE) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                logTest("after UPDATE_ONLY, star list size = %d", outputList->Size());
                
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
 
    logInfo("vobsSCENARIO::Execute() - %d star(s) found.", starList.Size()); 

    mcsSTRING16 time;
    timlogFormatTime(sumSearchTime, &time);

    logWarning("Total time in catalog queries %s", time);
    
    _catalogIndex = 0;
    
    return mcsSUCCESS;
    
errCond:
    // Discard time counter
    timlogCancel(timLogActionName);

    return mcsFAILURE;
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
mcsCOMPL_STAT vobsSCENARIO::SetCatalogList(vobsCATALOG_LIST* catalogList)
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

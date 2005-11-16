/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.cpp,v 1.26 2005-11-16 10:47:54 scetre Exp $"
*
* History
* ------- 
* $Log: not supported by cvs2svn $
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

static char *rcsId="@(#) $Id: vobsSCENARIO.cpp,v 1.26 2005-11-16 10:47:54 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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

/*
 * Local Headers 
 */
#include "vobsSCENARIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/*
 * Class constructor
 */
vobsSCENARIO::vobsSCENARIO()
{
    _entryIterator = _entryList.begin();
    // affect to NULL the catalog list
    _catalogList = NULL;
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
 * 
 * @return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSCENARIO::AddEntry(mcsSTRING32   catalogName,
                                     vobsREQUEST   *request,
                                     vobsSTAR_LIST *listInput,
                                     vobsSTAR_LIST *listOutput,
                                     vobsACTION action,
                                     vobsSTAR_COMP_CRITERIA_LIST *criteriaList,
                                     vobsFILTER    *filter)
{
    logTrace("vobsSCENARIO::AddEntry()");
    
    // Create a new entry
    // Affect in this entry the catalogName, the list input, the list output, the
    // action to do, and the criteria list
    vobsSCENARIO_ENTRY entry(catalogName,
                             request,
                             listInput,
                             listOutput,
                             action,
                             criteriaList,
                             filter);
    
    // Put element in the list    
    _entryList.push_back(entry);

    return mcsSUCCESS;
}


/**
 * Execute the scenario
 *
 * The methods execute the scenario which had been loaded before. It will
 * read each entry and ask the specific catalog.
 * @param request vobsREQUEST representing the user constraints
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
    _entryIterator=_entryList.begin();
    // For each entry
    while (_entryIterator != _entryList.end())
    {
        vobsSTAR_COMP_CRITERIA_LIST *criteriaList;
        criteriaList = (*_entryIterator)._criteriaList;

        // begin to clean the temporary list
        if ( tempList.Clear() == mcsFAILURE )
        {
            return mcsFAILURE;
        }
        // Copy the list input in it
        if ((*_entryIterator)._listInput != NULL)
        {
            tempList.Copy(*(*_entryIterator)._listInput);
        }
        
        // Start research in entry's catalog
        logInfo("Consulting %s ...", (*_entryIterator)._catalogName);
        
        // define action for timlog trace
        mcsSTRING256 timLogActionName;
        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalog;
        strcpy (catalog, (*_entryIterator)._catalogName);
        if (miscReplaceChrByChr(catalog, '/', '_') == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        strcpy(timLogActionName, catalog);
        // Add request type (primary or not)
        if ((*_entryIterator)._listInput == NULL)
        {
            strcat(timLogActionName, "_1");
        }
        else
        {
            strcat(timLogActionName, "_2");
        }
        
        // Start time counter
        timlogInfoStart(timLogActionName);

        // Check if the list is not NULL, i.e the SetCatalogList has ever been
        // executed one time
        if (_catalogList == NULL)
        {
            errAdd(vobsERR_CATALOG_LIST_EMPTY);
            return mcsFAILURE;
        }
        
        vobsCATALOG *tempCatalog = 
            _catalogList->Get((*_entryIterator)._catalogName);
       
        vobsREQUEST *request = (*_entryIterator)._request;
        // if research failed, return mcsFAILURE
        if ((tempCatalog)->Search(*request, tempList) == mcsFAILURE )
        {
            return mcsFAILURE;
        }
        // Stop time counter
        timlogStop(timLogActionName);
        
        // If the verbose level is higher or equal to debug level, the back
        // result will be stored in file
        if (logGetStdoutLogLevel() >= logDEBUG)
        {
            // This file will be stored in the $MCSDATA/tmp repository
            mcsSTRING256 logFileName;
            const char   *resolvedPath;
            strcpy(logFileName, "$MCSDATA/tmp/");

            // Get band used for search
            const char *band;
            band = request->GetSearchBand();
            strcat(logFileName, band);

            // Get catalog name, and replace '/' by '_'
            mcsSTRING32 catalogName;
            strcpy (catalogName, ((*_entryIterator)._catalogName));
            if (miscReplaceChrByChr(catalogName, '/', '_') == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            strcat(logFileName, "_");
            strcat(logFileName, catalogName);

            // Add request type (primary or not)
            if ((*_entryIterator)._listInput == NULL)
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
      
        // There are 3 different action to do when the scenario is executed
        switch((*_entryIterator)._action)
        {
            // First action is vobsCOPY. The list output will be clear and it
            // will be merge from the temporary list which contain the list
            // input
            case vobsCOPY:
                {
                    if (((*_entryIterator)._listOutput)->Clear() == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    if (((*_entryIterator)._listOutput)->
                        Merge(tempList, criteriaList) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
            // second action is vobsMERGE. The list output will be merge from
            // the temporary list whitout being clear. The information which is
            // stored in the the list output is preserved and can be modified
            case vobsMERGE:
                {
                    if (((*_entryIterator)._listOutput)->
                         Merge(tempList, criteriaList) == mcsFAILURE )
                    {
                        return mcsFAILURE;
                    }
                }
            // third action is vobsUPDATE_ONLY. The list output will be merge
            // from thetemporary list, but this merge will not modified the
            // existant information of the list output
            case vobsUPDATE_ONLY:
                {

                    if ( ((*_entryIterator)._listOutput)->
                         Merge(tempList, criteriaList, mcsTRUE) == mcsFAILURE )
                    {
                        return mcsFAILURE;
                    }
                }
            default:
                break;
        }
        // Apply filter if neccessary
        if ((*_entryIterator)._filter != NULL)
        {
            ((*_entryIterator)._filter)->Apply((*_entryIterator)._listOutput);
        }

        //printf("%d star(s) found in catalog.\n", tempList.Size());         
        //printf("%d star(s) after merging.\n", (*_entryIterator)._listOutput->Size());         
        //(*_entryIterator)._listOutput->Display();
        _entryIterator++;
    }
   
    // Copy resulting list
    if (starList.Copy(*(_entryList.back())._listOutput) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logInfo("%d star(s) found.", starList.Size()); 
    
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
mcsCOMPL_STAT vobsSCENARIO::SetCatalogList(vobsCATALOG_LIST * catalogList)
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

    return mcsSUCCESS;
}
/*___oOo___*/

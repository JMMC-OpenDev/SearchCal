/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.cpp,v 1.16 2005-02-11 14:23:07 gluck Exp $"
*
* History
* ------- 
* $Log: not supported by cvs2svn $
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
 * \file
 * vobsSCENARIO class definition.
 * 
 */

static char *rcsId="@(#) $Id: vobsSCENARIO.cpp,v 1.16 2005-02-11 14:23:07 gluck Exp $"; 
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
 * \param catalog the catalog to ask corresponding to the entry
 * \param listInput list of star in enter of the research
 * \param listOutput list of star resulting of the asking of the catalog
 * \param action enumerate representing the action to do
 * \param criteriaList list of comparaison criteria
 * 
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSCENARIO::AddEntry(vobsCATALOG *catalog,
                                     vobsSTAR_LIST *listInput,
                                     vobsSTAR_LIST *listOutput,
                                     vobsACTION action,
                                     vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    logExtDbg("vobsSCENARIO::AddEntry()");
    
    // Create a new entry
    // Affect in this entry the catalog, the list input, the list output, the
    // action to do, and the criteria list
    vobsSCENARIO_ENTRY entry(catalog,
                             listInput,
                             listOutput,
                             action,
                             criteriaList);
    
    // Put element in the list    
    _entryList.push_back(entry);

    return mcsSUCCESS;
}


/**
 * Execute the scenario
 *
 * The methods execute the scenario which had been loaded before. It will
 * read each entry and ask the specific catalog.
 * \param request vobsREQUEST representing the user constraints
 * \param starList vobsSTAR_LIST which is the result of the interrogation,
 * this is the last list return of the last interrogation.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT vobsSCENARIO::Execute(vobsREQUEST &request, 
                                    vobsSTAR_LIST &starList)
{
    logExtDbg("vobsSCENARIO::Execute()");
    
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
        logInfo("Consulting %s ...", ((*_entryIterator)._catalog)->GetName());
        if (((*_entryIterator)._catalog)->Search(request,
                                                 tempList) == mcsFAILURE )
        {
            return mcsFAILURE;
        }
        
        // If the verbose level is higher or equal to debug level, the back
        // result will be stored in file
        if (logGetStdoutLogLevel() >= logDEBUG)
        {
            // This file will be stored in the $MCSDATA/tmp repository
            mcsSTRING256 logFileName;
            strcpy(logFileName, "$MCSDATA/tmp/");

            // Get band used for search
            const char *band;
            band = request.GetSearchBand();
            strcat(logFileName, band);

            // Get catalog name, and replace '/' by '_'
            mcsSTRING32 catalogName;
            strcpy (catalogName, ((*_entryIterator)._catalog)->GetName());
            miscReplaceChrByChr(catalogName, '/', '_');
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

            // Save resulting list
            tempList.Save(logFileName);
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

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    08-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSCENARIO class definition.
 * 
 */

static char *rcsId="@(#) $Id: vobsSCENARIO.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $"; 
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
 * \param ra interval right acscension
 * \param dec interval declinaison
 * 
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT vobsSCENARIO::AddEntry( vobsCATALOG *catalog,
                                      vobsSTAR_LIST *listInput,
                                      vobsSTAR_LIST *listOutput,
                                      vobsACTION action,
                                      float ra,
                                      float dec )
{
    logExtDbg("vobsSCENARIO::AddEntry(0x%x, 0x%x, 0x%x, %d)", catalog, listInput, listOutput, action);
    vobsSCENARIO_ENTRY entry;
    
    entry.catalog=catalog;
    entry.listInput=listInput;
    entry.listOutput=listOutput;
    entry.action=action;
    entry.deltaRa=ra;
    entry.deltaDec=dec;

    // Put element in the list    
    _entryList.push_back(entry);

    return SUCCESS;
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned 
 */
mcsCOMPL_STAT vobsSCENARIO::Execute(vobsREQUEST &request, 
                                    vobsSTAR_LIST &starList)
{
    logExtDbg("vobsSCENARIO::Execute()");
    vobsSTAR_LIST tempList;
    _entryIterator=_entryList.begin();
    // For each entry
    while (_entryIterator != _entryList.end())
    {
        if ( tempList.Clear() == FAILURE )
        {
            return FAILURE;
        }
        if ((*_entryIterator).listInput != NULL)
        {
            tempList.Copy(*(*_entryIterator).listInput);
        }
        
        // start research in entry's catalog
        if (((*_entryIterator).catalog)->Search(request, tempList) == FAILURE )
        {
            return FAILURE;
        }
       
        if (logGetStdoutLogLevel() >= logDEBUG)
        {
            mcsSTRING256 logFileName;
            strcpy(logFileName, "$MCSDATA/tmp/");

            // Get band used for search
            mcsSTRING32 band;
            request.GetConstraint(OBSERVED_BAND_ID,band);
            strcat(logFileName, band);

            // Get catalog name, and replace '/' by '_'
            mcsSTRING32 catalogName;
            ((*_entryIterator).catalog)->GetName(catalogName);
            miscReplaceChrByChr(catalogName, '/', '_');
            strcat(logFileName, "_");
            strcat(logFileName, catalogName);

            // Add request type (primary or not)
            if ((*_entryIterator).listInput == NULL)
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

        switch((*_entryIterator).action)
        {
            case COPY:
                {
                    if (((*_entryIterator).listOutput)->Clear() 
                        == FAILURE)
                    {
                        return FAILURE;
                    }
                    if (((*_entryIterator).listOutput)->Merge(tempList) 
                        == FAILURE)
                    {
                        return FAILURE;
                    }
                }
            case MERGE:
                {
                    if ( ((*_entryIterator).listOutput)->Merge(tempList,
                                                          (*_entryIterator).deltaRa,
                                                          (*_entryIterator).deltaDec) 
                         == FAILURE )
                    {
                        return FAILURE;
                    }
                }
            case UPDATE_ONLY:
                {
                    
                    if ( ((*_entryIterator).listOutput)->Merge(tempList,
                                                          (*_entryIterator).deltaRa,
                                                          (*_entryIterator).deltaDec,
                                                          mcsTRUE)
                         == FAILURE )
                    {
                        return FAILURE;
                    }
                }
            default:
                break;
        }
        _entryIterator++;
    }
    
    if (starList.Copy(*(_entryList.back()).listOutput) == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/

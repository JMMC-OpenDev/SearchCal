/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.C,v 1.1 2004-09-14 08:58:59 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    08-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Brief description of the class file, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class file follows here.
 *
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the program. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa <entity to refer>
 * 
 * \bug OPTIONAL. Bugs list if it exists.
 * \bug For example, description of the first bug
 * \bug For example, description of the second bug
 * 
 * \todo OPTIONAL. Things to forsee list, if needed. 
 * \todo Action1 : description of action 1.
 * 
 */

static char *rcsId="@(#) $Id: vobsSCENARIO.C,v 1.1 2004-09-14 08:58:59 scetre Exp $"; 
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
        if ((*_entryIterator).listInput != NULL)
        {
            tempList.Copy(*(*_entryIterator).listInput);
        }
        else 
        {
            if ( tempList.Clear() == FAILURE )
            {
                return FAILURE;
            }
        }

        // start research in entry's catalog
        if (((*_entryIterator).catalog)->Search(request, tempList) == FAILURE )
        {
            return FAILURE;
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

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_COMP_CRITERIA_LIST.cpp,v 1.4 2005-01-24 13:53:33 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    14-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR_COMP_CRITERIA_LIST class definition.
 */

static char *rcsId="@(#) $Id: vobsSTAR_COMP_CRITERIA_LIST.cpp,v 1.4 2005-01-24 13:53:33 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <map>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
/**
 * Class constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST()
{
    _criteriaIterator = _criteriaList.begin();    
}
/**
 * Copy Constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST(vobsSTAR_COMP_CRITERIA_LIST &criteriaList)
{
    // For each criteria of the criteria list in parameter
    map<char *, float> ::iterator criteriaIterator;
    for (criteriaIterator = _criteriaList.begin();
         criteriaIterator != _criteriaList.end(); criteriaIterator++)
    {
        // Copy it in the criteria list
        _criteriaList[(*criteriaIterator).first] = 
            criteriaList._criteriaList[(*criteriaIterator).first];
    }
}
/**
 * Class destructor
 */
vobsSTAR_COMP_CRITERIA_LIST::~vobsSTAR_COMP_CRITERIA_LIST()
{
}

/**
 * Assignment operator
 */
vobsSTAR_COMP_CRITERIA_LIST&vobsSTAR_COMP_CRITERIA_LIST::operator=
(vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    logExtDbg("vobsSTAR_COMP_CRITERIA_LIST::operator=()"); 
    // For each criteria
    map<char *, float> ::iterator criteriaIterator;
    for (criteriaIterator = _criteriaList.begin();
         criteriaIterator != _criteriaList.end(); criteriaIterator++)
    {
        _criteriaList[(*criteriaIterator).first] = 
            criteriaList._criteriaList[(*criteriaIterator).first];
    }
    return *this;
}

/*
 * Public methods
 */
/**
 * Add a criteria in the list
 *
 * \param propertyId the identifier of property.
 * \param range the range value of the criteria.
 * 
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Add(char *propertyId,
                                               mcsFLOAT range)
{
    logExtDbg("vobsSTAR_COMP_CRITERIA_LIST::Add()");
    
    // Put criteria in the list
    _criteriaList[propertyId]=range;

    return mcsSUCCESS;
}

/**
 * Returns the next criteria in the list.
 *
 * This method returns the identifier end the value of the next criteria of
 * the list. If \em
 * init is mcsTRUE, it returns the first criteria of the list.
 * 
 * This method can be used to move forward in the list, as shown below:
 * \code
 *     for (unsigned int el = 0; el < criteriaList.Size(); el++)
 *     {
 *         criteriaList.GetNextCriteria(propertyId,
 *                                      &range,
 *                                      (mcsLOGICAL)(el==0));
 *     }
 * \endcode
 * 
 * \return 
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::GetNextCriteria(char *propertyId,
                                                           mcsFLOAT *range,
                                                           mcsLOGICAL init)
{
    logExtDbg("vobsSTAR_COMP_CRITERIA_LIST::GetNextCriteria()");
    // if init == mcsTRUE the wanted criteria is the first of the list
    if (init == mcsTRUE)
    {
        _criteriaIterator = _criteriaList.begin();
    }
    else
    {
        _criteriaIterator++;
        if (_criteriaIterator == _criteriaList.end())
        {
            errAdd(vobsERR_NO_MORE_CRITERIA);
            return mcsFAILURE;
        }
    }
    strcpy(propertyId, (*_criteriaIterator).first);
    *range = (*_criteriaIterator).second;
    return mcsSUCCESS;
}

/**
 * Get the size of the list
 *
 * \return the number of element in the list
 **/
int vobsSTAR_COMP_CRITERIA_LIST::Size()
{
    return _criteriaList.size();
}
/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/

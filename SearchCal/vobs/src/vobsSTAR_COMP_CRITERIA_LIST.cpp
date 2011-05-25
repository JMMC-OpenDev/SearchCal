/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_COMP_CRITERIA_LIST class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsSTAR_COMP_CRITERIA_LIST.cpp,v 1.13 2011-03-03 13:09:43 lafrasse Exp $"; 

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
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
/**
 * Class constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST()
{
    // Put the criteria list iterator in first position of the list
    _criteriaIterator = _criteriaList.begin();    
}

/**
 * Copy Constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST
    (vobsSTAR_COMP_CRITERIA_LIST &criteriaList)
{
    // operator =
    *this = criteriaList;
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
(const vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    logTrace("vobsSTAR_COMP_CRITERIA_LIST::operator=()"); 
    // Copy it in the criteria list
    _criteriaList = criteriaList._criteriaList;
    return *this;
}

/*
 * Public methods
 */
/**
 * Clear the criteria list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Clear(void)
{
    logTrace("vobsSTAR_COMP_CRITERIA_LIST::Clear()");

    _criteriaList.erase(_criteriaList.begin(), _criteriaList.end());

    return mcsSUCCESS;
}

/**
 * Add a criteria in the list
 *
 * @param propertyId the identifier of property.
 * @param range the range value of the criteria used when comparing float
 * property. It is ignored when comparing string.
 * 
 * @return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Add(char *propertyId,
                                               mcsDOUBLE range)
{
    logTrace("vobsSTAR_COMP_CRITERIA_LIST::Add()");
  
    // create a star
    vobsSTAR star;
    // this star gave method to check that a property is known
    // If criteria is not a property return failure
    if (star.IsProperty(propertyId) == mcsFALSE)
    {
        errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId); 
        return mcsFAILURE;
    }

    // Put criteria in the list
    _criteriaList[propertyId]=range;

    return mcsSUCCESS;
}

/**
 * Method to remove a criteria from the list of criteria
 *
 * @param propertyId the id of the criteria to removed
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Remove(char *propertyId)
{
    // create a star
    vobsSTAR star;
    // this star gave method to check that a property is known
    // If criteria is not a property return failure
    if (star.IsProperty(propertyId) == mcsFALSE)
    {
        errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId); 
        return mcsFAILURE;
    }

    // Remove criteria from the list
    _criteriaList.erase(propertyId);

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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_NO_MORE_CRITERIA
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::GetNextCriteria(char *propertyId,
                                                           mcsDOUBLE *range,
                                                           mcsLOGICAL init)
{
    logTrace("vobsSTAR_COMP_CRITERIA_LIST::GetNextCriteria()");
    // if init == mcsTRUE the wanted criteria is the first of the list
    if (init == mcsTRUE)
    {
        _criteriaIterator = _criteriaList.begin();
    }
    // else, the value wanted is the next value of the list after the iterator
    else
    {
        _criteriaIterator++;
    }

    // If the the criteria iterator is at the end of the list, there are no
    // more criteria in the list and return a failure
    if (_criteriaIterator == _criteriaList.end())
    {
        errAdd(vobsERR_NO_MORE_CRITERIA);
        return mcsFAILURE;
    }

    // copy the criteria name found and get the value of the range
    strcpy(propertyId, (*_criteriaIterator).first);
    *range = (*_criteriaIterator).second;
    
    return mcsSUCCESS;
}

/**
 * Get the size of the list
 *
 * @return the number of element in the list
 **/
int vobsSTAR_COMP_CRITERIA_LIST::Size(void)
{
    return _criteriaList.size();
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.9 2005-11-28 10:11:24 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/11/24 13:17:34  scetre
 * Updated documentation
 *
 * Revision 1.7  2005/11/24 08:15:01  scetre
 * Changed to generic filter class
 *
 * Revision 1.6  2005/11/23 10:22:20  scetre
 * Generalized filter
 *
 * Revision 1.5  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.4  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/05 15:52:36  gzins
 * Change Enabled and Disabled to Enable and Disable
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:36:27  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsFILTER class.
 */

static char *rcsId="@(#) $Id: vobsFILTER.cpp,v 1.9 2005-11-28 10:11:24 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "vobsFILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsFILTER::vobsFILTER()
{
    // by default, the filter is disabled
    Disable();
}

/**
 * Class destructor
 */
vobsFILTER::~vobsFILTER()
{
}

/*
 * Public methods
 */
/**
 * Set property id
 *
 * @param ucd property id
 * 
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::SetPropertyId(mcsSTRING32 ucd)
{
    logTrace("vobsFILTER::SetPropertyId()");

    // copy ucd id
    strcpy(_ucd, ucd);
    
    return mcsSUCCESS;
}

/**
 * Add float condition
 *
 * @param condition <, >, <=, >=, ==
 * @param value the value to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsFILTER::AddCondition(vobsCONDITION condition,
                                       mcsFLOAT value)
{
    logTrace("vobsFILTER::AddCondition(float)");

    vobsSTAR star;
    if (star.GetPropertyType(_ucd) == vobsSTRING_PROPERTY)
    {
        // errAdd
        return mcsFAILURE;
    }
    
    vobsFLOAT_CONDITION floatCondition;
    floatCondition.condition = condition;
    floatCondition.value = value;

    _floatConditions.push_back(floatCondition);

    return mcsSUCCESS;
}

/**
 * Add String condition
 * 
 * @param the string to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsFILTER::AddCondition(string value)
{
    logTrace("vobsFILTER::AddCondition(string)");

    vobsSTAR star;
    if (star.GetPropertyType(_ucd) == vobsFLOAT_PROPERTY)
    {
        // errAdd
        return mcsFAILURE;
    }

    _stringConditions.push_back(value);
    
    return mcsSUCCESS;
}


/**
 * Apply filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsFILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsFILTER::Apply()");

    mcsLOGICAL isRespectedCondition;
    
    if (IsEnabled() == mcsTRUE)
    {
        vobsSTAR *star;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            // by default star doesn't respected condition
            isRespectedCondition = mcsFALSE;

            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // If property not set remove the star
            if (star->IsPropertySet(_ucd) != mcsTRUE)
            {
                // Remove it
                logTest("star %d had no property %s",
                        el+1, _ucd);
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;
            }
            else
            {
                // if list of float condition is not empty, it is a float
                // property, the test will be on numerical value
                if (_floatConditions.size() != 0)
                {
                    // in the case of float property the star respect the
                    // confition and it is changed to false if one property
                    // doesn't
                    isRespectedCondition = mcsTRUE;
                    // get float value of the property
                    mcsFLOAT value;
                    if (star->GetPropertyValue(_ucd, &value) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    // if property is set, Apply sequentially all the condition
                    // check all float condition
                    _floatConditionsIterator = _floatConditions.begin();
                    while (_floatConditionsIterator != _floatConditions.end())
                    {
                        // According to the desire condition 
                        vobsCONDITION condition = 
                            (*_floatConditionsIterator).condition;
                        // switch case
                        switch(condition)
                        {
                            case vobsLESS:
                                // If value is less or equal than to the wanted
                                // value condition
                                if (value  >= (*_floatConditionsIterator).value)
                                {
                                    isRespectedCondition = mcsFALSE;
                                }
                                break;

                            case vobsLESS_EQUAL:
                                // If value is less than to the wanted
                                // value condition
                                if (value  > (*_floatConditionsIterator).value)
                                {
                                    isRespectedCondition = mcsFALSE;
                                }

                                break;

                            case vobsMORE:
                                // If value is more or equal than to the wanted
                                // value condition
                                if (value  < (*_floatConditionsIterator).value)
                                {
                                    isRespectedCondition = mcsFALSE;
                                }
                                break;

                            case vobsMORE_EQUAL:
                                // If value is more than to the wanted
                                // value condition
                                if (value < (*_floatConditionsIterator).value)
                                {
                                    isRespectedCondition = mcsFALSE;
                                }
                                break;

                            case vobsEQUAL:
                                // If value is not equal to the wanted value 
                                // condition
                                if (value != (*_floatConditionsIterator).value)
                                {
                                    isRespectedCondition = mcsFALSE;
                                }
                                break;

                            default:
                                break;
                        }
                        _floatConditionsIterator++;
                    }
                } 
                else if (_stringConditions.size() != 0)
                {
                    isRespectedCondition = mcsFALSE;
                    // retreive the string property of the testing star
                    mcsSTRING32 value;
                    strcpy(value, star->GetPropertyValue(_ucd));
                    // if property is set, Apply sequentially all the condition
                    // check all string condition
                    _stringConditionsIterator = _stringConditions.begin();
                    while (_stringConditionsIterator != _stringConditions.end())
                    {
                        // in string case, the value should be the same
                        if (strcmp(value,
                                   (*_stringConditionsIterator).c_str()) == 0)
                        {
                            isRespectedCondition = mcsTRUE;
                        }
                        _stringConditionsIterator++;
                    }
                }

                if (isRespectedCondition == mcsFALSE)
                {
                    // Remove it
                    if (list->Remove(*star) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    el = el-1;            
                }
            }
        }
    }
    
    return mcsSUCCESS;
}


/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsGENERIC_FILTER.cpp,v 1.3 2005-12-07 12:22:51 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/29 13:51:43  gzins
 * Minor change related to variable name
 *
 * Revision 1.1  2005/11/29 10:34:12  gzins
 * Moved vobsFILTER to vobsGENERIC_FILTER
 *
 * Revision 1.9  2005/11/28 10:11:24  scetre
 * Changed OR test to AND test
 *
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
 *  Definition of vobsGENERIC_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsGENERIC_FILTER.cpp,v 1.3 2005-12-07 12:22:51 gzins Exp $"; 
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
#include "vobsGENERIC_FILTER.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/**
 * Class constructor
 *
 * The exprType parameter indicates whether all conditions of this filter have
 * to be verified by star or only one is enough; i.e. if it is set to vobsAND
 * the expression is evaluated as 'cond1 && cond2 && ...', and if it is set to
 * vobsOR it is evaluated as 'cond1 || cond2 || ...'
 *
 * @param propId property id on which filter has to be applied
 * @param exprType type of expression; vobsAND or vobsOR
 */
vobsGENERIC_FILTER::vobsGENERIC_FILTER(char *propId, 
                                       vobsEXPRESSION_TYPE exprType)
{
    // Copy the name of property
    strncpy(_propId, propId, (sizeof(_propId) - 1));
    
    // and expression type
    _exprType = exprType;

    // By default, the filter is disabled
    Disable();
}

/**
 * Class destructor
 */
vobsGENERIC_FILTER::~vobsGENERIC_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Add condition on numerical value
 *
 * @param condition <, >, <=, >=, ==
 * @param value the value to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsGENERIC_FILTER::AddCondition(vobsOPERATOR op,
                                               mcsFLOAT value)
{
    logTrace("vobsGENERIC_FILTER::AddCondition(float)");

    // If condition list is not empty, check that new condition has same type
    // (float or string) than the other conditions
    if (_conditions.empty() == false)
    {
        if (_propType != vobsFLOAT_PROPERTY)
        {
            errAdd(vobsERR_CONDITION_TYPE, "float", "string");
            return mcsFAILURE;
        }
    }
    else
    {
        _propType = vobsFLOAT_PROPERTY;
    }

    // Add new condition to the list
    vobsCONDITION condition(op, value);;
    _conditions.push_back(condition);

    return mcsSUCCESS;
}

/**
 * Add condition on string value
 * 
 * @param condition <, >, <=, >=, ==
 * @param value the string to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsGENERIC_FILTER::AddCondition(vobsOPERATOR op,
                                               char *value)
{
    logTrace("vobsGENERIC_FILTER::AddCondition(string)");

    // If condition list is not empty, check that new condition has same type
    // (float or string) than the other conditions
    if (_conditions.empty() == false)
    {
        if (_propType != vobsSTRING_PROPERTY)
        {
            errAdd(vobsERR_CONDITION_TYPE, "string", "float");
            return mcsFAILURE;
        }
    }
    else
    {
        _propType = vobsSTRING_PROPERTY;
    }

    // Add new condition to the list
    vobsCONDITION condition(op, value);;
    _conditions.push_back(condition);

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
mcsCOMPL_STAT vobsGENERIC_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsGENERIC_FILTER::Apply()");

    // If list is empty, return
    if (list->IsEmpty() == mcsTRUE)
    {
        return mcsSUCCESS;
    }

    // If condition list is empty, return
    if (_conditions.size() == 0)
    {
        return mcsSUCCESS;
    }

    // Check property Id
    vobsSTAR *star;
    star = (vobsSTAR *)list->GetNextStar(mcsTRUE);
    vobsSTAR_PROPERTY *property;
    property = star->GetProperty(_propId);
    if (property == NULL)
    {
        return mcsFAILURE;
    }

    // Check compatibility between property type and filter
    if (star->GetPropertyType(_propId) != _propType)
    {
        if (_propType == vobsFLOAT_PROPERTY)
        {
            errAdd(vobsERR_INCOMPATIBLE_TYPES, "string", "float");
        }
        else
        {
            errAdd(vobsERR_INCOMPATIBLE_TYPES, "float", "string");
        }
        return mcsFAILURE;
    }

    // If filter is enabled 
    if (IsEnabled() == mcsTRUE)
    {
        // For each star of the list
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            bool expr;
            
            // Init expresion eveluation status 
            if (_exprType == vobsOR)
            {
                expr = false;
            }
            else
            {
                expr = true;
            }

            star = (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));

            // If property not set remove the star
            if (star->IsPropertySet(_propId) != mcsTRUE)
            {
                // Remove it
                logTest("star %d - property %s is not set", el + 1, _propId);
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;
            }
            else
            {
                mcsFLOAT numValue;
                string   strValue;
                if (_propType == vobsFLOAT_PROPERTY)
                {
                    if (star->GetPropertyValue(_propId, 
                                               &numValue) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
                else
                {
                    strValue = star->GetPropertyValue(_propId);
                }

                // Evaluate all conditions
                std::list<vobsCONDITION>::iterator iter;
                for (iter = _conditions.begin(); 
                     iter != _conditions.end(); iter++)
                {
                    bool condition;

                    if (_propType == vobsFLOAT_PROPERTY)
                    {
                        condition = (*iter).Evaluate(numValue);
                    }
                    else
                    {
                        condition = (*iter).Evaluate(strValue);
                    }


                    if (_exprType == vobsOR)
                    {
                        expr = (expr || condition);
                    }
                    else
                    {
                        expr = expr && condition;
                    }
                }

                // If exression is false, remove star
                if (expr == false)
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

// Private class
/**
 * Class constructor
 */
vobsGENERIC_FILTER::vobsCONDITION::vobsCONDITION(vobsOPERATOR op, 
                                                 mcsFLOAT operand)
{
    _operator = op;
    _numOperand = operand;
}

vobsGENERIC_FILTER::vobsCONDITION::vobsCONDITION(vobsOPERATOR op,
                                                 char *operand)
{
    _operator = op;
    _strOperand = operand;
}

/**
 * Class destructor
 */
vobsGENERIC_FILTER::vobsCONDITION::~vobsCONDITION()
{
}

/**
 * Condition evaluators.
 */
bool vobsGENERIC_FILTER::vobsCONDITION::Evaluate(mcsFLOAT value)
{
    switch (_operator)
    {
        case vobsLESS:
            if (value < _numOperand)
            {
                return true;
            }
            break;
        case vobsLESS_OR_EQUAL:
            if (value <= _numOperand)
            {
                return true;
            }
            break;
        case vobsGREATER:
            if (value > _numOperand)
            {
                return true;
            }
            break; 
        case vobsGREATER_OR_EQUAL:
            if (value >= _numOperand)
            {
                return true;
            }
            break; 
        case vobsEQUAL:
            if (value == _numOperand)
            {
                return true;
            }
            break; 
        case vobsNOT_EQUAL:
            if (value != _numOperand)
            {
                return true;
            }
            break; 
        default:
            break;
    }
    return false;
}

bool vobsGENERIC_FILTER::vobsCONDITION::Evaluate(string value)
{
    switch (_operator)
    {
        case vobsLESS:
            if (value < _strOperand)
            {
                return true;
            }
            break;
        case vobsLESS_OR_EQUAL:
            if (value <= _strOperand)
            {
                return true;
            }
            break;
        case vobsGREATER:
            if (value > _strOperand)
            {
                return true;
            }
            break; 
        case vobsGREATER_OR_EQUAL:
            if (value >= _strOperand)
            {
                return true;
            }
            break; 
        case vobsEQUAL:
            if (value == _strOperand)
            {
                return true;
            }
            break; 
        case vobsNOT_EQUAL:
            if (value != _strOperand)
            {
                return true;
            }
            break; 
        default:
            break;
    }
    return false;
}

/*___oOo___*/

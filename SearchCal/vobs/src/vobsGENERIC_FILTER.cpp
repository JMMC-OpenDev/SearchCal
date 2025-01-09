/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsGENERIC_FILTER class.
 */

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
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
 * @param filterId filter identifier
 * @param propId property id on which filter has to be applied
 * @param exprType type of expression; vobsAND or vobsOR
 */
vobsGENERIC_FILTER::vobsGENERIC_FILTER(const char* filterId,
                                       const char* propId,
                                       const vobsEXPRESSION_TYPE exprType)
: vobsFILTER(filterId)
{
    // Copy the name of property
    strncpy(_propId, propId, (sizeof (_propId) - 1));

    // and expression type
    _exprType = exprType;
}

/**
 * Class destructor
 */
vobsGENERIC_FILTER::~vobsGENERIC_FILTER()
{
    // Free all conditions
    for (vobsCONDITION_PTR_LIST::iterator iter = _conditions.begin(); iter != _conditions.end(); iter++)
    {
        delete(*iter);
    }
}

/*
 * Public methods
 */

/**
 * Add condition on numerical value
 *
 * @param op <, >, <=, >=, ==
 * @param value the value to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsGENERIC_FILTER::AddCondition(const vobsOPERATOR op,
                                               const mcsDOUBLE value)
{
    // If condition list is not empty, check that new condition has same type
    // (mcsDOUBLE or string) than the other conditions
    if (!_conditions.empty())
    {
        FAIL_COND_DO((_propType == vobsSTRING_PROPERTY), 
                     errAdd(vobsERR_CONDITION_TYPE, "double", "string"));
    }
    else
    {
        _propType = vobsFLOAT_PROPERTY;
    }

    vobsCONDITION* condition = new vobsCONDITION(op, value);

    // Add new condition to the list
    _conditions.push_back(condition);

    return mcsSUCCESS;
}

/**
 * Add condition on string value
 *
 * @param op <, >, <=, >=, ==
 * @param value the string to compare with
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsGENERIC_FILTER::AddCondition(const vobsOPERATOR op,
                                               const char *value)
{
    // If condition list is not empty, check that new condition has same type
    // (mcsDOUBLE or string) than the other conditions
    if (!_conditions.empty())
    {
        FAIL_COND_DO((_propType != vobsSTRING_PROPERTY), 
                     errAdd(vobsERR_CONDITION_TYPE, "string", "double"));
    }
    else
    {
        _propType = vobsSTRING_PROPERTY;
    }

    vobsCONDITION* condition = new vobsCONDITION(op, value);

    // Add new condition to the list
    _conditions.push_back(condition);

    return mcsSUCCESS;
}

/**
 * Apply filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsGENERIC_FILTER::Apply(vobsSTAR_LIST *list)
{
    // If list is empty, return
    if (IS_TRUE(list->IsEmpty()))
    {
        return mcsSUCCESS;
    }

    // If condition list is empty, return
    if (_conditions.empty())
    {
        return mcsSUCCESS;
    }

    // Check property Id
    vobsSTAR* starPtr = list->GetNextStar(mcsTRUE);

    vobsSTAR_PROPERTY* property = starPtr->GetProperty(_propId);
    FAIL_NULL(property);

    // Check compatibility between property type and filter
    // TODO: fix mappings (vobsINT_PROPERTY, vobsBOOL_PROPERTY)
    if ((_propType == vobsSTRING_PROPERTY) && (starPtr->GetPropertyType(_propId) != _propType))
    {
        if (_propType == vobsFLOAT_PROPERTY)
        {
            errAdd(vobsERR_INCOMPATIBLE_TYPES, "string", "double");
        }
        else
        {
            errAdd(vobsERR_INCOMPATIBLE_TYPES, "double", "string");
        }
        return mcsFAILURE;
    }

    // If filter is enabled
    if (IS_TRUE(IsEnabled()))
    {
        mcsDOUBLE numValue;
        string strValue;

        // For each star of the given star list
        // note: Remove() and GetNextStar() ensure proper list traversal:
        for (starPtr = list->GetNextStar(mcsTRUE); IS_NOT_NULL(starPtr); starPtr = list->GetNextStar(mcsFALSE))
        {
            // Get Star ID
            mcsSTRING64 starId;
            FAIL(starPtr->GetId(starId, sizeof (starId)));

            bool expr;

            // Init expression evaluation status
            if (_exprType == vobsOR)
            {
                expr = false;
            }
            else
            {
                expr = true;
            }

            // If property not set remove the star
            if (IS_FALSE(starPtr->IsPropertySet(_propId)))
            {
                // Remove it
                logTest("star '%s' has been removed by the filter '%s' : property %s is not set", starId, GetId(), _propId);

                list->RemoveRef(starPtr);
            }
            else
            {
                if (_propType == vobsFLOAT_PROPERTY)
                {
                    FAIL(starPtr->GetPropertyValue(_propId, &numValue));
                }
                else
                {
                    strValue = starPtr->GetPropertyValue(_propId);
                }

                // Evaluate all conditions
                for (vobsCONDITION_PTR_LIST::iterator iter = _conditions.begin(); iter != _conditions.end(); iter++)
                {
                    bool condition;

                    if (_propType == vobsFLOAT_PROPERTY)
                    {
                        condition = (*iter)->Evaluate(numValue);
                    }
                    else
                    {
                        condition = (*iter)->Evaluate(strValue);
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
                if (!expr)
                {
                    logDebug("star '%s' has been removed by the filter '%s'", starId, GetId());

                    // Remove it
                    list->RemoveRef(starPtr);
                }
                else
                {
                    logDebug("star '%s' valid for the filter '%s'", starId, GetId());
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
vobsGENERIC_FILTER::vobsCONDITION::vobsCONDITION(const vobsOPERATOR op,
                                                 const mcsDOUBLE operand)
{
    _operator = op;
    _numOperand = operand;
}

vobsGENERIC_FILTER::vobsCONDITION::vobsCONDITION(const vobsOPERATOR op,
                                                 const char *operand)
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
bool vobsGENERIC_FILTER::vobsCONDITION::Evaluate(const mcsDOUBLE value)
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

bool vobsGENERIC_FILTER::vobsCONDITION::Evaluate(const string& value)
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

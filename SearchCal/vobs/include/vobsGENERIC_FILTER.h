#ifndef vobsGENERIC_FILTER_H
#define vobsGENERIC_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsGENERIC_FILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <list>
#include <string.h>
using namespace std;

/*
 * MCS header
 */
#include "mcs.h"
#include "vobsSTAR_LIST.h"
#include "vobsFILTER.h"

/*
 * Enumeration type definition
 */

/**
 * vobsOPERATOR defines the list of logical operators supported by
 * vobsCONDITION.
 */
typedef enum
{
    vobsLESS,
    vobsLESS_OR_EQUAL,
    vobsGREATER,
    vobsGREATER_OR_EQUAL,
    vobsEQUAL,
    vobsNOT_EQUAL
} vobsOPERATOR;

/**
 * vobsEXPRESSION_TYPE defines the two possible expression types: OR and AND.
 */
typedef enum
{
    vobsAND,
    vobsOR
} vobsEXPRESSION_TYPE;

/*
 * Class declaration
 */

/**
 * Filter class
 *
 * Generic class to filter a list of star on a given property, using conditions
 * such as equal to, less than, greater than and so on.
 */
class vobsGENERIC_FILTER : public vobsFILTER
{
public:
    // Class constructor
    vobsGENERIC_FILTER(const char* filterId,
                       const char* propId,
                       vobsEXPRESSION_TYPE exprType = vobsAND);

    // Class destructor
    virtual ~vobsGENERIC_FILTER();

    virtual mcsCOMPL_STAT AddCondition(const vobsOPERATOR op, const mcsDOUBLE value);
    virtual mcsCOMPL_STAT AddCondition(const vobsOPERATOR op, const char *value);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:

private:

    class vobsCONDITION
    {
    public:

        // Class constructor
        vobsCONDITION(const vobsOPERATOR op, const mcsDOUBLE operand = 0.0);
        vobsCONDITION(const vobsOPERATOR op, const char *operand);

        // Class destructor
        virtual ~vobsCONDITION();

        // Condition evaluation
        bool Evaluate(const mcsDOUBLE value);
        bool Evaluate(const string& value);

    protected:

        // Declaration of copy constructor and assignment operator as private
        // methods, in order to hide them from the users.
        vobsCONDITION(const vobsCONDITION&);
        vobsCONDITION& operator=(const vobsCONDITION&);

        vobsOPERATOR _operator;
        mcsDOUBLE _numOperand;
        string _strOperand;

    private:
    };

    /*
     * Type declaration
     */

    /** Condition pointer ordered list */
    typedef std::list<vobsCONDITION*> vobsCONDITION_PTR_LIST;


    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsGENERIC_FILTER(const vobsGENERIC_FILTER&);
    vobsGENERIC_FILTER& operator=(const vobsGENERIC_FILTER&);

    mcsSTRING32 _propId;
    vobsEXPRESSION_TYPE _exprType;
    vobsPROPERTY_TYPE _propType;

    vobsCONDITION_PTR_LIST _conditions;
};

#endif /*!vobsGENERIC_FILTER_H*/

/*___oOo___*/

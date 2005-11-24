#ifndef vobsFILTER_H
#define vobsFILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.h,v 1.7 2005-11-24 08:15:01 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/11/23 10:22:21  scetre
 * Generalized filter
 *
 * Revision 1.5  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.4  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/05 15:52:41  gzins
 * Change Enabled and Disabled to Enable and Disable
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:35:59  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsFILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <list>
/*
 * MCS header
 */
#include "mcs.h"
#include "vobsSTAR_LIST.h"
#include "vobsBASE_FILTER.h"

/*
 * Class declaration
 */

/*
 * Enumeration type definition
 */
/**
 * vobsCONDITION is an enumeration which allow correspondance between an id and
 * a condition.
 */
typedef enum
{
    vobsNO_CONDITION = -1,
    vobsLESS,
    vobsLESS_EQUAL,
    vobsMORE,
    vobsMORE_EQUAL,
    vobsEQUAL
} vobsCONDITION;

/**
 * structure of float condition
 */
typedef struct
{
    vobsCONDITION condition;
    mcsFLOAT value;
} vobsFLOAT_CONDITION;

/**
 * Filter class
 *
 * A filter is define by a status (enable or disable) and a filter property
 * which can be a numeric value, a string, a boolean
 */
class vobsFILTER : public vobsBASE_FILTER
{

public:
    // Class constructor
    vobsFILTER();

    // Class destructor
    virtual ~vobsFILTER();

    virtual mcsCOMPL_STAT SetPropertyId(mcsSTRING32 ucd);
    virtual mcsCOMPL_STAT AddCondition(vobsCONDITION condition,
                                       mcsFLOAT value);
    virtual mcsCOMPL_STAT AddCondition(string value);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    mcsSTRING32 _name;
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER(const vobsFILTER&);
    vobsFILTER& operator=(const vobsFILTER&);

    mcsSTRING32 _ucd;

    std::list<vobsFLOAT_CONDITION> _floatConditions;
    std::list<vobsFLOAT_CONDITION>::iterator _floatConditionsIterator;
    std::list<string> _stringConditions;
    std::list<string>::iterator _stringConditionsIterator;
};

#endif /*!vobsFILTER_H*/

/*___oOo___*/

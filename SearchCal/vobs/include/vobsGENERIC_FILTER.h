#ifndef vobsGENERIC_FILTER_H
#define vobsGENERIC_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsGENERIC_FILTER.h,v 1.1 2005-11-29 10:29:41 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/11/24 08:15:01  scetre
 * Changed to generic filter class
 *
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
 * Declaration of vobsGENERIC_FILTER class.
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
#include "vobsFILTER.h"

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
    vobsLESS,
    vobsLESS_OR_EQUAL,
    vobsGREATER,
    vobsGREATER_OR_EQUAL,
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
class vobsGENERIC_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsGENERIC_FILTER();

    // Class destructor
    virtual ~vobsGENERIC_FILTER();

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
    vobsGENERIC_FILTER(const vobsGENERIC_FILTER&);
    vobsGENERIC_FILTER& operator=(const vobsGENERIC_FILTER&);

    mcsSTRING32 _ucd;

    std::list<vobsFLOAT_CONDITION> _floatConditions;
    std::list<vobsFLOAT_CONDITION>::iterator _floatConditionsIterator;
    std::list<string> _stringConditions;
    std::list<string>::iterator _stringConditionsIterator;
};

#endif /*!vobsGENERIC_FILTER_H*/

/*___oOo___*/

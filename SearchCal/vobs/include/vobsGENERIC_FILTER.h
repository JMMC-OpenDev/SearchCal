#ifndef vobsGENERIC_FILTER_H
#define vobsGENERIC_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsGENERIC_FILTER.h,v 1.4 2005-12-13 16:30:33 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/12/07 12:22:43  gzins
 * Added vobsCONDITION class for easier expression evaluation
 *
 * Revision 1.2  2005/11/29 13:45:57  gzins
 * Removed filter name definition
 *
 * Revision 1.1  2005/11/29 10:29:41  gzins
 * Moved vobsFILTER to vobsGENERIC_FILTER
 *
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
    vobsGENERIC_FILTER(const char*         filterId,
                             char*         propId,
                       vobsEXPRESSION_TYPE exprType=vobsAND);

    // Class destructor
    virtual ~vobsGENERIC_FILTER();

    virtual mcsCOMPL_STAT AddCondition(vobsOPERATOR op, mcsFLOAT value);
    virtual mcsCOMPL_STAT AddCondition(vobsOPERATOR op, char *value);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    class vobsCONDITION
    {
    public:

        // Class constructor
        vobsCONDITION(vobsOPERATOR op, mcsFLOAT operand=0.0);
        vobsCONDITION(vobsOPERATOR op, char *operand);

        // Class destructor
        virtual ~vobsCONDITION();

        // Condition evaluation
        bool Evaluate(mcsFLOAT value);
        bool Evaluate(string   value);

    protected:
        vobsOPERATOR _operator;
        mcsFLOAT     _numOperand;
        string       _strOperand;

    private:
    };

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsGENERIC_FILTER(const vobsGENERIC_FILTER&);
    vobsGENERIC_FILTER& operator=(const vobsGENERIC_FILTER&);

    mcsSTRING32         _propId;
    vobsEXPRESSION_TYPE _exprType;
    vobsPROPERTY_TYPE   _propType;

    std::list<vobsCONDITION> _conditions;
};

#endif /*!vobsGENERIC_FILTER_H*/

/*___oOo___*/

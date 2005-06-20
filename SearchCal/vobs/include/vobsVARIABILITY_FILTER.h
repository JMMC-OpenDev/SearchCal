#ifndef vobsVARIABILITY_FILTER_H
#define vobsVARIABILITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVARIABILITY_FILTER.h,v 1.1 2005-06-20 11:31:53 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * Declaration of vobsVARIABILITY_FILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local header
 */
#include "vobsFILTER.h"
#include "vobsSTAR_LIST.h"

#define vobsVARIABILITY_FILTER_NAME "Filter by Variability"

/*
 * Class declaration
 */

/**
 * Variability filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsVARIABILITY_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsVARIABILITY_FILTER();

    // Class destructor
    virtual ~vobsVARIABILITY_FILTER();

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsVARIABILITY_FILTER(const vobsVARIABILITY_FILTER&);
    vobsVARIABILITY_FILTER& operator=(const vobsVARIABILITY_FILTER&);
};

#endif /*!vobsVARIABILITY_FILTER_H*/

/*___oOo___*/

#ifndef vobsMULTIPLICITY_FILTER_H
#define vobsMULTIPLICITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsMULTIPLICITY_FILTER.h,v 1.4 2005-11-29 13:45:57 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/11/29 10:28:46  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.2  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
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
 * Declaration of vobsMULTIPLICITY_FILTER class.
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

/*
 * Class declaration
 */

/**
 * Multiplicity filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsMULTIPLICITY_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsMULTIPLICITY_FILTER();

    // Class destructor
    virtual ~vobsMULTIPLICITY_FILTER();

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsMULTIPLICITY_FILTER(const vobsMULTIPLICITY_FILTER&);
    vobsMULTIPLICITY_FILTER& operator=(const vobsMULTIPLICITY_FILTER&);
};

#endif /*!vobsMULTIPLICITY_FILTER_H*/

/*___oOo___*/

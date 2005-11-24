#ifndef sclsvrVISIBILITY_FILTER_H
#define sclsvrVISIBILITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVISIBILITY_FILTER.h,v 1.5 2005-11-24 13:21:02 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.3  2005/06/20 14:42:11  scetre
 * Changed call to mother class from sclsvrFILTER to vobsFILTER after filter removed to vobs module
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
 * Declaration of sclsvrVISIBILITY_FILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"


/*
 * SCALIB header
 */
#include "vobs.h"


/*
 * Local header
 */
#include "sclsvrCALIBRATOR_LIST.h"


#define sclsvrVISIBILITY_FILTER_NAME "Filter by Visibility"


/*
 * Class declaration
 */
/**
 * Visibility filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa vobsBASE_FILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 */
class sclsvrVISIBILITY_FILTER : public vobsBASE_FILTER
{
public:
    // Class constructor
    sclsvrVISIBILITY_FILTER();

    // Class destructor
    virtual ~sclsvrVISIBILITY_FILTER();

    virtual mcsCOMPL_STAT SetVisibilityValue(mcsFLOAT visMax);

    virtual mcsCOMPL_STAT GetVisibilityValue(mcsFLOAT *visMax);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);    


protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrVISIBILITY_FILTER(const sclsvrVISIBILITY_FILTER&);
    sclsvrVISIBILITY_FILTER& operator=(const sclsvrVISIBILITY_FILTER&);

    mcsFLOAT _visMax;
};

#endif /*!sclsvrVISIBILITY_FILTER_H*/

/*___oOo___*/

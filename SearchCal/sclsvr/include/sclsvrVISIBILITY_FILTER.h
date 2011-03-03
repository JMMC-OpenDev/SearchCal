#ifndef sclsvrVISIBILITY_FILTER_H
#define sclsvrVISIBILITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVISIBILITY_FILTER.h,v 1.9 2011-03-03 13:12:51 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/12/14 09:02:00  scetre
 * Updated constructor with filter name
 *
 * Revision 1.7  2005/11/30 10:35:21  scetre
 * Updated Filter without name
 * Updated scenario
 *
 * Revision 1.6  2005/11/29 10:38:43  gzins
 * *** empty log message ***
 *
 * Revision 1.5  2005/11/24 13:21:02  scetre
 * Inherite from vobsBASE_FILTER instead of vobsFILTER
 *
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
class sclsvrVISIBILITY_FILTER : public vobsFILTER
{
public:
    // Class constructor
    sclsvrVISIBILITY_FILTER(const char *filterId);

    // Class destructor
    virtual ~sclsvrVISIBILITY_FILTER();

    virtual mcsCOMPL_STAT SetVisibilityValue(mcsDOUBLE visMax);

    virtual mcsCOMPL_STAT GetVisibilityValue(mcsDOUBLE *visMax);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);    


protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrVISIBILITY_FILTER(const sclsvrVISIBILITY_FILTER&);
    sclsvrVISIBILITY_FILTER& operator=(const sclsvrVISIBILITY_FILTER&);

    mcsDOUBLE _visMax;
};

#endif /*!sclsvrVISIBILITY_FILTER_H*/

/*___oOo___*/

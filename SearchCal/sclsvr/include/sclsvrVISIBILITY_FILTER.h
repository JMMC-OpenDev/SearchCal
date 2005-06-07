#ifndef sclsvrVISIBILITY_FILTER_H
#define sclsvrVISIBILITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVISIBILITY_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * Local header
 */
#include "sclsvrFILTER.h"
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
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrVISIBILITY_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrVISIBILITY_FILTER();

    // Class destructor
    virtual ~sclsvrVISIBILITY_FILTER();

    virtual mcsCOMPL_STAT SetVisibilityValue(mcsFLOAT visMax);

    virtual mcsCOMPL_STAT GetVisibilityValue(mcsFLOAT *visMax);

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);    

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

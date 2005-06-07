#ifndef sclsvrVARIABILITY_FILTER_H
#define sclsvrVARIABILITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVARIABILITY_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
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
 * Declaration of sclsvrVARIABILITY_FILTER class.
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

#define sclsvrVARIABILITY_FILTER_NAME "Filter by Variability"

/*
 * Class declaration
 */

/**
 * Variability filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrVARIABILITY_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrVARIABILITY_FILTER();

    // Class destructor
    virtual ~sclsvrVARIABILITY_FILTER();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrVARIABILITY_FILTER(const sclsvrVARIABILITY_FILTER&);
    sclsvrVARIABILITY_FILTER& operator=(const sclsvrVARIABILITY_FILTER&);
};

#endif /*!sclsvrVARIABILITY_FILTER_H*/

/*___oOo___*/

#ifndef sclsvrMULTIPLICITY_FILTER_H
#define sclsvrMULTIPLICITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrMULTIPLICITY_FILTER.h,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrMULTIPLICITY_FILTER class.
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

#define sclsvrMULTIPLICITY_FILTER_NAME "Filter by Multiplicity"

/*
 * Class declaration
 */

/**
 * Multiplicity filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * \sa sclsvrFILTER.cpp
 * \sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrMULTIPLICITY_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrMULTIPLICITY_FILTER();

    // Class destructor
    virtual ~sclsvrMULTIPLICITY_FILTER();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrMULTIPLICITY_FILTER(const sclsvrMULTIPLICITY_FILTER&);
    sclsvrMULTIPLICITY_FILTER& operator=(const sclsvrMULTIPLICITY_FILTER&);
};

#endif /*!sclsvrMULTIPLICITY_FILTER_H*/

/*___oOo___*/

#ifndef sclsvrVISIBILITY_FILTER_H
#define sclsvrVISIBILITY_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

#ifndef vobsVARIABILITY_FILTER_H
#define vobsVARIABILITY_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    vobsVARIABILITY_FILTER(const char* filterId);

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

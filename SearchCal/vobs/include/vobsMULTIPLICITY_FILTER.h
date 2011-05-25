#ifndef vobsMULTIPLICITY_FILTER_H
#define vobsMULTIPLICITY_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    vobsMULTIPLICITY_FILTER(const char* filterId);

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

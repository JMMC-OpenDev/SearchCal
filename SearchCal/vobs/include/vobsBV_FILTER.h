#ifndef vobsBV_FILTER_H
#define vobsBV_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsBV_FILTER class.
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
 * Magnitude B-V filter class.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsBV_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsBV_FILTER(const char* filterId);

    // Class destructor
    virtual ~vobsBV_FILTER();

    virtual mcsCOMPL_STAT SetMagnitudeValue(mcsDOUBLE magValue);

    virtual mcsCOMPL_STAT GetMagnitudeValue(mcsDOUBLE *magValue);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsBV_FILTER(const vobsBV_FILTER&);
    vobsBV_FILTER& operator=(const vobsBV_FILTER&);

    mcsDOUBLE _magValue;
};

#endif /*!vobsBV_FILTER_H*/

/*___oOo___*/

#ifndef vobsORIGIN_FILTER_H
#define vobsORIGIN_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsORIGIN_FILTER class.
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
 * Origin filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsORIGIN_FILTER : public vobsFILTER
{
public:
    // Class constructor
    vobsORIGIN_FILTER(const char* filterId);

    // Class destructor
    ~vobsORIGIN_FILTER();

    mcsCOMPL_STAT SetCriteria(const char* propId, vobsORIGIN_INDEX originIndex);

    mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsORIGIN_FILTER(const vobsORIGIN_FILTER&);
    vobsORIGIN_FILTER& operator=(const vobsORIGIN_FILTER&);

    mcsSTRING32 _propId;
    vobsORIGIN_INDEX _originIndex;
};

#endif /*!vobsORIGIN_FILTER_H*/

/*___oOo___*/

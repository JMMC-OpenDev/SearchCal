#ifndef vobsDISTANCE_FILTER_H
#define vobsDISTANCE_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsDISTANCE_FILTER class.
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
 * Distance filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsDISTANCE_FILTER : public vobsFILTER
{
public:
    // Class constructor
    vobsDISTANCE_FILTER(const char* filterId);

    // Class destructor
    virtual ~vobsDISTANCE_FILTER();

    virtual mcsCOMPL_STAT SetDistanceValue(const mcsSTRING32 raRef,
                                           const mcsSTRING32 decRef,
                                           const mcsDOUBLE separation);

    virtual mcsCOMPL_STAT GetDistanceValue(mcsSTRING32* raRef,
                                           mcsSTRING32* decRef,
                                           mcsDOUBLE* separation);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST* list);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsDISTANCE_FILTER(const vobsDISTANCE_FILTER&);
    vobsDISTANCE_FILTER& operator=(const vobsDISTANCE_FILTER&);

    mcsSTRING32 _raRef;
    mcsSTRING32 _decRef;
    mcsDOUBLE _separation;
};

#endif /*!vobsDISTANCE_FILTER_H*/

/*___oOo___*/

#ifndef vobsMAGNITUDE_FILTER_H
#define vobsMAGNITUDE_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsMAGNITUDE_FILTER class.
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
 * Magnitude filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsMAGNITUDE_FILTER : public vobsFILTER
{
public:
    // Class constructor
    vobsMAGNITUDE_FILTER(const char* filterId);

    // Class destructor
    virtual ~vobsMAGNITUDE_FILTER();

    virtual mcsCOMPL_STAT SetMagnitudeValue(const char* band,
                                            mcsDOUBLE magValue,
                                            mcsDOUBLE magRange);

    virtual mcsCOMPL_STAT GetMagnitudeValue(char* band,
                                            mcsDOUBLE *magValue,
                                            mcsDOUBLE *magRange);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsMAGNITUDE_FILTER(const vobsMAGNITUDE_FILTER&);
    vobsMAGNITUDE_FILTER& operator=(const vobsMAGNITUDE_FILTER&);

    mcsSTRING32 _band;
    mcsDOUBLE _magValue;
    mcsDOUBLE _magRange;
};

#endif /*!vobsMAGNITUDE_FILTER_H*/

/*___oOo___*/

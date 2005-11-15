#ifndef vobsBV_FILTER_H
#define vobsBV_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsBV_FILTER.h,v 1.1 2005-11-15 14:54:33 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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


#define vobsBV_MAGNITUDE_FILTER_NAME "Filter by Magnitude B-V"

/*
 * Class declaration
 */

/**
 * Magnitude B-V filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsBV_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsBV_FILTER();

    // Class destructor
    virtual ~vobsBV_FILTER();

    virtual mcsCOMPL_STAT SetMagnitudeValue(mcsFLOAT magValue);

    virtual mcsCOMPL_STAT GetMagnitudeValue(mcsFLOAT *magValue);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsBV_FILTER(const vobsBV_FILTER&);
    vobsBV_FILTER& operator=(const vobsBV_FILTER&);

    mcsFLOAT _magValue;
};

#endif /*!vobsBV_FILTER_H*/

/*___oOo___*/

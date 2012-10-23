#ifndef vobsCATALOG_USNO_H
#define vobsCATALOG_USNO_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_USNO class.
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
#include "vobsREMOTE_CATALOG.h"

/*
 * Class declaration
 */

/**
 * vobsCATALOG_USNO is a class which caracterise the USNO-B1.0 catalog
 *
 * vobsCATALOG_USNO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 */
class vobsCATALOG_USNO : public vobsREMOTE_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_USNO();

    // Class destructor
    virtual ~vobsCATALOG_USNO();

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_USNO(const vobsCATALOG_USNO&);
    vobsCATALOG_USNO& operator=(const vobsCATALOG_USNO&);

    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_USNO_H*/

/*___oOo___*/

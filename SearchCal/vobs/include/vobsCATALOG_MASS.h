#ifndef vobsCATALOG_MASS_H
#define vobsCATALOG_MASS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_MASS class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header
 */
#include "vobsREMOTE_CATALOG.h"


/*
 * Class declaration
 */

/**
 * vobsCATALOG_MASS is a class which caracterise the 2MASS ALL-Sky Catalog of
 * Point Sources.
 * 
 * vobsCATALOG_MASS methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_MASS : public vobsREMOTE_CATALOG
{
public:
    // Constructor
    vobsCATALOG_MASS();

    // Destructor
    virtual ~vobsCATALOG_MASS();

    // Method to process the output star list from the catalog
    virtual mcsCOMPL_STAT ProcessList(vobsSTAR_LIST &list);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_MASS(const vobsCATALOG_MASS&);
    vobsCATALOG_MASS& operator=(const vobsCATALOG_MASS&);

    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_MASS_H*/


/*___oOo___*/

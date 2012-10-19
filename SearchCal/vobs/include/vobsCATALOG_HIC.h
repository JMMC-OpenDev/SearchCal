#ifndef vobsCATALOG_HIC_H
#define vobsCATALOG_HIC_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_HIC class declaration.
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
 * vobsCATALOG_HIC is a class which caracterise the Hipparcos Input Catalog.
 * 
 * vobsCATALOG_HIC methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_HIC : public vobsREMOTE_CATALOG
{
public:
    // Constructor
    vobsCATALOG_HIC();

    // Destructor
    virtual ~vobsCATALOG_HIC();

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_HIC(const vobsCATALOG_HIC&);
    vobsCATALOG_HIC& operator=(const vobsCATALOG_HIC&);

    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};



#endif /*!vobsCATALOG_HIC_H*/


/*___oOo___*/

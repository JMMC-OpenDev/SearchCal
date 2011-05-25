#ifndef vobsCATALOG_CHARM2_H
#define vobsCATALOG_CHARM2_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_CHARM2 class.
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
 * vobsCATALOG_CHARM2 is a class which caracterise the Catalog of High Angular
 * Position M.
 * 
 * vobsCATALOG_CHARM2 methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsCATALOG_CHARM2 : public vobsREMOTE_CATALOG
{

public:
    // Class constructor
    vobsCATALOG_CHARM2();

    // Class destructor
    virtual ~vobsCATALOG_CHARM2();

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_CHARM2(const vobsCATALOG_CHARM2&);
    vobsCATALOG_CHARM2& operator=(const vobsCATALOG_CHARM2&);

    // Method to build specific asking part
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_CHARM2_H*/

/*___oOo___*/

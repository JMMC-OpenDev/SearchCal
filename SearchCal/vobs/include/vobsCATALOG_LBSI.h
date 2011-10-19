#ifndef vobsCATALOG_LBSI_H
#define vobsCATALOG_LBSI_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_LBSI class declaration.
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
 * vobsCATALOG_LBSI is a class which caracterise the Catalog of calibrators
 * star for LBSI.
 * 
 * vobsCATALOG_LBSI methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_LBSI : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_LBSI();

   // Destructor
   virtual ~vobsCATALOG_LBSI();
   
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_LBSI(const vobsCATALOG_LBSI&);
    vobsCATALOG_LBSI& operator=(const vobsCATALOG_LBSI&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);
};


#endif /*!vobsCATALOG_LBSI_H*/


/*___oOo___*/

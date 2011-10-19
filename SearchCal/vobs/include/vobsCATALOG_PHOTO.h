#ifndef vobsCATALOG_PHOTO_H
#define vobsCATALOG_PHOTO_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_PHOTO class declaration.
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
 * vobsCATALOG_PHOTO is a class which caracterise the Photoelectric Catalog.
 * 
 * vobsCATALOG_PHOTO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_PHOTO : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_PHOTO();

   // Destructor
   virtual ~vobsCATALOG_PHOTO();
   
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_PHOTO(const vobsCATALOG_PHOTO&);
    vobsCATALOG_PHOTO& operator=(const vobsCATALOG_PHOTO&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);
};

#endif /*!vobsCATALOG_PHOTO_H*/


/*___oOo___*/

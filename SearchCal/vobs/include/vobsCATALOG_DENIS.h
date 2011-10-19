#ifndef vobsCATALOG_DENIS_H
#define vobsCATALOG_DENIS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_DENIS class declaration.
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
 * vobsCATALOG_DENIS is a class which caracterise the DENIS database.
 * 
 * vobsCATALOG_DENIS methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsCATALOG_DENIS : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_DENIS();

   // Destructor
   virtual ~vobsCATALOG_DENIS();
   
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_DENIS(const vobsCATALOG_DENIS&);
    vobsCATALOG_DENIS& operator=(const vobsCATALOG_DENIS&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_DENIS_H*/


/*___oOo___*/

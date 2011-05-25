#ifndef vobsCATALOG_BSC_H
#define vobsCATALOG_BSC_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_BSC class.
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
 * vobsCATALOG_BSC is a class which caracterise the Bright Star Catalog.
 * 
 * vobsCATALOG_BSC methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsCATALOG_BSC : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_BSC();

   // Destructor
   virtual ~vobsCATALOG_BSC();
   
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

private:
};

#endif /*!vobsCATALOG_BSC_H*/


/*___oOo___*/

#ifndef vobsCATALOG_CIO_H
#define vobsCATALOG_CIO_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_CIO class declaration.
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
 * vobsCATALOG_CIO is a class which caracterise the Catalog of High Angular
 * Position M.
 * 
 * vobsCATALOG_CIO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_CIO : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_CIO();

   // Destructor
   virtual ~vobsCATALOG_CIO();
   
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQueryConstantPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);

private:
};


#endif /*!vobsCATALOG_CIO_H*/


/*___oOo___*/

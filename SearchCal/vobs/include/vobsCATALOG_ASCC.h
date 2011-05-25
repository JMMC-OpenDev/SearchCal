#ifndef vobsCATALOG__ASCC_H
#define vobsCATALOG__ASCC_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_ASCC class.
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
 * vobsCATALOG_ASCC is a class which caracterise the All-Sky Compiled Catalog
 * I/280.
 * 
 * vobsCATALOG_ASCC methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_ASCC : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_ASCC();

   // Destructor
   virtual ~vobsCATALOG_ASCC(); 

protected:
    // Method to build parts of the request
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);

private:
};

#endif /*!vobsCATALOG__ASCC_H*/


/*___oOo___*/

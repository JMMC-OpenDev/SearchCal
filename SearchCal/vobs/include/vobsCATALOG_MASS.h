#ifndef vobsCATALOG_MASS_H
#define vobsCATALOG_MASS_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_MASS.h,v 1.4 2004-11-30 10:32:31 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
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
#include "vobsCATALOG.h"


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

class vobsCATALOG_MASS : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_MASS();

   // Destructor
   virtual ~vobsCATALOG_MASS();
   
protected:
    // Method to build specific asking part
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

private:
};

#endif /*!vobsCATALOG_MASS_H*/


/*___oOo___*/

#ifndef vobsCATALOG_HIC_H
#define vobsCATALOG_HIC_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_HIC.h,v 1.1 2004-07-28 14:16:21 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
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
#include "vobsCATALOG.h"


/*
 * Class declaration
 */

/**
 * vobsCATALOG_HIC is a class which caracterise the Hipparcos Input Catalog.
 * 
 * methods allow to
 * \li Prepare a request
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
 *
 * 
 */

class vobsCATALOG_HIC:vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_HIC();
   
   // Destructor
   virtual ~vobsCATALOG_HIC();
protected:
   // Method to build specific asking part
   virtual mcsCOMPL_STAT WriteAskingSpecificParameters(void);
    
private:
};



#endif /*!vobsCATALOG_HIC_H*/


/*___oOo___*/

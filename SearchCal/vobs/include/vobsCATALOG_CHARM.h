#ifndef vobsCATALOG_CHARM_H
#define vobsCATALOG_CHARM_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CHARM.h,v 1.3 2004-11-23 12:47:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_CHARM class declaration.
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
 * vobsCATALOG_CHARM is a class which caracterise the Catalog of High Angular
 * Position M.
 * 
 * vobsSTAR methods allow to
 * \li Prepare a request
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
 *
 * 
 */
class vobsCATALOG_CHARM : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_CHARM();

   // Destructor
   virtual ~vobsCATALOG_CHARM();
   
protected:
    // Method to build specific asking part
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

private:
};

#endif /*!vobsCATALOG_CHARM_H*/


/*___oOo___*/

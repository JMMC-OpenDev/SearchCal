#ifndef vobsCATALOG_BSC_H
#define vobsCATALOG_BSC_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_BSC.h,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_BSC class declaration.
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
 * vobsCATALOG_BSC is a class which caracterise the Bright Star Catalog.
 * 
 * vobsSTAR methods allow to
 * \li Prepare a request
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
 *
 * 
 */
class vobsCATALOG_BSC : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_BSC();

   // Destructor
   virtual ~vobsCATALOG_BSC();
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteAskingSpecificParameters(void);
private:
};

#endif /*!vobsCATALOG_BSC_H*/


/*___oOo___*/

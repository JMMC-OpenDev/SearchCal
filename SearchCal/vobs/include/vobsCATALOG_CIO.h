#ifndef vobsCATALOG_CIO_H
#define vobsCATALOG_CIO_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CIO.h,v 1.1 2004-07-28 14:16:21 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
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
#include "vobsCATALOG.h"

/*
 * Class declaration
 */

/**
 * vobsCATALOG_CIO is a class which caracterise the Catalog of High Angular
 * Position M.
 * 
 * vobsSTAR methods allow to
 * \li Prepare a request
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
 *
 * 
 */

class vobsCATALOG_CIO:vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_CIO();

   // Destructor
   virtual ~vobsCATALOG_CIO();
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteAskingConstant(void);
    virtual mcsCOMPL_STAT WriteAskingSpecificParameters(void);
    virtual mcsCOMPL_STAT WriteAskingSpecificParameters(vobsREQUEST request);
private:
};


#endif /*!vobsCATALOG_CIO_H*/


/*___oOo___*/

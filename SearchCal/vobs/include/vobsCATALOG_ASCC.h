#ifndef vobsCATALOG__ASCC_H
#define vobsCATALOG__ASCC_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_ASCC.h,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_ASCC class declaration.
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
 * vobsCATALOG_ASCC is a class which caracterise the All-Sky Compiled Catalog
 * I/280.
 * 
 * vobsSTAR methods allow to
 * \li Prepare a request
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
 *
 * 
 */

class vobsCATALOG_ASCC : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_ASCC();

   // Destructor
   virtual ~vobsCATALOG_ASCC(); 

protected:
    // Method to build parts of the request
   virtual mcsCOMPL_STAT WriteAskingSpecificParameters(void);
   virtual mcsCOMPL_STAT WriteAskingSpecificParameters(vobsREQUEST request);

private:
};

#endif /*!vobsCATALOG__ASCC_H*/


/*___oOo___*/

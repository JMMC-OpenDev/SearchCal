#ifndef vobsCATALOG_DENIS_H
#define vobsCATALOG_DENIS_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_DENIS.h,v 1.4 2004-11-30 10:32:31 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
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
#include "vobsCATALOG.h"

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

class vobsCATALOG_DENIS : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_DENIS();

   // Destructor
   virtual ~vobsCATALOG_DENIS();
   
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

private:
};

#endif /*!vobsCATALOG_DENIS_H*/


/*___oOo___*/

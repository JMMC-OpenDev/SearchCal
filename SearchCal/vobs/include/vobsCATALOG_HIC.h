#ifndef vobsCATALOG_HIC_H
#define vobsCATALOG_HIC_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_HIC.h,v 1.5 2005-01-26 08:10:32 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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
 * vobsCATALOG_HIC methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_HIC : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_HIC();
   
   // Destructor
   virtual ~vobsCATALOG_HIC();
   
protected:
   // Method to build specific asking part
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    
private:
};



#endif /*!vobsCATALOG_HIC_H*/


/*___oOo___*/

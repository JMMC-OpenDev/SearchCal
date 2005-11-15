#ifndef vobsCATALOG_LBSI_H
#define vobsCATALOG_LBSI_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_LBSI.h,v 1.7 2005-11-15 14:57:56 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
* Revision 1.5  2005/01/26 08:10:32  scetre
* change history
*
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_LBSI class declaration.
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
 * vobsCATALOG_LBSI is a class which caracterise the Catalog of calibrators
 * star for LBSI.
 * 
 * vobsCATALOG_LBSI methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_LBSI : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_LBSI();

   // Destructor
   virtual ~vobsCATALOG_LBSI();

protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);
private:
};


#endif /*!vobsCATALOG_LBSI_H*/


/*___oOo___*/

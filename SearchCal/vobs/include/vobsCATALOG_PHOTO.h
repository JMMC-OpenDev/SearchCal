#ifndef vobsCATALOG_PHOTO_H
#define vobsCATALOG_PHOTO_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_PHOTO.h,v 1.7 2005-02-11 14:22:10 gluck Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/02/07 19:32:52  gzins
* Updated vobsREQUEST API
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
 * vobsCATALOG_PHOTO class declaration.
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
 * vobsCATALOG_PHOTO is a class which caracterise the Photoelectric Catalog.
 * 
 * vobsCATALOG_PHOTO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_PHOTO : public vobsREMOTE_CATALOG
{
public:
    // Constructor
   vobsCATALOG_PHOTO();

   // Destructor
   virtual ~vobsCATALOG_PHOTO();
   
protected:
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);

private:
};

#endif /*!vobsCATALOG_PHOTO_H*/


/*___oOo___*/

#ifndef vobsCATALOG_CIO_H
#define vobsCATALOG_CIO_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CIO.h,v 1.7 2005-02-07 19:32:52 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/01/26 08:10:32  scetre
* change history
*
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
 * vobsCATALOG_CIO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_CIO : public vobsCATALOG
{
public:
    // Constructor
   vobsCATALOG_CIO();

   // Destructor
   virtual ~vobsCATALOG_CIO();
   
protected:
    // Method to prepare the request as a string
    virtual mcsCOMPL_STAT PrepareQuery(vobsREQUEST &request);
       
    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQueryConstantPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);

private:
};


#endif /*!vobsCATALOG_CIO_H*/


/*___oOo___*/

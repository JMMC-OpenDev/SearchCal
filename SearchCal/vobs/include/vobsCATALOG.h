#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.8 2004-11-30 10:32:31 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS header
 */
#include "misc.h"

/*
 * Local header
 */
#include "vobsREQUEST.h"
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */

/**
 * vobsCATALOG is a class which caracterise a catalog.
 * 
 * vobsCATALOG methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsCATALOG
{
public:
   // Constructor
   vobsCATALOG();

   // Destructor
   virtual ~vobsCATALOG();

    // Method to set catalog name
   virtual mcsCOMPL_STAT SetName(char *name);
   
   // Method to get the catalog name
   virtual mcsCOMPL_STAT GetName(char *name);  

   // Method to get a  star list from the catalog
   virtual mcsCOMPL_STAT Search(vobsREQUEST &request,
                                vobsSTAR_LIST &list);
   
protected:
    // Method to prepare the request in a string format
   virtual mcsCOMPL_STAT PrepareQuery(vobsREQUEST &request);
   virtual mcsCOMPL_STAT PrepareQuery(vobsREQUEST request,
                                 vobsSTAR_LIST &tmpList);

   // Method to build all parts of the asking
   virtual mcsCOMPL_STAT WriteQueryURIPart(void);
   virtual mcsCOMPL_STAT WriteQueryConstantPart(void);
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST request);
   virtual mcsCOMPL_STAT WriteReferenceStarPosition(vobsREQUEST request);
   virtual mcsCOMPL_STAT WriteQueryStarListPart(vobsSTAR_LIST &list);
  
   // Method to get a star list in a string format from a normal star list
   // format
   virtual mcsCOMPL_STAT StarList2Sring(miscDYN_BUF &strList,
                                        vobsSTAR_LIST &list);

   // Name of the catalog
    mcsSTRING32 _name;
    
    // Request to write and to send to the CDS
    miscDYN_BUF _query;

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsCATALOG& operator=(const vobsCATALOG&);
    vobsCATALOG (const vobsCATALOG&);
};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.5 2004-09-30 07:40:09 scetre Exp $"
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
 * \li Send this to the CDS
 * \li Build a star list from the return of the CDS
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

   // Method to search in the catalog and to have the star list
   virtual mcsCOMPL_STAT Search(vobsREQUEST &request,
                                vobsSTAR_LIST &list);
   
   // Method to get the fileName associated to the catalog
   virtual mcsCOMPL_STAT GetFileName(char *fileName);
   
protected:
    // Method to preare the request as a string
   virtual mcsCOMPL_STAT PrepareAsking(vobsREQUEST &request);
   virtual mcsCOMPL_STAT PrepareAsking(vobsREQUEST request,
                                 vobsSTAR_LIST &tmpList);

   // Method to build all parts of the asking
   virtual mcsCOMPL_STAT WriteAskingURI(void);
   virtual mcsCOMPL_STAT WriteAskingConstant(void);
   virtual mcsCOMPL_STAT WriteAskingSpecificParameters(void);
   virtual mcsCOMPL_STAT WriteAskingSpecificParameters(vobsREQUEST request);
   virtual mcsCOMPL_STAT WriteAskingPosition(vobsREQUEST request);
   virtual mcsCOMPL_STAT WriteAskingEnd(vobsSTAR_LIST &list);
  
   // Method to have a star list as a string from a normal star list
   virtual mcsCOMPL_STAT StarList2Sring(miscDYN_BUF &strList,
                                        vobsSTAR_LIST &list);

   // Name of the catalog
    mcsSTRING32 _name;
    
    // Request to write and to send to the CDS
    miscDYN_BUF _asking;

    // File name to save the result of the ask of the catalog
    mcsSTRING256 _fileName;
    mcsSTRING32 _nameForFile;

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsCATALOG& operator=(const vobsCATALOG&);
    vobsCATALOG (const vobsCATALOG&);
    
};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

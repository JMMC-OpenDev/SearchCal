#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.11 2005-02-11 14:22:10 gluck Exp $"
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
 * Declaration of vobsCATALOG class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS header
 */


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
 * vobsCATALOG methods allow to find a star list in a catalog
 * 
 */
class vobsCATALOG
{
public:
   // Constructor
   vobsCATALOG(const char *name);

   // Destructor
   virtual ~vobsCATALOG();

   // Method to get the catalog name
   virtual const char *GetName(void);  

   // Method to get a  star list from the catalog
   virtual mcsCOMPL_STAT Search(vobsREQUEST &request,
                                vobsSTAR_LIST &list) = 0;
   
protected:
   // Name of the catalog
   string _name;

private:
   // Declaration of assignment operator as private
   // method, in order to hide them from the users.
   vobsCATALOG& operator=(const vobsCATALOG&);
   vobsCATALOG (const vobsCATALOG&);
};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

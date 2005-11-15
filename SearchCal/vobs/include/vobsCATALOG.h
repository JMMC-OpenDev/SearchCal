#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.12 2005-11-15 14:57:56 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.11  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
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

#define vobsCATALOG_ASCC_ID         "I/280"
#define vobsCATALOG_BSC_ID          "V/50/catalog"
#define vobsCATALOG_CHARM2_ID       "J/A+A/386/492/charm"
#define vobsCATALOG_CHARM_ID        "J/A+A/431/773/charm2"
#define vobsCATALOG_CIO_ID          "II/225/catalog"
#define vobsCATALOG_DENIS_ID        "B/denis"
#define vobsCATALOG_DENIS_JK_ID     "J/A+A/413/1037"
#define vobsCATALOG_HIC_ID          "I/196/main"
#define vobsCATALOG_LBSI_ID         "J/A+A/393/183/catalog"
#define vobsCATALOG_MASS_ID         "II/246/out"
#define vobsCATALOG_MERAND_ID       "J/A+A/433/1155"
#define vobsCATALOG_MIDI_ID         "MIDI"
#define vobsCATALOG_PHOTO_ID        "II/7A/catalog"
#define vobsCATALOG_SBSC_ID         "V/36B/bsc4s"

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

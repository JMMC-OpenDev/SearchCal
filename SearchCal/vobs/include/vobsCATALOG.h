#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.18 2009-02-26 10:39:03 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.17  2009/02/09 16:26:10  lafrasse
* Added Spectral Binary catalog (SB9).
*
* Revision 1.16  2006/01/18 09:06:43  scetre
* Added option in the query
*
* Revision 1.15  2005/11/24 14:54:25  scetre
* UNSO catalog added
*
* Revision 1.14  2005/11/21 13:49:40  scetre
* Added UNSO id
*
* Revision 1.13  2005/11/17 08:05:52  scetre
* Added possibility to add no catalog in an entry
*
* Revision 1.12  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
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

#define vobsNO_CATALOG_ID           "NO CATALOG"
#define vobsCATALOG_ASCC_ID         "I/280"
#define vobsCATALOG_UNSO_ID         "I/284"
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
#define vobsCATALOG_UNSO_ID         "I/284"
#define vobsCATALOG_SB9_ID          "B/sb9/main"

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

   virtual mcsCOMPL_STAT SetOption(string option) = 0;
   
protected:
   // Name of the catalog
   string _name;
   string _option;

private:
   // Declaration of assignment operator as private
   // method, in order to hide them from the users.
   vobsCATALOG& operator=(const vobsCATALOG&);
   vobsCATALOG (const vobsCATALOG&);
};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

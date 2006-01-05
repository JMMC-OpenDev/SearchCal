#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.h,v 1.16 2006-01-05 09:07:39 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.15  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.14  2005/06/16 09:32:59  scetre
* Removed unused vobsCATALOG_TOTO
*
* Revision 1.13  2005/06/13 10:19:47  scetre
* Added Charm2, Merand and new Denis catalogs
*
* Revision 1.12  2005/02/07 19:32:52  gzins
* Updated vobsREQUEST API
*
* Revision 1.11  2005/02/04 13:32:04  gzins
* Added MIDI catalog
*
* Revision 1.10  2005/01/26 08:10:32  scetre
* change history
*
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * @file
 * vobsVIRTUAL_OBSERVATORY class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header
 */
#include "vobsREQUEST.h"
#include "vobsSTAR_LIST.h"
#include "vobsSCENARIO.h"
#include "vobsCATALOG.h"
#include "vobsCATALOG_ASCC.h"
#include "vobsCATALOG_BSC.h"
#include "vobsCATALOG_CHARM.h"
#include "vobsCATALOG_CHARM2.h"
#include "vobsCATALOG_CIO.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG_DENIS_JK.h"
#include "vobsCATALOG_HIC.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_MERAND.h"
#include "vobsCATALOG_MASS.h"
#include "vobsCATALOG_PHOTO.h"
#include "vobsCATALOG_SBSC.h"
#include "vobsCATALOG_MIDI.h"

/*
 * Class declaration
 */

/**
 * vobsVIRTUAL_OBSERVATORY is the netry point of the 'vodbs' module.
 *
 * It allows to:
 * @li Load the scenario of research (i.e. the order of star catalogs queries);
 * @li launch the search in all the catalogs.
 */

class vobsVIRTUAL_OBSERVATORY
{
public:
     // Constructor
    vobsVIRTUAL_OBSERVATORY();

    // Destructor
    virtual ~vobsVIRTUAL_OBSERVATORY();
 
    // Method to star the research in the different catalog
    virtual mcsCOMPL_STAT Search(vobsSCENARIO   *scenario,
                                 vobsREQUEST    &request,
                                 vobsSTAR_LIST  &StarList);
    
protected:
        
private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsVIRTUAL_OBSERVATORY& operator=(const vobsVIRTUAL_OBSERVATORY&);
    vobsVIRTUAL_OBSERVATORY (const vobsVIRTUAL_OBSERVATORY&);

    // Catalog list
    vobsCATALOG_LIST  _catalogList;

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/

/*___oOo___*/

#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

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

#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.h,v 1.5 2004-09-13 07:00:15 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
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
#include "vobsCATALOG_CIO.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG_HIC.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_MASS.h"
#include "vobsCATALOG_PHOTO.h"
#include "vobsCATALOG_SBSC.h"

/*
 * Class declaration
 */

/**
 * vobsVIRTUAL_OBSERVATORY is a class which represent the virtual observatory.
 * 
 * vobsVIRTUAL_OBSERVATORY methods allow to
 * \li Load the scenario of research : the order of interrogation of the
 * catalogs
 * \li search in all the catalogs
 * 
 */

class vobsVIRTUAL_OBSERVATORY
{
public:
     // Constructor
    vobsVIRTUAL_OBSERVATORY();

    // Destructor
    virtual ~vobsVIRTUAL_OBSERVATORY();
 
    // Method to star the research in the different catalog
    virtual mcsCOMPL_STAT Research(vobsREQUEST &request,
                                   vobsSTAR_LIST &StarList);
    
    /*vobsCATALOG_CIO cio;
    vobsCATALOG_PHOTO photo;
    vobsCATALOG_ASCC ascc;
    vobsCATALOG_CIO cio2;
    vobsCATALOG_HIC hic;
    vobsCATALOG_MASS mass;
    vobsCATALOG_LBSI lbsi;
    vobsCATALOG_CHARM charm;
    vobsCATALOG_PHOTO photo2;
    vobsCATALOG_BSC bsc;
    vobsCATALOG_SBSC sbsc;
    vobsCATALOG_DENIS denis;*/
protected:
    // Method to load scenario according to the request
    virtual mcsCOMPL_STAT LoadScenario(vobsREQUEST      &request,
                                       vobsSTAR_LIST    &StarList,
                                       vobsSCENARIO     &scenario);
    
private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsVIRTUAL_OBSERVATORY& operator=(const vobsVIRTUAL_OBSERVATORY&);
    vobsVIRTUAL_OBSERVATORY (const vobsVIRTUAL_OBSERVATORY&);
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/


/*___oOo___*/

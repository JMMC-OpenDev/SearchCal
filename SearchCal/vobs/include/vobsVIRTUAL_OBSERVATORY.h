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
#include "vobsSCENARIO_RUNTIME.h"

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
    ~vobsVIRTUAL_OBSERVATORY();

    // Method to initialize the scenario with the given request and optional input list
    mcsCOMPL_STAT Init(vobsSCENARIO* scenario,
                       vobsREQUEST* request,
                       vobsSTAR_LIST* starList = NULL);

    // Method to star the research in the different catalog
    mcsCOMPL_STAT Search(vobsSCENARIO *scenario,
                         vobsSTAR_LIST &starList);

protected:

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsVIRTUAL_OBSERVATORY& operator=(const vobsVIRTUAL_OBSERVATORY&);
    vobsVIRTUAL_OBSERVATORY(const vobsVIRTUAL_OBSERVATORY&);

    // Catalog list
    vobsCATALOG_LIST _catalogList;
    
    // Scenario runtime context
    vobsSCENARIO_RUNTIME _ctx;
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/

/*___oOo___*/

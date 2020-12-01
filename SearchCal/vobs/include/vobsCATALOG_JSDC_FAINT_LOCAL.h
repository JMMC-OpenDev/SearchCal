#ifndef vobsCATALOG_JSDC_FAINT_LOCAL_H
#define vobsCATALOG_JSDC_FAINT_LOCAL_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_JSDC_FAINT_LOCAL class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobsLOCAL_CATALOG.h"


/*
 * Class declaration
 */

/**
 * Class used to query JSDC_FAINT_LOCAL catalog.
 *
 * Through JSDC_FAINT_LOCAL catalog interrogation, this class allows to get all the primary stars for JSDC
 *
 * \n
 * \sa - vobsCATALOG class
 */
class vobsCATALOG_JSDC_FAINT_LOCAL : public vobsLOCAL_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_JSDC_FAINT_LOCAL();

    // Class destructor
    virtual ~vobsCATALOG_JSDC_FAINT_LOCAL();

    // Search for star list in ASCC catalog
    mcsCOMPL_STAT Search(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST &request, vobsSTAR_LIST &list, const char* option,
                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_JSDC_FAINT_LOCAL(const vobsCATALOG_JSDC_FAINT_LOCAL&);
    vobsCATALOG_JSDC_FAINT_LOCAL& operator=(const vobsCATALOG_JSDC_FAINT_LOCAL&) ;

    // Load JSDC_FAINT_LOCAL catalog
    virtual mcsCOMPL_STAT Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap);
} ;

#endif /*!vobsCATALOG_JSDC_FAINT_LOCAL_H*/

/*___oOo___*/


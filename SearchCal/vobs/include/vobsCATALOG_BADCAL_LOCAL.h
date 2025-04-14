#ifndef vobsCATALOG_BADCAL_LOCAL_H
#define vobsCATALOG_BADCAL_LOCAL_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_BADCAL_LOCAL class.
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
class vobsCATALOG_BADCAL_LOCAL : public vobsLOCAL_CATALOG
{
public:

    static vobsCATALOG_BADCAL_LOCAL& getInstance()
    {
        static vobsCATALOG_BADCAL_LOCAL INSTANCE;
        return INSTANCE;
    }

    /** preload the catalog at startup */
    static bool loadData();
    /** free the catalog at shutdown */
    static void freeData();
    
    // Class destructor
    virtual ~vobsCATALOG_BADCAL_LOCAL();

    // Search for star list in ASCC catalog
    mcsCOMPL_STAT Search(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST &request, vobsSTAR_LIST &list, const char* option,
                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE);

    mcsCOMPL_STAT GetAll(vobsSTAR_LIST &list);
    
private:
    // Private Class constructor
    vobsCATALOG_BADCAL_LOCAL();

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_BADCAL_LOCAL(const vobsCATALOG_BADCAL_LOCAL&);
    vobsCATALOG_BADCAL_LOCAL& operator=(const vobsCATALOG_BADCAL_LOCAL&) ;

    // Loadlocal catalog
    virtual mcsCOMPL_STAT Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap);
    
    mcsSTRING512 _catalogFileName;
    
    time_t _loaded_time;
} ;

#endif /*!vobsCATALOG_BADCAL_LOCAL_H*/

/*___oOo___*/


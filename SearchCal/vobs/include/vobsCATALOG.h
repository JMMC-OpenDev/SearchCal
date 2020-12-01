#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/* 
 * System Headers 
 */
#include <map>

/*
 * Local header
 */
#include "vobsREQUEST.h"
#include "vobsCATALOG_META.h"
#include "vobsSTAR_LIST.h"


/* CDS common column names */
/* RA/DEC coordinates precessed by CDS in J200 epoch 2000.0 */
#define vobsCATALOG___RAJ2000       "_RAJ2000"
#define vobsCATALOG___DEJ2000       "_DEJ2000"
#define vobsCATALOG___TARGET_ID     "_1"


/** Forward reference */
class vobsSCENARIO_RUNTIME;

/*
 * Type declaration
 */

/** CatalogMeta pointer map keyed by catalog id */
typedef std::map<vobsORIGIN_INDEX, vobsCATALOG_META*> vobsCATALOG_META_PTR_MAP;

/** Catalog name / CatalogMeta pointer pair */
typedef std::pair<vobsORIGIN_INDEX, vobsCATALOG_META*> vobsCATALOG_META_PAIR;

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
    vobsCATALOG(vobsORIGIN_INDEX catalogId);

    // Destructor
    virtual ~vobsCATALOG();

    /**
     * Get the catalog meta data
     *
     * @return catalog meta data.
     */
    inline const vobsCATALOG_META* GetCatalogMeta() const __attribute__((always_inline))
    {
        return _meta;
    }

    /**
     * Get the catalog ID as string literal
     *
     * @return catalog ID or NULL if not set.
     */
    inline const char* GetId() const __attribute__((always_inline))
    {
        return _meta->GetId();
    }

    /** 
     * Return the catalog id as origin index
     */
    inline vobsORIGIN_INDEX GetCatalogId() const __attribute__((always_inline))
    {
        return _meta->GetCatalogId();
    }

    /**
     * Get the catalog name as string literal
     *
     * @return catalog name or NULL if not set.
     */
    inline const char* GetName() const __attribute__((always_inline))
    {
        return _meta->GetName();
    }

    // Method to get a  star list from the catalog
    virtual mcsCOMPL_STAT Search(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST &request, vobsSTAR_LIST &list, const char* option,
                                 vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE) = 0;

    // Method to process optionally the output star list from the catalog
     virtual mcsCOMPL_STAT PostProcessList(vobsSTAR_LIST &list) = 0;
    
    /**
     * Find the catalog meta data for the given catalog identifier
     * @param catalogId catalog identifier
     * @return catalog meta data or NULL if not found in the catalog meta map
     */
    inline static vobsCATALOG_META* GetCatalogMeta(vobsORIGIN_INDEX catalogId) __attribute__((always_inline))
    {
        // Look for catalog meta:
        vobsCATALOG_META_PTR_MAP::iterator idxIter = vobsCATALOG::vobsCATALOG_catalogMetaMap.find(catalogId);

        // If no catalog meta with the given Id was found
        if (idxIter == vobsCATALOG::vobsCATALOG_catalogMetaMap.end())
        {
            return NULL;
        }

        return idxIter->second;
    }

    inline static void AddCatalogMeta(vobsCATALOG_META* catalogMeta) __attribute__((always_inline))
    {
        vobsORIGIN_INDEX catalogId = catalogMeta->GetCatalogId();

        if (IS_NULL(GetCatalogMeta(catalogId)))
        {
            vobsCATALOG::vobsCATALOG_catalogMetaMap.insert(vobsCATALOG_META_PAIR(catalogId, catalogMeta));
        }
    }

    static void FreeCatalogMetaMap(void)
    {
        for (vobsCATALOG_META_PTR_MAP::iterator iter = vobsCATALOG::vobsCATALOG_catalogMetaMap.begin();
             iter != vobsCATALOG::vobsCATALOG_catalogMetaMap.end(); iter++)
        {
            delete(iter->second);
        }
        vobsCATALOG::vobsCATALOG_catalogMetaMap.clear();
        vobsCATALOG::vobsCATALOG_catalogMetaInitialized = false;
    }


private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsCATALOG& operator=(const vobsCATALOG&) ;
    vobsCATALOG(const vobsCATALOG&);

    static bool vobsCATALOG_catalogMetaInitialized;

    // shared catalog meta map:
    static vobsCATALOG_META_PTR_MAP vobsCATALOG_catalogMetaMap;

    // metadata (constant):
    const vobsCATALOG_META* _meta;

    // Method to define all catalog meta data
    static void AddCatalogMetas(void);

    static mcsCOMPL_STAT DumpCatalogMetaAsXML();

    static mcsCOMPL_STAT DumpCatalogMetaAsXML(miscoDYN_BUF &xmlBuf, const char* name);

} ;

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

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

#define vobsNO_CATALOG_ID           "NO CATALOG"
#define vobsCATALOG_AKARI_ID        "II/297/irc"
#define vobsCATALOG_ASCC_ID         "I/280"
#define vobsCATALOG_ASCC_LOCAL_ID   "I/280B"
#define vobsCATALOG_BSC_ID          "V/50/catalog"
#define vobsCATALOG_CIO_ID          "II/225/catalog"
#define vobsCATALOG_DENIS_ID        "B/denis"
#define vobsCATALOG_DENIS_JK_ID     "J/A+A/413/1037/table1"
#define vobsCATALOG_HIC_ID          "I/196/main"
#define vobsCATALOG_HIP2_ID         "I/311/hip2"
#define vobsCATALOG_LBSI_ID         "J/A+A/393/183/catalog"
#define vobsCATALOG_MASS_ID         "II/246/out"
#define vobsCATALOG_MERAND_ID       "J/A+A/433/1155"
#define vobsCATALOG_MIDI_ID         "MIDI"
#define vobsCATALOG_PHOTO_ID        "II/7A/catalog"
#define vobsCATALOG_SBSC_ID         "V/36B/bsc4s"
#define vobsCATALOG_SB9_ID          "B/sb9/main"
#define vobsCATALOG_USNO_ID         "I/284"
#define vobsCATALOG_WDS_ID          "B/wds/wds"

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


/*
 * Type declaration
 */

/*
 * const char* comparator used by map<const char*, ...> defined in vobsSTAR.h
 */
struct constStringComparator;

/** CatalogMeta pointer map keyed by catalog name using char* keys and custom comparator functor */
typedef std::map<const char*, vobsCATALOG_META*, constStringComparator> vobsCATALOG_META_PTR_MAP;

/** Catalog name / CatalogMeta pointer pair */
typedef std::pair<const char*, vobsCATALOG_META*> vobsCATALOG_META_PAIR;

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
    vobsCATALOG(const char* name);

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
     * Get the catalog name as string literal
     *
     * @return catalog name or NULL if not set.
     */
    inline const char* GetName() const __attribute__((always_inline))
    {
        return _meta->GetName();
    }

    /**
     * Get the option as string literal
     *
     * @return option or "" if not set.
     */
    inline const char* GetOption() const __attribute__((always_inline))
    {
        if (_option == NULL)
        {
            return "";
        }
        return _option;
    }

    virtual mcsCOMPL_STAT SetOption(const char* option)
    {
        _option = option;

        return mcsSUCCESS;
    }

    // Method to get a  star list from the catalog
    virtual mcsCOMPL_STAT Search(vobsREQUEST &request, vobsSTAR_LIST &list,
                                 vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE) = 0;

    /**
     * Find the catalog meta data for the given catalog identifier
     * @param name catalog name
     * @return catalog meta data or NULL if not found in the catalog meta map
     */
    inline static vobsCATALOG_META* GetCatalogMeta(const char* name) __attribute__((always_inline))
    {
        // Look for property
        vobsCATALOG_META_PTR_MAP::iterator idxIter = vobsCATALOG::vobsCATALOG_catalogMetaMap.find(name);

        // If no property with the given Id was found
        if (idxIter == vobsCATALOG::vobsCATALOG_catalogMetaMap.end())
        {
            return NULL;
        }

        return idxIter->second;
    }

    inline static void AddCatalogMeta(vobsCATALOG_META* catalogMeta) __attribute__((always_inline))
    {
        const char* name = catalogMeta->GetName();

        if (GetCatalogMeta(name) == NULL)
        {
            vobsCATALOG::vobsCATALOG_catalogMetaMap.insert(vobsCATALOG_META_PAIR(name, catalogMeta));
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
    vobsCATALOG& operator=(const vobsCATALOG&);
    vobsCATALOG(const vobsCATALOG&);

    static bool vobsCATALOG_catalogMetaInitialized;

    // shared catalog meta map:
    static vobsCATALOG_META_PTR_MAP vobsCATALOG_catalogMetaMap;

    // metadata (constant):
    const vobsCATALOG_META* _meta;

    // data:
    // options for the query string:
    const char* _option;

    // Method to define all catalog meta data
    static void AddCatalogMetas(void);

    static mcsCOMPL_STAT DumpCatalogMetaAsXML();

    static mcsCOMPL_STAT DumpCatalogMetaAsXML(miscoDYN_BUF& buffer, const char* name);

};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/

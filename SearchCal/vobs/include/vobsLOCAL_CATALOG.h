#ifndef vobsLOCAL_CATALOG_H
#define vobsLOCAL_CATALOG_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsLOCAL_CATALOG class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobsCATALOG.h"


/*
 * Class declaration
 */

/**
 * Class used to query local catalog.
 *
 * Through local catalog interrogation, this class alows to realise search
 * from user request.
 *
 * \n
 * \sa - vobsCATALOG class
 */
class vobsLOCAL_CATALOG : public vobsCATALOG
{
public:
    // Class constructor
    vobsLOCAL_CATALOG(vobsORIGIN_INDEX catalogId, const char *filename);

    // Class destructor
    virtual ~vobsLOCAL_CATALOG();
    
    // Method to process optionally the output star list from the catalog
    mcsCOMPL_STAT PostProcessList(vobsSTAR_LIST &list)
    {
        return mcsSUCCESS;
    }

protected:
    // Catalog filename to load
    const char *_filename;

    // Flag to know if catalog is loaded or not
    mcsLOGICAL _loaded;

    // Star list built from local catalog stars
    vobsSTAR_LIST _starList;

    // Load local catalog
    virtual mcsCOMPL_STAT Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap);

    virtual mcsCOMPL_STAT SetOption(const char* option);

    virtual mcsCOMPL_STAT Clear(void);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsLOCAL_CATALOG(const vobsLOCAL_CATALOG&);
    vobsLOCAL_CATALOG& operator=(const vobsLOCAL_CATALOG&) ;
} ;

#endif /*!vobsLOCAL_CATALOG_H*/

/*___oOo___*/


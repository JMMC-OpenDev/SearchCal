#ifndef vobsLOCAL_CATALOG_H
#define vobsLOCAL_CATALOG_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsLOCAL_CATALOG.h,v 1.1 2005-02-11 14:14:31 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * \usedfiles
 * \filename vobsMidiCatalog.cfg : star list used for MIDI calibrator search
 *
 * \n
 * \sa - vobsCATALOG class
 */
class vobsLOCAL_CATALOG : public vobsCATALOG
{
public:
    // Class constructor
    vobsLOCAL_CATALOG(const char *name, const char *filename);

    // Class destructor
    virtual ~vobsLOCAL_CATALOG();

    // Search for star list in MIDI catalog
    virtual mcsCOMPL_STAT Search (vobsREQUEST &request, vobsSTAR_LIST &list);
    
protected:
    // Flag to know if catalog is loaded or not
    mcsLOGICAL _loaded;

    // Star list built from MIDI catalog stars
    vobsSTAR_LIST _starList;

    // Load MIDI catalog
    virtual mcsCOMPL_STAT Load();

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsLOCAL_CATALOG(const vobsLOCAL_CATALOG&);
    vobsLOCAL_CATALOG& operator=(const vobsLOCAL_CATALOG&);

    // Catalog filename to load
    const char *_filename;
};

#endif /*!vobsLOCAL_CATALOG_H*/

/*___oOo___*/


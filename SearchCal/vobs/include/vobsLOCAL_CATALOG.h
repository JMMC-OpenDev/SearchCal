#ifndef vobsLOCAL_CATALOG_H
#define vobsLOCAL_CATALOG_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsLOCAL_CATALOG.h,v 1.3 2005-11-16 10:47:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/08/08 11:27:28  scetre
 * Moved specific treatment of midi catalog in the vobsCATALOG_MIDI class instead of vobsLOCAL_CATALOG class
 *
 * Revision 1.1  2005/02/11 14:14:31  gluck
 * Added vobsLOCAL_CATALOG  and vobsREMOTE_CATALOG classes to have a more coherent and homogenous inheritance tree
 *
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

protected:
    // Flag to know if catalog is loaded or not
    mcsLOGICAL _loaded;

    // Star list built from MIDI catalog stars
    vobsSTAR_LIST _starList;

    // Load MIDI catalog
    virtual mcsCOMPL_STAT Load(void);

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


#ifndef vobsCATALOG_MIDI_H
#define vobsCATALOG_MIDI_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_MIDI.h,v 1.1 2005-02-04 13:56:40 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_MIDI class.
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
 * Class which allows to query MIDI catalog.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa JMMC-MEM-2600-0011 document
 * 
 * \bug OPTIONAL. Bugs list if it exists.
 * \bug For example, description of the first bug
 * \bug For example, description of the second bug
 * 
 * \todo OPTIONAL. Things to forsee list, if needed. For example, 
 * \todo add other methods, dealing with operations.
 * 
 */
class vobsCATALOG_MIDI : public vobsCATALOG
{
public:
    // Class constructor
    vobsCATALOG_MIDI();

    // Class destructor
    virtual ~vobsCATALOG_MIDI();

    // Search for star list in MIDI catalog
    virtual mcsCOMPL_STAT Search (vobsREQUEST &request, vobsSTAR_LIST &list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_MIDI(const vobsCATALOG_MIDI&);
    vobsCATALOG_MIDI& operator=(const vobsCATALOG_MIDI&);

    // Catalog filename to load
    const char *_catalogFilename;

    // Flag to know if catalog is loaded or not
    mcsLOGICAL _loaded;

    // Star list built from MIDI catalog stars
    vobsSTAR_LIST _starList;

    // Load MIDI catalog
    virtual mcsCOMPL_STAT Load(void);
};

#endif /*!vobsCATALOG_MIDI_H*/

/*___oOo___*/


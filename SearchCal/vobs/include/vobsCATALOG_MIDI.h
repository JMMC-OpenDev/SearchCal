#ifndef vobsCATALOG_MIDI_H
#define vobsCATALOG_MIDI_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
#include "vobsLOCAL_CATALOG.h"


/*
 * Class declaration
 */

/**
 * Class used to query MIDI catalog.
 * 
 * Through MIDI catalog interrogation, this class alows to realise band N search
 * from user request. Provided selected stars, extracted from MIDI catalog, will
 * be caracterised by circumstellar dust lack and 12 mu IR flux greater than 5
 * Jy.
 *
 * A star will be extracted if it is located in the search field.
 *
 * Search field is defined through user request by:
 * - N magnitude range calculated from N object magnitude
 * - field on sky (heigh and width corresponding to search box)
 *
 * \usedfiles
 * \filename vobsMidiCatalog.cfg : star list used for MIDI calibrator search
 *
 * \n
 * \sa - vobsCATALOG class
 * \sa - JMMC-MEM-2600-0011 document
 */
class vobsCATALOG_MIDI : public vobsLOCAL_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_MIDI();

    // Class destructor
    virtual ~vobsCATALOG_MIDI();

    // Search for star list in MIDI catalog
    virtual mcsCOMPL_STAT Search(vobsREQUEST &request, vobsSTAR_LIST &list, mcsLOGICAL logResult = mcsFALSE);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_MIDI(const vobsCATALOG_MIDI&);
    vobsCATALOG_MIDI& operator=(const vobsCATALOG_MIDI&);

    // Load MIDI catalog
    virtual mcsCOMPL_STAT Load(void);
};

#endif /*!vobsCATALOG_MIDI_H*/

/*___oOo___*/


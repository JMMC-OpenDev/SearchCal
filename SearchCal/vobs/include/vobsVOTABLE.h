#ifndef vobsVOTABLE_H
#define vobsVOTABLE_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVOTABLE.h,v 1.1 2005-11-30 15:24:37 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsVOTABLE class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local Headers 
 */
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */

/**
 * Serialize a vobsSTAR_LIST object in a VOTable 1.1 XML file.
 * 
 * \sa vobsSTAR_LIST
 * \sa http://www.ivoa.net/Documents/latest/VOT.html
 * 
 * \todo VOTable parsing.
 */
class vobsVOTABLE
{

public:
    // Class constructor
    vobsVOTABLE();

    // Class destructor
    virtual ~vobsVOTABLE();

    // Serialize a star list in a VOTable 1.1 XML file
    mcsCOMPL_STAT Save(vobsSTAR_LIST& starList, const char* fileName);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsVOTABLE(const vobsVOTABLE&);
    vobsVOTABLE& operator=(const vobsVOTABLE&);
};

#endif /*!vobsVOTABLE_H*/

/*___oOo___*/

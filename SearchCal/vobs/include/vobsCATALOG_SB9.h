#ifndef vobsCATALOG_SB9_H
#define vobsCATALOG_SB9_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_SB9.h,v 1.1 2009-02-09 16:23:48 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsCATALOG_SB9 class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"


/*
 * Local header
 */
#include "vobsREMOTE_CATALOG.h"


/*
 * Class declaration
 */

/**
 * vobsCATALOG_SB9 is a class which handle Catalogue of Spectroscopic Binary
 * Orbits (Pourbaix+ 2004-2008).
 * 
 * Methods hereafter allow to:
 * \li prepare a request;
 * \li send this request to the CDS;
 * \li build a star list from the CDS answer.
 * 
 */
class vobsCATALOG_SB9 : public vobsREMOTE_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_SB9();

    // Class destructor
    virtual ~vobsCATALOG_SB9();

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_SB9(const vobsCATALOG_SB9&);
    vobsCATALOG_SB9& operator=(const vobsCATALOG_SB9&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_SB9_H*/

/*___oOo___*/

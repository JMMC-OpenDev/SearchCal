#ifndef vobsCATALOG_AKARI_H
#define vobsCATALOG_AKARI_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_AKARI.h,v 1.1.2.1 2011-04-08 19:42:49 duvert Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2010/01/26 14:15:38  lafrasse
 * Added vobsCATALOG_AKARI.
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsCATALOG_AKARI class.
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
 * vobsCATALOG_AKARI is a class which handle The AKARI/IRC mid-IR all-sky Survey (ISAS/JAXA, 2010)
 * (Ishihara D. et al 2010)
 * 
 * Methods hereafter allow to:
 * \li prepare a request;
 * \li send this request to the CDS;
 * \li build a star list from the CDS answer.
 * 
 */
class vobsCATALOG_AKARI : public vobsREMOTE_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_AKARI();

    // Class destructor
    virtual ~vobsCATALOG_AKARI();

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_AKARI(const vobsCATALOG_AKARI&);
    vobsCATALOG_AKARI& operator=(const vobsCATALOG_AKARI&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_AKARI_H*/

/*___oOo___*/

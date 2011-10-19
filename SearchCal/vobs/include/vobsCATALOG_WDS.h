#ifndef vobsCATALOG_WDS_H
#define vobsCATALOG_WDS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsCATALOG_WDS class.
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
 * vobsCATALOG_WDS is a class which handle The Washington Double Star Catalog
 * (Mason+ 2001-2010).
 * 
 * Methods hereafter allow to:
 * \li prepare a request;
 * \li send this request to the CDS;
 * \li build a star list from the CDS answer.
 * 
 */
class vobsCATALOG_WDS : public vobsREMOTE_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_WDS();

    // Class destructor
    virtual ~vobsCATALOG_WDS();

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_WDS(const vobsCATALOG_WDS&);
    vobsCATALOG_WDS& operator=(const vobsCATALOG_WDS&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
};

#endif /*!vobsCATALOG_WDS_H*/

/*___oOo___*/

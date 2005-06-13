#ifndef vobsCATALOG_DENIS_JK_H
#define vobsCATALOG_DENIS_JK_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_DENIS_JK.h,v 1.1 2005-06-13 10:19:30 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_DENIS_JK class.
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
 * vobsCATALOG_DENIS_JK is a class which caracterise the DENIS database.
 * 
 * vobsCATALOG_DENIS_JK methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsCATALOG_DENIS_JK : public vobsREMOTE_CATALOG
{

public:
    // Class constructor
    vobsCATALOG_DENIS_JK();

    // Class destructor
    virtual ~vobsCATALOG_DENIS_JK();

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_DENIS_JK(const vobsCATALOG_DENIS_JK&);
    vobsCATALOG_DENIS_JK& operator=(const vobsCATALOG_DENIS_JK&);

    // Method to build parts of the request
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

};

#endif /*!vobsCATALOG_DENIS_JK_H*/

/*___oOo___*/

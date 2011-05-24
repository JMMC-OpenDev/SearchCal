#ifndef vobsCATALOG_ASCC_LOCAL_H
#define vobsCATALOG_ASCC_LOCAL_H
/*******************************************************************************
 * JMMC project
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_ASCC_LOCAL class.
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
 * Class used to query ASCC_LOCAL catalog.
 * 
 * Through ASCC_LOCAL catalog interrogation, this class allows to get all the primary stars for JSDC
 *
 * \usedfiles
 * \filename vobsAsccExecrptsForJSDC.cfg : star list used for ASCC_LOCAL calibrator search
 *
 * \n
 * \sa - vobsCATALOG class
 */
class vobsCATALOG_ASCC_LOCAL : public vobsLOCAL_CATALOG
{
public:
    // Class constructor
    vobsCATALOG_ASCC_LOCAL();

    // Class destructor
    virtual ~vobsCATALOG_ASCC_LOCAL();

    // Search for star list in ASCC catalog
    virtual mcsCOMPL_STAT Search (vobsREQUEST &request, vobsSTAR_LIST &list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_ASCC_LOCAL(const vobsCATALOG_ASCC_LOCAL&);
    vobsCATALOG_ASCC_LOCAL& operator=(const vobsCATALOG_ASCC_LOCAL&);

    // Load ASCC_LOCAL catalog
    virtual mcsCOMPL_STAT Load(void);
};

#endif /*!vobsCATALOG_ASCC_LOCAL_H*/

/*___oOo___*/


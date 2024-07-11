#ifndef vobsCATALOG_LIST_H
#define vobsCATALOG_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_LIST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System headers
 */
using namespace std;
#include <map>

/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local Headers
 */
#include "vobsCATALOG.h"
#include "vobsREMOTE_CATALOG.h"
#include "vobsCATALOG_ASCC_LOCAL.h"
#include "vobsCATALOG_JSDC_FAINT_LOCAL.h"


/* pre-initialize vobs module (dev flag) */
void vobsPreInit();

/* initialize vobs module (vobsCATALOG meta data) */
void vobsInit();

/* clean vobs module on exit */
void vobsExit();

/** Catalog pointer map keyed by catalog ID */
typedef std::map<const vobsORIGIN_INDEX, vobsCATALOG*> vobsCATALOG_PTR_MAP;

/*
 * Class declaration
 */

/**
 * vobsCATALOG_LIST handles the list of available catalogs.
 */
class vobsCATALOG_LIST
{
public:
    // Class constructor
    vobsCATALOG_LIST();

    // Class destructor
    virtual ~vobsCATALOG_LIST();

    vobsCATALOG* Get(vobsORIGIN_INDEX catalogId) const;

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_LIST(const vobsCATALOG_LIST&);
    vobsCATALOG_LIST& operator=(const vobsCATALOG_LIST&) ;

    vobsCATALOG_PTR_MAP _catalogMap;

    // List of available catalogs
    vobsREMOTE_CATALOG _akari;
    vobsREMOTE_CATALOG _ascc;
    vobsCATALOG_ASCC_LOCAL _jsdc_local; // local
    vobsCATALOG_JSDC_FAINT_LOCAL _jsdc_faint_local; // local
    vobsREMOTE_CATALOG _bsc;
    vobsREMOTE_CATALOG _cio;
    vobsREMOTE_CATALOG _denis;
    vobsREMOTE_CATALOG _denisJK;
    vobsREMOTE_CATALOG _hic;
    vobsREMOTE_CATALOG _hip1;
    vobsREMOTE_CATALOG _hip2;
    vobsREMOTE_CATALOG _mass;
    vobsREMOTE_CATALOG _photo;
    vobsREMOTE_CATALOG _sbsc;
    vobsREMOTE_CATALOG _sb9;
    vobsREMOTE_CATALOG _usno;
    vobsREMOTE_CATALOG _wds;
    vobsREMOTE_CATALOG _wise;
    vobsREMOTE_CATALOG _gaiaDR3;
    vobsREMOTE_CATALOG _gaiaDR3ap;
    vobsREMOTE_CATALOG _mdfc;
} ;


#endif /*!vobsCATALOG_LIST_H*/

/*___oOo___*/

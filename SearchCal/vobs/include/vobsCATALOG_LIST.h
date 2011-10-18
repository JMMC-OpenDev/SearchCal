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
#include "vobsCATALOG_ASCC.h"
#include "vobsCATALOG_ASCC_LOCAL.h"
#include "vobsCATALOG_BSC.h"
#include "vobsCATALOG_CIO.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG_DENIS_JK.h"
#include "vobsCATALOG_HIC.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_MERAND.h"
#include "vobsCATALOG_MASS.h"
#include "vobsCATALOG_PHOTO.h"
#include "vobsCATALOG_SBSC.h"
#include "vobsCATALOG_MIDI.h"
#include "vobsCATALOG_UNSO.h"
#include "vobsCATALOG_SB9.h"
#include "vobsCATALOG_WDS.h"
#include "vobsCATALOG_AKARI.h"

/*
 * const char* comparator used by map<const char*, ...> defined in vobsSTAR.h
 */
struct constStringComparator;

/* catalog map type using char* keys and custom comparator functor */
typedef std::map<const char*, vobsCATALOG*, constStringComparator> CatalogList;

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

    vobsCATALOG* Get(const char* catalogName) const;

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_LIST(const vobsCATALOG_LIST&);
    vobsCATALOG_LIST& operator=(const vobsCATALOG_LIST&);

    CatalogList _catalogList;

    // List of available catalogs
    vobsCATALOG_CIO        _cio;
    vobsCATALOG_PHOTO      _photo;
    vobsCATALOG_ASCC       _ascc;
    vobsCATALOG_ASCC_LOCAL _ascc_local;
    vobsCATALOG_HIC        _hic;
    vobsCATALOG_MASS       _mass;
    vobsCATALOG_LBSI       _lbsi;
    vobsCATALOG_MERAND     _merand;
    vobsCATALOG_BSC        _bsc;
    vobsCATALOG_SBSC       _sbsc;
    vobsCATALOG_DENIS      _denis;
    vobsCATALOG_DENIS_JK   _denisJK;
    vobsCATALOG_MIDI       _midi;
    vobsCATALOG_UNSO       _unso;
    vobsCATALOG_SB9        _sb9;
    vobsCATALOG_WDS        _wds;
    vobsCATALOG_AKARI      _akari;
};


#endif /*!vobsCATALOG_LIST_H*/

/*___oOo___*/

#ifndef vobsCATALOG_LIST_H
#define vobsCATALOG_LIST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_LIST.h,v 1.3 2005-11-29 10:27:06 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/24 14:54:25  scetre
 * UNSO catalog added
 *
 * Revision 1.1  2005/11/15 14:56:43  scetre
 * Create catalog list
 *
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
#include "vobsCATALOG_BSC.h"
#include "vobsCATALOG_CHARM.h"
#include "vobsCATALOG_CHARM2.h"
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

    vobsCATALOG * Get(string catalogName);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_LIST(const vobsCATALOG_LIST&);
    vobsCATALOG_LIST& operator=(const vobsCATALOG_LIST&);

    map<string, vobsCATALOG *> _catalogList;
    map<string, vobsCATALOG *>::iterator _catalogListIterator;

    // List of available catalogs
    vobsCATALOG_CIO      _cio;
    vobsCATALOG_PHOTO    _photo;
    vobsCATALOG_ASCC     _ascc;
    vobsCATALOG_HIC      _hic;
    vobsCATALOG_MASS     _mass;
    vobsCATALOG_LBSI     _lbsi;
    vobsCATALOG_MERAND   _merand;
    vobsCATALOG_CHARM    _charm;
    vobsCATALOG_CHARM2   _charm2;
    vobsCATALOG_BSC      _bsc;
    vobsCATALOG_SBSC     _sbsc;
    vobsCATALOG_DENIS    _denis;
    vobsCATALOG_DENIS_JK _denisJK;
    vobsCATALOG_MIDI     _midi;
    vobsCATALOG_UNSO     _unso;
};


#endif /*!vobsCATALOG_LIST_H*/

/*___oOo___*/

#ifndef vobsSCENARIO_ENTRY_H
#define vobsSCENARIO_ENTRY_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.h,v 1.3 2005-02-08 11:10:04 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/01/27 15:55:54  scetre
 * scenario became a friend class of scenario entry to be able to access private members
 *
 * Revision 1.1  2005/01/27 13:42:54  scetre
 * vobsSCENARIO_ENTRY created. It move from vobsSCENARIO
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsSCENARIO_ENTRY class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local Header
 */
#include "vobsCATALOG.h"
#include "vobsSTAR_LIST.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

/*
 * Class declaration
 */

/**
 * Class which define an entry.
 *
 * this class is made by private member. An entry is defined by a catalog to
 * query, two list of star (list input and list output), an action to realise
 * and a list of association criteria. 
 */

/*
 * Enumeration type definition
 */
/**
 * vobsACTION_ID is an enumeration which allow correspondance between an id and
 * an ACTION.
 */
typedef enum
{
    vobsNO_ACTION = -1,
    vobsCOPY,
    vobsMERGE,
    vobsUPDATE_ONLY
} vobsACTION;

class vobsSCENARIO_ENTRY
{
public:
    // Class constructor
    vobsSCENARIO_ENTRY();
    vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY&);
    vobsSCENARIO_ENTRY(vobsCATALOG                  *catalog,
                       vobsSTAR_LIST                *listInput,
                       vobsSTAR_LIST                *listOutput,
                       vobsACTION                   action,
                       vobsSTAR_COMP_CRITERIA_LIST  criteriaList);
    
    // Class destructor
    virtual ~vobsSCENARIO_ENTRY();
    friend class vobsSCENARIO;

protected:
    
private:
    // Declaration of copy constructor as private
    // methods, in order to hide them from the users.
    vobsSCENARIO_ENTRY& operator=(const vobsSCENARIO_ENTRY&);

    vobsCATALOG                 *_catalog;
    vobsSTAR_LIST               *_listInput;
    vobsSTAR_LIST               *_listOutput;
    vobsACTION                  _action;
    vobsSTAR_COMP_CRITERIA_LIST _criteriaList;
};

#endif /*!vobsSCENARIO_ENTRY_H*/

/*___oOo___*/

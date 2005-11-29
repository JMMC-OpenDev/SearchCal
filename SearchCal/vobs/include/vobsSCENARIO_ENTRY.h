#ifndef vobsSCENARIO_ENTRY_H
#define vobsSCENARIO_ENTRY_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.h,v 1.8 2005-11-29 10:30:04 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.6  2005/11/15 14:57:56  scetre
 * Added new scenario structure
 * Added possibility to query merand and borde as primary catalog
 *
 * Revision 1.5  2005/02/09 06:09:57  gzins
 * Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
 *
 * Revision 1.4  2005/02/08 11:10:04  scetre
 * changed action enumerate <NAME> in vobs<NAME>
 *
 * Revision 1.3  2005/02/08 11:10:04  scetre
 * changed action enumerate <NAME> in vobs<NAME>
 *
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
#include "vobsFILTER.h"

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
    vobsCOPY,
    vobsMERGE,
    vobsUPDATE_ONLY
} vobsACTION;

class vobsSCENARIO_ENTRY
{
public:
    // Class constructor
    vobsSCENARIO_ENTRY(mcsSTRING32                  catalogName,
                       vobsREQUEST                  *request,
                       vobsSTAR_LIST                *listInput,
                       vobsSTAR_LIST                *listOutput,
                       vobsACTION                   action,
                       vobsSTAR_COMP_CRITERIA_LIST  *criteriaList,
                       vobsFILTER                   *filter);
    vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY&);
    
    // Class destructor
    virtual ~vobsSCENARIO_ENTRY();
    friend class vobsSCENARIO;

protected:
    
private:
    // Declaration of copy constructor as private
    // methods, in order to hide them from the users.
    vobsSCENARIO_ENTRY& operator=(const vobsSCENARIO_ENTRY&);

    mcsSTRING32                 _catalogName;
    vobsREQUEST                 *_request;
    vobsSTAR_LIST               *_listInput;
    vobsSTAR_LIST               *_listOutput;
    vobsACTION                  _action;
    vobsSTAR_COMP_CRITERIA_LIST *_criteriaList;
    vobsFILTER             *_filter;
};

#endif /*!vobsSCENARIO_ENTRY_H*/

/*___oOo___*/

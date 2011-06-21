#ifndef vobsSCENARIO_ENTRY_H
#define vobsSCENARIO_ENTRY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    vobsSCENARIO_ENTRY(const mcsSTRING32            catalogName,
                       vobsREQUEST                  *request,
                       vobsSTAR_LIST                *listInput,
                       vobsSTAR_LIST                *listOutput,
                       vobsACTION                   action,
                       vobsSTAR_COMP_CRITERIA_LIST  *criteriaList,
                       vobsFILTER                   *filter);
    vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY&);
    
    mcsCOMPL_STAT SetQueryOption(string queryOption);
    string GetQueryOption(void);
    
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
    vobsFILTER                  *_filter;
    string                      _queryOption;
};

#endif /*!vobsSCENARIO_ENTRY_H*/

/*___oOo___*/

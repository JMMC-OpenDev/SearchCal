#ifndef sclsvrFILTER_LIST_H
#define sclsvrFILTER_LIST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrFILTER_LIST.h,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrFILTER_LIST class.
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
#include "sclsvrFILTER.h"

/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 */
class sclsvrFILTER_LIST
{

public:
    // Class constructor
    sclsvrFILTER_LIST();

    // Class destructor
    virtual ~sclsvrFILTER_LIST();

    virtual mcsCOMPL_STAT Add(sclsvrFILTER *filter);
    virtual mcsCOMPL_STAT Reset(void);
    virtual mcsUINT32 Size();
    virtual sclsvrFILTER *GetNextFilter(mcsLOGICAL init = mcsFALSE);
    virtual sclsvrFILTER *GetFilter(mcsSTRING32 name);
    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrFILTER_LIST(const sclsvrFILTER_LIST&);
    sclsvrFILTER_LIST& operator=(const sclsvrFILTER_LIST&);

    std::list<sclsvrFILTER *> _filterList;
    std::list<sclsvrFILTER *>::iterator _filterlistIterator;
};

#endif /*!sclsvrFILTER_LIST_H*/

/*___oOo___*/

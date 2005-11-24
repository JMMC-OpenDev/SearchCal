#ifndef vobsBASE_FILTER_H
#define vobsBASE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsBASE_FILTER.h,v 1.1 2005-11-24 08:13:50 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsBASE_FILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */
/**
 * Base Filter class
 *
 * A filter is define by a status (enable or disable) and a filter property
 * which can be a numeric value, a string, a boolean
 */
class vobsBASE_FILTER
{

public:
    // Class constructor
    vobsBASE_FILTER();

    // Class destructor
    virtual ~vobsBASE_FILTER();

    virtual char * GetName(void);
    virtual mcsLOGICAL IsEnabled(void);
    virtual mcsCOMPL_STAT Enable(void);
    virtual mcsCOMPL_STAT Disable(void);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list) = 0;
    
protected:
    mcsSTRING32 _name;    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsBASE_FILTER(const vobsBASE_FILTER&);
    vobsBASE_FILTER& operator=(const vobsBASE_FILTER&);

    mcsLOGICAL _isEnable;    
};

#endif /*!vobsBASE_FILTER_H*/

/*___oOo___*/

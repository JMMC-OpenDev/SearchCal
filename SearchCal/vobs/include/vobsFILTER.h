#ifndef vobsFILTER_H
#define vobsFILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.h,v 1.9 2005-11-29 13:46:38 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/11/29 10:28:00  gzins
 * Moved vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.1  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsFILTER class.
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
 * Filter class
 *
 * Class to filter out a list of stars; i.e. all stars which do not match the
 * filter conditions are removed from the list. 
 */
class vobsFILTER
{

public:
    // Class constructor
    vobsFILTER();

    // Class destructor
    virtual ~vobsFILTER();

    virtual mcsLOGICAL    IsEnabled(void);
    virtual mcsCOMPL_STAT Enable(void);
    virtual mcsCOMPL_STAT Disable(void);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list) = 0;
    
protected:
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER(const vobsFILTER&);
    vobsFILTER& operator=(const vobsFILTER&);

    mcsLOGICAL _isEnable;    
};

#endif /*!vobsFILTER_H*/

/*___oOo___*/

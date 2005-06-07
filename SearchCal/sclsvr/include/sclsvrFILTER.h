#ifndef sclsvrFILTER_H
#define sclsvrFILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrFILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of sclsvrFILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "sclsvrCALIBRATOR_LIST.h"

/*
 * Class declaration
 */

/**
 * Filter class
 *
 * A filter is define by a status (enable or disable) and a filter property
 * which can be a numeric value, a string, a boolean
 */
class sclsvrFILTER
{

public:
    // Class constructor
    sclsvrFILTER();

    // Class destructor
    virtual ~sclsvrFILTER();

    virtual char * GetName();
    virtual mcsLOGICAL IsEnable();
    virtual mcsCOMPL_STAT Enable();
    virtual mcsCOMPL_STAT Disable();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list) = 0;

protected:
    mcsSTRING32 _name;
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrFILTER(const sclsvrFILTER&);
    sclsvrFILTER& operator=(const sclsvrFILTER&);

    mcsLOGICAL _isEnable;
};

#endif /*!sclsvrFILTER_H*/

/*___oOo___*/

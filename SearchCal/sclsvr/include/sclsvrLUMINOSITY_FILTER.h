#ifndef sclsvrLUMINOSITY_FILTER_H
#define sclsvrLUMINOSITY_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrLUMINOSITY_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
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
 * Declaration of sclsvrLUMINOSITY_FILTER class.
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
#include "sclsvrCALIBRATOR_LIST.h"


#define sclsvrLUMINOSITY_FILTER_NAME "Filter by luminosity"
#define sclsvrNB_LUMINOSITY_CLASS 6
/*
 * Class declaration
 */

/**
 * luminosity filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrLUMINOSITY_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrLUMINOSITY_FILTER();

    // Class destructor
    virtual ~sclsvrLUMINOSITY_FILTER();

    virtual mcsCOMPL_STAT
        SetLuminosity(std::list<char *> lumClassList);

    virtual std::list<char *> * GetLuminosity();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);
   
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrLUMINOSITY_FILTER(const sclsvrLUMINOSITY_FILTER&);
    sclsvrLUMINOSITY_FILTER& operator=(const sclsvrLUMINOSITY_FILTER&);

    std::list<char *> _luminosityClassList;
};

#endif /*!sclsvrLUMINOSITY_FILTER_H*/

/*___oOo___*/

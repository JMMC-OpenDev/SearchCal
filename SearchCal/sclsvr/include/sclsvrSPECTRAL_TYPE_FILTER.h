#ifndef sclsvrSPECTRAL_TYPE_FILTER_H
#define sclsvrSPECTRAL_TYPE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSPECTRAL_TYPE_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
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
 * Declaration of sclsvrSPECTRAL_TYPE_FILTER class.
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


#define sclsvrSPECTRAL_TYPE_FILTER_NAME "Filter by Spectral type"

/*
 * Class declaration
 */

/**
 * Spectral type filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrSPECTRAL_TYPE_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrSPECTRAL_TYPE_FILTER();

    // Class destructor
    virtual ~sclsvrSPECTRAL_TYPE_FILTER();

    virtual mcsCOMPL_STAT SetSpectralType(std::list<char *> tempClassList);

    virtual std::list<char *> * GetSpectralType();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSPECTRAL_TYPE_FILTER(const sclsvrSPECTRAL_TYPE_FILTER&);
    sclsvrSPECTRAL_TYPE_FILTER& operator=(const sclsvrSPECTRAL_TYPE_FILTER&);

    std::list<char *> _tempClassList;
};

#endif /*!sclsvrSPECTRAL_TYPE_FILTER_H*/

/*___oOo___*/

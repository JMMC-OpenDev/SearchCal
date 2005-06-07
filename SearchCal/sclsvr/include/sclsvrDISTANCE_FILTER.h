#ifndef sclsvrDISTANCE_FILTER_H
#define sclsvrDISTANCE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrDISTANCE_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
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
 * Declaration of sclsvrDISTANCE_FILTER class.
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

#define sclsvrDISTANCE_FILTER_NAME "Filter by Distance"

/*
 * Class declaration
 */

/**
 * Distance filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrDISTANCE_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrDISTANCE_FILTER();

    // Class destructor
    virtual ~sclsvrDISTANCE_FILTER();

    virtual mcsCOMPL_STAT SetDistanceValue(mcsSTRING32 raRef,
                                           mcsSTRING32 decRef,
                                           mcsFLOAT raRange,
                                           mcsFLOAT decRange);
    virtual mcsCOMPL_STAT GetDistanceValue(mcsSTRING32 *raRef,
                                           mcsSTRING32 *decRef,
                                           mcsFLOAT *raRange,
                                           mcsFLOAT *decRange);
    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrDISTANCE_FILTER(const sclsvrDISTANCE_FILTER&);
    sclsvrDISTANCE_FILTER& operator=(const sclsvrDISTANCE_FILTER&);

    mcsSTRING32 _raRef;
    mcsSTRING32 _decRef;
    mcsFLOAT _raRange;
    mcsFLOAT _decRange;
};

#endif /*!sclsvrDISTANCE_FILTER_H*/

/*___oOo___*/

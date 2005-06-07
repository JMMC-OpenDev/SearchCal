#ifndef sclsvrMAGNITUDE_FILTER_H
#define sclsvrMAGNITUDE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrMAGNITUDE_FILTER.h,v 1.2 2005-06-07 12:35:59 scetre Exp $"
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
 * Declaration of sclsvrMAGNITUDE_FILTER class.
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


#define sclsvrMAGNITUDE_FILTER_NAME "Filter by Magnitude"

/*
 * Class declaration
 */

/**
 * Magnitude filter class.
 * 
 * This class is a sclsvrFILTER object.
 *
 * @sa sclsvrFILTER.cpp
 * @sa sclsvrCALIBRATOR_LIST.cpp
 * 
 */
class sclsvrMAGNITUDE_FILTER : public sclsvrFILTER
{

public:
    // Class constructor
    sclsvrMAGNITUDE_FILTER();

    // Class destructor
    virtual ~sclsvrMAGNITUDE_FILTER();

    virtual mcsCOMPL_STAT SetMagnitudeValue(mcsSTRING32 band,
                                            mcsFLOAT magValue,
                                            mcsFLOAT magRange);

    virtual mcsCOMPL_STAT GetMagnitudeValue(mcsSTRING32 *band,
                                            mcsFLOAT *magValue,
                                            mcsFLOAT *magRange);
    
    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrMAGNITUDE_FILTER(const sclsvrMAGNITUDE_FILTER&);
    sclsvrMAGNITUDE_FILTER& operator=(const sclsvrMAGNITUDE_FILTER&);

    mcsSTRING32 _band;
    mcsFLOAT _magValue;
    mcsFLOAT _magRange;
};

#endif /*!sclsvrMAGNITUDE_FILTER_H*/

/*___oOo___*/

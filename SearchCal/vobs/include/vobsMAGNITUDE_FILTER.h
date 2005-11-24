#ifndef vobsMAGNITUDE_FILTER_H
#define vobsMAGNITUDE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsMAGNITUDE_FILTER.h,v 1.2 2005-11-24 08:13:50 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:35:59  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsMAGNITUDE_FILTER class.
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
#include "vobsBASE_FILTER.h"
#include "vobsSTAR_LIST.h"


#define vobsMAGNITUDE_FILTER_NAME "Filter by Magnitude"

/*
 * Class declaration
 */

/**
 * Magnitude filter class.
 * 
 * This class is a vobsBASE_FILTER object.
 *
 * @sa vobsBASE_FILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsMAGNITUDE_FILTER : public vobsBASE_FILTER
{

public:
    // Class constructor
    vobsMAGNITUDE_FILTER();

    // Class destructor
    virtual ~vobsMAGNITUDE_FILTER();

    virtual mcsCOMPL_STAT SetMagnitudeValue(mcsSTRING32 band,
                                            mcsFLOAT magValue,
                                            mcsFLOAT magRange);

    virtual mcsCOMPL_STAT GetMagnitudeValue(mcsSTRING32 *band,
                                            mcsFLOAT *magValue,
                                            mcsFLOAT *magRange);
    
    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsMAGNITUDE_FILTER(const vobsMAGNITUDE_FILTER&);
    vobsMAGNITUDE_FILTER& operator=(const vobsMAGNITUDE_FILTER&);

    mcsSTRING32 _band;
    mcsFLOAT _magValue;
    mcsFLOAT _magRange;
};

#endif /*!vobsMAGNITUDE_FILTER_H*/

/*___oOo___*/

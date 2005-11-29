#ifndef vobsMAGNITUDE_FILTER_H
#define vobsMAGNITUDE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsMAGNITUDE_FILTER.h,v 1.3 2005-11-29 10:28:46 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
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
#include "vobsFILTER.h"
#include "vobsSTAR_LIST.h"


#define vobsMAGNITUDE_FILTER_NAME "Filter by Magnitude"

/*
 * Class declaration
 */

/**
 * Magnitude filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsMAGNITUDE_FILTER : public vobsFILTER
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

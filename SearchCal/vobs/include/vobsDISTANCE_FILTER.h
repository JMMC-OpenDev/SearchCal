#ifndef vobsDISTANCE_FILTER_H
#define vobsDISTANCE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsDISTANCE_FILTER.h,v 1.8 2011-03-03 13:09:42 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2009/12/09 10:00:10  lafrasse
 * Updated to truly compute distance as star separation and not as coordinates box
 * selection, to better filter science objects in sclsvr.
 *
 * Revision 1.6  2006/03/01 16:41:38  lafrasse
 * Added const attributes for SetDistanceValue() method parameters.
 * Replaced all the 'strcpy' calls with 'strncpy'.
 * Code review
 *
 * Revision 1.5  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.4  2005/11/29 13:45:57  gzins
 * Removed filter name definition
 *
 * Revision 1.3  2005/11/29 10:27:23  gzins
 * Changed parent class to vobsFILTER
 *
 * Revision 1.2  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.1  2005/06/20 11:44:25  scetre
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
 * Declaration of vobsDISTANCE_FILTER class.
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

/*
 * Class declaration
 */

/**
 * Distance filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsDISTANCE_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsDISTANCE_FILTER(const char* filterId);

    // Class destructor
    virtual ~vobsDISTANCE_FILTER();

    virtual mcsCOMPL_STAT SetDistanceValue(const mcsSTRING32  raRef,
                                           const mcsSTRING32  decRef,
                                           const mcsDOUBLE     distance);

    virtual mcsCOMPL_STAT GetDistanceValue(mcsSTRING32  *raRef,
                                           mcsSTRING32  *decRef,
                                           mcsDOUBLE     *distance);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsDISTANCE_FILTER(const vobsDISTANCE_FILTER&);
    vobsDISTANCE_FILTER& operator=(const vobsDISTANCE_FILTER&);

    mcsSTRING32  _raRef;
    mcsSTRING32  _decRef;
    mcsDOUBLE     _distance;
};

#endif /*!vobsDISTANCE_FILTER_H*/

/*___oOo___*/

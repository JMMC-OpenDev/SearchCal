#ifndef sclsvrSCENARIO_BRIGHT_K_H
#define sclsvrSCENARIO_BRIGHT_K_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_K.h,v 1.3 2005-11-24 13:19:37 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/16 14:28:02  scetre
 * Used criteria list as member of the class. All necessary criteria list are now built at the beginning of the Init() method
 *
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_BRIGHT_K class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobs.h"

/*
 * Class declaration
 */

/**
 * Specific scenario for K band research
 */
class sclsvrSCENARIO_BRIGHT_K : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_BRIGHT_K();

    // Class destructor
    virtual ~sclsvrSCENARIO_BRIGHT_K();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_BRIGHT_K(const sclsvrSCENARIO_BRIGHT_K&);
    sclsvrSCENARIO_BRIGHT_K& operator=(const sclsvrSCENARIO_BRIGHT_K&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;

    vobsREQUEST _request;
    vobsREQUEST _requestI280;

    // filter on origin
    vobsORIGIN_FILTER _originFilter;
    // filter on magnitude
    vobsMAGNITUDE_FILTER _magnitudeFilter;
    // filter list build with the 2 filter on origin and magnitude
    vobsFILTER_LIST _filterList;
    // filter on B-V for I/280
    vobsBV_FILTER _bvFilter;
    // filter on opt=T
    vobsFILTER _filterOptT;
    
    // criteria list for LBSI, Merand, charm2, denis JK, 2MASS, II/225
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDec;
    // criteria list for II/7A
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecMagV;
    // criteria list for I/196, BSC, SBSC
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecHd;
};

#endif /*!sclsvrSCENARIO_BRIGHT_K_H*/

/*___oOo___*/

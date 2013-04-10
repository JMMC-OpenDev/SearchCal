#ifndef vobsREMOTE_CATALOG_H
#define vobsREMOTE_CATALOG_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsREMOTE_CATALOG class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS header
 */
#include "misc.h"

/*
 * Local header
 */
#include "vobsCATALOG.h"

/** Get the vizier URI in use */
char* vobsGetVizierURI();

/** Free the vizier URI */
void vobsFreeVizierURI();


/** TargetId mapping (targetId_epoch1, targetId_epoch2) type using const char* key / value pairs */
typedef std::map<const char*, const char*, constStringComparator> vobsTARGET_ID_MAPPING;

/** TargetId pair (targetId_epoch1, targetId_epoch2) */
typedef std::pair<const char*, const char*> vobsTARGET_ID_PAIR;

/*
 * Class declaration
 */

/**
 * vobsCATALOG is a class which caracterise a remote catalog.
 * 
 * vobsCATALOG methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */
class vobsREMOTE_CATALOG : public vobsCATALOG
{
public:
    // Constructor
    vobsREMOTE_CATALOG(const char *name);

    // Destructor
    virtual ~vobsREMOTE_CATALOG();

    // Method to get a  star list from the catalog
    mcsCOMPL_STAT Search(vobsREQUEST &request, vobsSTAR_LIST &list,
                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE);

protected:

    // Method to process optionally the output star list from the catalog
    virtual mcsCOMPL_STAT ProcessList(vobsSTAR_LIST &list);

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsREMOTE_CATALOG& operator=(const vobsCATALOG&);
    vobsREMOTE_CATALOG(const vobsCATALOG&);

    // Method to prepare the request in a string format
    mcsCOMPL_STAT PrepareQuery(vobsREQUEST &request);
    mcsCOMPL_STAT PrepareQuery(vobsREQUEST &request, vobsSTAR_LIST &tmpList);

    mcsCOMPL_STAT WriteQueryConstantPart(vobsREQUEST &request, vobsSTAR_LIST &tmpList);

    // Method to build all parts of the asking
    mcsCOMPL_STAT WriteQueryBandPart(const char* band, mcsSTRING32 &rangeMag);
    mcsCOMPL_STAT WriteQueryURIPart(void);
    mcsCOMPL_STAT WriteQuerySpecificPart(void);
    mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);
    mcsCOMPL_STAT WriteReferenceStarPosition(vobsREQUEST &request);
    mcsCOMPL_STAT WriteQueryStarListPart(vobsSTAR_LIST &list);

    // Write option
    mcsCOMPL_STAT WriteOption(void);

    // Method to get a star list in a string format from a normal star list
    // format
    mcsCOMPL_STAT StarList2String(miscDYN_BUF &strList, const vobsSTAR_LIST &list);

    mcsCOMPL_STAT GetAverageEpochSearchRadius(const vobsSTAR_LIST &list, mcsDOUBLE &radius);
    mcsCOMPL_STAT GetEpochSearchArea(const vobsSTAR_LIST &list, mcsDOUBLE &deltaRA, mcsDOUBLE &deltaDEC);

    void ClearTargetIdIndex();

    // Request to write and to send to the CDS
    miscDYN_BUF _query;

    /** targetId index: used only when the precession to catalog's epoch is needed */
    vobsTARGET_ID_MAPPING* _targetIdIndex;

};

#endif /*!vobsREMOTE_CATALOG_H*/


/*___oOo___*/

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
#include "vobsSCENARIO_RUNTIME.h"

/** Get the vizier URI in use */
char* vobsGetVizierURI();

/** Free the vizier URI */
void vobsFreeVizierURI();

/* Cancel flag stored in thread local storage */
bool vobsIsCancelled(void);
void vobsSetCancelFlag(bool* cancelFlag);

/* Thread Cancel Flag handling */
mcsCOMPL_STAT vobsCancelInit(void);
mcsCOMPL_STAT vobsCancelExit(void);

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
    vobsREMOTE_CATALOG(vobsORIGIN_INDEX catalogId);

    // Destructor
    virtual ~vobsREMOTE_CATALOG();

    // Method to get a  star list from the catalog
    mcsCOMPL_STAT Search(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST &request, vobsSTAR_LIST &list, const char* option,
                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap, mcsLOGICAL logResult = mcsFALSE);

    // Method to process optionally the output star list from the catalog
    mcsCOMPL_STAT PostProcessList(vobsSTAR_LIST &list);

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsREMOTE_CATALOG& operator=(const vobsCATALOG&) ;
    vobsREMOTE_CATALOG(const vobsCATALOG&);

    // Method to prepare the request in a string format
    mcsCOMPL_STAT PrepareQuery(miscoDYN_BUF* query, vobsREQUEST &request, const char* option);
    mcsCOMPL_STAT PrepareQuery(vobsSCENARIO_RUNTIME &ctx, miscoDYN_BUF* query, vobsREQUEST &request, vobsSTAR_LIST &tmpList, const char* option);

    mcsCOMPL_STAT WriteQueryConstantPart(miscoDYN_BUF* query, vobsREQUEST &request, vobsSTAR_LIST &tmpList);

    // Method to build all parts of the asking
    mcsCOMPL_STAT WriteQueryBandPart(miscoDYN_BUF* query, const char* band, const mcsSTRING32 rangeMag);
    mcsCOMPL_STAT WriteQueryURIPart(miscoDYN_BUF* query);
    mcsCOMPL_STAT WriteQuerySpecificPart(miscoDYN_BUF* query);
    mcsCOMPL_STAT WriteQuerySpecificPart(miscoDYN_BUF* query, vobsREQUEST &request);
    mcsCOMPL_STAT WriteReferenceStarPosition(miscoDYN_BUF* query, vobsREQUEST &request);
    mcsCOMPL_STAT WriteQueryStarListPart(vobsSCENARIO_RUNTIME &ctx, miscoDYN_BUF* query, vobsSTAR_LIST &list);

    // Write option
    mcsCOMPL_STAT WriteOption(miscoDYN_BUF* query, const char* option);

    // Method to get a star list in a string format from a normal star list
    // format
    mcsCOMPL_STAT StarList2String(vobsSCENARIO_RUNTIME &ctx, miscoDYN_BUF* query, const vobsSTAR_LIST &list);

    mcsCOMPL_STAT GetAverageEpochSearchRadius(const vobsSTAR_LIST &list, mcsDOUBLE &radius);
    mcsCOMPL_STAT GetEpochSearchArea(const vobsSTAR_LIST &list, mcsDOUBLE &deltaRA, mcsDOUBLE &deltaDEC);

    // Method to process optionally the output star list from the catalog
    mcsCOMPL_STAT ProcessList(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &list);

} ;

#endif /*!vobsREMOTE_CATALOG_H*/


/*___oOo___*/

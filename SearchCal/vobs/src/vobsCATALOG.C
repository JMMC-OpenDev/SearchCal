/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.C,v 1.4 2004-07-28 14:19:41 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    27-Jul-2004  Created
*
*
*******************************************************************************/


/**
 * \file
 * vobsCATALOG class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG.C,v 1.4 2004-07-28 14:19:41 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;  /**< Export standard iostream objects (cin, cout,...) */


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"


/*
 * Local Headers 
 */
#include "vobsCATALOG.h"
#include "vobsREQUEST.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Local Variables
 */

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG::vobsCATALOG()
{
    strcpy(_name,"");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG::~vobsCATALOG()
{
}

/*
 * Public methods
 */

/**
 * Set a catalog name.
 *
 * \param name name value to set as a string
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_BAD_CATALOG_NAME
 */
mcsCOMPL_STAT vobsCATALOG::SetName(char *name)
{
    logExtDbg("vobsCATALOG::SetName()");
    
    // Test if the name is correst
    if (strcmp(name,"")==0 ||
        name==NULL)
    {
        errAdd(vobsERR_BAD_CATALOG_NAME);
        return FAILURE;
    }
    
    strcpy(_name, name);
    return SUCCESS;
}

/**
 * Get a catalog name as a string
 *
 * \param name  name to get, as a string
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_BAD_CATALOG_NAME
 */
mcsCOMPL_STAT vobsCATALOG::GetName(char *name)
{
    logExtDbg("vobsCATALOG::GetName()");
    
    if (_name==NULL)
    {
        errAdd(vobsERR_BAD_CATALOG_NAME);
        return FAILURE;
    }
    
    strcpy(name,_name);
    return SUCCESS;
}


/**
 * Search in the catalog a list of star.
 *
 * Search int the catalog a list of star according to a vobsREQUEST
 *
 * \param request a vobsREQUEST which have all the contraints for the search
 * \param list a vobsSTAR_LIST as the result of the search
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \sa vobsREQUEST
 *
 * \b Errors codes:\n 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsCATALOG::Search(vobsREQUEST request, vobsSTAR_LIST list)
{
    logExtDbg("vobsCATALOG::Search()");

    return SUCCESS;
}
/*
 * Protected methods
 */

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints) for a first ask
 * to the CDS, that's mean that the use of this asking will help to have a
 * list of possible star
 *
 * \param request vobsREQUEST which have all the contraints for the search
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Errors codes:\n 
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::PrepareAsking(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::PrepareAsking()");

    if ((WriteAskingURI()==FAILURE) ||
        (WriteAskingPosition(request)==FAILURE) ||
        (WriteAskingSpecificParameters(request)==FAILURE) )
    {
        errAdd(vobsERR_ASKING_WRITE_FAILED, _asking);
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints). The knowledge of
 * another list of star help to create the asking for a final ask to the CDS.
 *
 * \param request vobsREQUEST which have all the contraints for the search  
 * \param tmpList vobsSTAR_LIST which come from an older ask to the CDS. 
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsCATALOG::PrepareAsking(vobsREQUEST request, vobsSTAR_LIST tmpList)
{
    logExtDbg("vobsCATALOG::PrepareAsking()");

    if ( (WriteAskingURI()==FAILURE) ||
         (WriteAskingConstant()==FAILURE) ||
         (WriteAskingSpecificParameters()==FAILURE) ||
         (WriteAskingEnd(tmpList)==FAILURE) )
    {
        errAdd(vobsERR_ASKING_WRITE_FAILED, _asking);
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Build the destination part of the asking.
 *
 * Build the destination part of the asking. All catalog files are located on
 * web server. It is possible to find them on the URL : 
 * http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source= ...
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingURI(void)
{
    logExtDbg("vobsCATALOG::GetAskingURI()");

    if ((miscDynStrAppendString(&_asking, "http://vizier.u-strasbg.fr/viz-bin/")==FAILURE) ||
        (miscDynStrAppendString(&_asking, "asu-xml?-source=")==FAILURE) ||
        (miscDynStrAppendString(&_asking, _name)==FAILURE) )
    {
        errAdd(vobsERR_URI_WRITE_FAILED);
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Build the constant part of the asking
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingConstant(void)
{
    logExtDbg("vobsCATALOG::GetAskingConstant()");

    if ( (miscDynStrAppendString(&_asking,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin")==FAILURE) ||
         (miscDynStrAppendString(&_asking,"&-out.max=50")==FAILURE) ||
         (miscDynStrAppendString(&_asking,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms")==FAILURE) )
    {
        errAdd(vobsERR_CONSTANT_WRITE_FAILED);
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG::GetAskingSpecificParameters()");

    miscDynStrAppendString(&_asking,"");    
    
    return SUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * \param request vobsREQUEST which help to restrict the search
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingSpecificParameters(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::GetAskingSpecificParameters()");

    miscDynStrAppendString(&_asking,"");

    return SUCCESS;
}

/**
 * Build the position box part of the asking.
 *
 * Build the position box part of the asking. This method is used in case of
 * restrictive search.
 *
 * \param request vobsREQUEST which help to restrict the search 
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingPosition(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::GetAskingPosition()");

    char *ra="";
    request.GetConstraint(SEARCH_BOX_RA_ID,ra);
    char *dec="";
    request.GetConstraint(SEARCH_BOX_DEC_ID,dec);
    
    if ( (miscDynStrAppendString(&_asking,"&-c.ra=")==FAILURE) ||
         (miscDynStrAppendString(&_asking,ra)==FAILURE) ||
         (miscDynStrAppendString(&_asking,"&-c.dec=")==FAILURE) ||
         (miscDynStrAppendString(&_asking,dec)==FAILURE) )
    {
        errAdd(vobsERR_POSITION_WRITE_FAILED);
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * Buil the end part of the asking.
 *
 * The end part of the asking for a search from a star list.
 *
 * \param list vobsSTAR_LIST which help to build the end part
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteAskingEnd(vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG::GetAskingEnd()");

    // Build of the stringlist
    char *strList="";
    StarList2Sring(strList, list);
    
    if ( (miscDynStrAppendString(&_asking,"&-out.form=List")==FAILURE) ||
         (miscDynStrAppendString(&_asking, strList)==FAILURE) )
    {
        errAdd(vobsERR_END_WRITE_FAILED);
        return FAILURE;
    }
    
    return SUCCESS;
}

/**
 * Convert a star list to a string list.
 *
 * The research of specific star knowong their coordonate need to write in the
 * asking the list of coordonate as a string. This method convert the position
 * of all star present in a star list in a string.
 *
 * \param stringList string list as a string
 * \param list star list to cnvert
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::StarList2Sring(char *stringList,
                                          vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG::StarList2Sring()");

    return SUCCESS;
}
/*
 * Private methods
 */
/*___oOo___*/

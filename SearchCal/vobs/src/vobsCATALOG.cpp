/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.cpp,v 1.7 2005-01-31 13:31:38 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/01/26 08:11:28  scetre
* change history
*
* scetre    27-Jul-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with nez mcsLOGICAL enumerate
*
******************************************************************************/


/**
 * \file
 * vobsCATALOG class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG.cpp,v 1.7 2005-01-31 13:31:38 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;  
/**
 * Export standard iostream objects (cin, cout,...).
 */


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
#include "vobsPARSER.h"
#include "vobsCATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"
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
    miscDynBufInit(&_query);
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
    miscDynBufDestroy(&_query);
}

/*
 * Public methods
 */

/**
 * Set a catalog name.
 *
 * \param name name value to set, in a string format
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_BAD_CATALOG_NAME
 */
mcsCOMPL_STAT vobsCATALOG::SetName(char *name)
{
    logExtDbg("vobsCATALOG::SetName()");
    
    // Test if the name is correst
    if ( (strcmp(name,"")==0) || (name==NULL) )
    {
        errAdd(vobsERR_BAD_CATALOG_NAME);
        return mcsFAILURE;
    }
    
    // copy in the catalog name attribute the value to set
    strcpy(_name, name);
    return mcsSUCCESS;
}

/**
 * Get a catalog name in a string format
 *
 * \param name  name to get, in a string format
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_BAD_CATALOG_NAME
 */
mcsCOMPL_STAT vobsCATALOG::GetName(char *name)
{
    logExtDbg("vobsCATALOG::GetName()");
    
    // Check if the name is not aqual to NULL. it it is equal return error
    if (_name==NULL)
    {
        errAdd(vobsERR_BAD_CATALOG_NAME);
        return mcsFAILURE;
    }
    // copy the catalog name in the name to return
    strcpy(name,_name);
    return mcsSUCCESS;
}


/**
 * Search in the catalog a list of star.
 *
 * Search int the catalog a list of star according to a vobsREQUEST
 *
 * \param request a vobsREQUEST which have all the contraints for the search
 * \param list a vobsSTAR_LIST as the result of the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \sa vobsREQUEST
 *
 * \b Errors codes:\n 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsCATALOG::Search(vobsREQUEST &request, vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG::Search()");
    
    // Prepare file name to log result of the catalog request
    mcsSTRING256 logFileName;
    // if the log level is higher or equal to the debug level
    if (logGetStdoutLogLevel() >= logDEBUG)
    {
        // build the first part of the file name in the MCSDATA directory
        strcpy(logFileName, "$MCSDATA/tmp/list_");

        // Get band used for search
        mcsSTRING32 band;
        request.GetConstraint(OBSERVED_BAND_ID,band);
        // copy the observed band in the filename
        strcat(logFileName, band);

        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalogName;
        strcpy (catalogName, _name);
        miscReplaceChrByChr(catalogName, '/', '_');
        strcat(logFileName, "_");
        strcat(logFileName, catalogName);
        // the list is mpty the data which will be write in the file will come
        // from a "primary" asking
        if (list.IsEmpty()==mcsTRUE)
        {
            strcat(logFileName, "_1.log");
        }
        else
        {
            strcat(logFileName, "_2.log");
        }
    }
    else
    {
       memset((char *)logFileName , '\0', sizeof(logFileName)); 
    }

    // Check if the list is empty
    // if ok, the asking is writing according to only the request
    if (list.IsEmpty()==mcsTRUE)
    {
        if (PrepareQuery(request)==mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // else, the asking is writing according to the request and the star list
    else 
    {
        if (PrepareQuery(request, list)==mcsFAILURE)
        { 
            return mcsFAILURE; 
        }
    }
    
    
    // create a parser object
    vobsPARSER parser;
    // the parser get the internet of the query and analyse th file coming
    // from this address
    if (parser.Parse(miscDynBufGetBuffer(&_query), list, logFileName)
        == mcsFAILURE)
    {
        return mcsFAILURE; 
    }
    
    return mcsSUCCESS;
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors codes:\n 
 * The possible errors are:
 * \li vobsERR_QUERY_WRITE_FAILED
 *
 */
mcsCOMPL_STAT vobsCATALOG::PrepareQuery(vobsREQUEST &request)
{
    logExtDbg("vobsCATALOG::PrepareQuery()");
    
    // Reset the dynamic buffer which contain the query    
    miscDynBufReset(&_query);

    // in this case of request, there are three parts to write :
    // the location
    // the position of the reference star
    // the specific part of the query
    if ((WriteQueryURIPart()==mcsFAILURE) ||
        (WriteReferenceStarPosition(request)==mcsFAILURE) ||
        (WriteQuerySpecificPart(request)==mcsFAILURE) )
    {
        errAdd(vobsERR_QUERY_WRITE_FAILED, _query);
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 * \li vobsERR_QUERY_WRITE_FAILED
 */
mcsCOMPL_STAT vobsCATALOG::PrepareQuery(vobsREQUEST request, vobsSTAR_LIST &tmpList)
{
    logExtDbg("vobsCATALOG::PrepareQuery()");
    
    miscDynBufReset(&_query);
    // in this case of request, there are four parts to write :
    // the location
    // the constant part of the query
    // the specific part of the query
    // the list to complete
    if ( (WriteQueryURIPart()==mcsFAILURE) ||
         (WriteQueryConstantPart()==mcsFAILURE) ||
         (WriteQuerySpecificPart()==mcsFAILURE) ||
         (WriteQueryStarListPart(tmpList)==mcsFAILURE) )
    {
        errAdd(vobsERR_QUERY_WRITE_FAILED, _query);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Build the destination part of the asking.
 *
 * Build the destination part of the asking. All catalog files are located on
 * web server. It is possible to find them on the URL : 
 * http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source= ...
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 * \li vobsERR_URI_WRITE_FAILED
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteQueryURIPart(void)
{
    logExtDbg("vobsCATALOG::WriteQueryURI()");

    if ((miscDynBufAppendString(&_query, "http://vizier.u-strasbg.fr/viz-bin/")==mcsFAILURE) ||
        (miscDynBufAppendString(&_query, "asu-xml?-source=")==mcsFAILURE) ||
        (miscDynBufAppendString(&_query, _name)==mcsFAILURE) )
    {
        errAdd(vobsERR_URI_WRITE_FAILED);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Build the constant part of the asking
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 * \li vobsERR_CONSTANT_WRITE_FAILED
 */
mcsCOMPL_STAT vobsCATALOG::WriteQueryConstantPart(void)
{
    logExtDbg("vobsCATALOG::GetAskingConstant()");

    if ( (miscDynBufAppendString(&_query,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin")==mcsFAILURE) ||
         (miscDynBufAppendString(&_query,"&-out.max=50")==mcsFAILURE) ||
         (miscDynBufAppendString(&_query,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms")==mcsFAILURE) )
    {
        errAdd(vobsERR_CONSTANT_WRITE_FAILED);
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG::GetAskingSpecificParameters()");

    
    return mcsSUCCESS;
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteQuerySpecificPart(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::GetAskingSpecificParameters()");


    return mcsSUCCESS;
}

/**
 * Build the position box part of the asking.
 *
 * Build the position box part of the asking. This method is used in case of
 * restrictive search.
 *
 * \param request vobsREQUEST which help to restrict the search 
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 * vobsERR_POSITION_WRITE_FAILED
 */
mcsCOMPL_STAT vobsCATALOG::WriteReferenceStarPosition(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::GetAskingPosition()");

    mcsSTRING32 ra;
    request.GetConstraint(RA_ID,ra);
    mcsSTRING32 dec;
    request.GetConstraint(DEC_ID,dec);
    
    if ( (miscDynBufAppendString(&_query,"&-c.ra=")==mcsFAILURE) ||
         (miscDynBufAppendString(&_query,ra)==mcsFAILURE) ||
         (miscDynBufAppendString(&_query,"&-c.dec=")==mcsFAILURE) ||
         (miscDynBufAppendString(&_query,dec)==mcsFAILURE) )
    {
        errAdd(vobsERR_POSITION_WRITE_FAILED);
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Buil the end part of the asking.
 *
 * The end part of the asking for a search from a star list.
 *
 * \param list vobsSTAR_LIST which help to build the end part
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::WriteQueryStarListPart(vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG::GetAskingEnd()");
    // Build of the stringlist
    miscDYN_BUF strList;
    miscDynBufInit(&strList);
    // write a star list object as a dynamic buffer in order to write it in a
    // string format in the query
    StarList2Sring(strList, list);
    
    
    if ( (miscDynBufAppendString(&_query,"&-out.form=List")==mcsFAILURE) ||
         (miscDynBufAppendString(&_query, miscDynBufGetBuffer(&strList))==mcsFAILURE) )
    {
        errAdd(vobsERR_END_WRITE_FAILED);
        miscDynBufDestroy(&strList);
        return mcsFAILURE;
    }

    miscDynBufDestroy(&strList);
    
    return mcsSUCCESS;
}

/**
 * Convert a star list to a string list.
 *
 * The research of specific star knowong their coordonate need to write in the
 * asking the list of coordonate as a string. This method convert the position
 * of all star present in a star list in a string.
 *
 * \param strList string list as a string
 * \param list star list to cnvert
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG::StarList2Sring(miscDYN_BUF &strList,
                                          vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG::StarList2Sring()");
    
    // if the list is not empty
    if (list.Size()!=0)
    {
        miscDynBufAppendString(&strList,"&-c=%3C%3C%3D%3D%3D%3Dresult1%5F280%2Etxt&");
        int compt=0;
        
        for (unsigned int el = 0; el < list.Size(); el++)
        {
            if (compt!=0)
            {
                miscDynBufAppendString(&strList,"&+");
            }
            compt++;
            
            mcsSTRING32 ra;
            mcsSTRING32 hra, mra, sra;
            mcsSTRING32 dec;
            mcsSTRING32 ddec, mdec, sdec;
            
            vobsSTAR *star = list.GetNextStar((mcsLOGICAL)(el==0));
            strcpy(ra, star->GetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN));
            /*{
                if (errIsInStack(MODULE_ID, 
                                 vobsERR_PROPERTY_NOT_SET) == mcsTRUE)
                {
                    errResetStack();
                }
                return mcsFAILURE;
            }*/
            if (sscanf(ra, "%s %s %s",
                       (char*)&hra,
                       (char*)&mra,
                       (char*)&sra) != 3)
            {
                return mcsFAILURE;
            }
            miscDynBufAppendString(&strList, hra);
            miscDynBufAppendString(&strList,"+");
            miscDynBufAppendString(&strList, mra);
            miscDynBufAppendString(&strList,"+");
            miscDynBufAppendString(&strList, sra);
            

            strcpy(dec, star->GetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN));
            /*{
                // if get property failed because of property not set, ignore
                // error 
                if (errIsInStack(MODULE_ID, 
                                 vobsERR_PROPERTY_NOT_SET) == mcsTRUE)
                {
                    errResetStack();
                }
                return mcsFAILURE;
            }*/
            if (sscanf(dec, "%s %s %s",
                       (char*)&ddec,
                       (char*)&mdec,
                       (char*)&sdec) != 3)
            {
                return mcsFAILURE;
            }
            if (ddec[0]=='+')
            {
                miscDynBufAppendString(&strList,"%2b");
                miscDynBufAppendString(&strList, &ddec[1]);
                miscDynBufAppendString(&strList,"+");
            }
            else
            {
                miscDynBufAppendString(&strList, ddec);
                miscDynBufAppendString(&strList,"+");
            }
            miscDynBufAppendString(&strList, mdec);
            miscDynBufAppendString(&strList,"+");
            miscDynBufAppendString(&strList, sdec);
        }
            


        miscDynBufAppendString(&strList,"&%3D%3D%3D%3Dresult1%5F280%2Etxt");
        
    }
    return mcsSUCCESS;
}
/*
 * Private methods
 */
/*___oOo___*/

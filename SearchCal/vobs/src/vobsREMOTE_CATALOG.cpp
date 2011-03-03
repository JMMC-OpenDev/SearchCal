/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREMOTE_CATALOG.cpp,v 1.20 2011-03-03 13:09:43 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.19  2011/02/10 14:16:28  lafrasse
* Moved back to official version of Vizier service (not beta).
*
* Revision 1.18  2011/02/10 13:44:38  lafrasse
* Increased default max number of stars retruned by CDS from 50 to 1000.
*
* Revision 1.17  2010/02/15 15:43:30  mella
* Move to POST query method
*
* Revision 1.16  2010/01/22 15:38:07  lafrasse
* Remote Vizier beta testing.
*
* Revision 1.15  2010/01/22 15:35:29  lafrasse
* Log refinments.
*
* Revision 1.14  2009/02/03 08:53:08  mella
* Made changes to fall back on UCD1 with viz-bin service
*
* Revision 1.13  2008/03/10 07:53:42  lafrasse
* Changed VIZIER URI to reflect CDS changes.
* Minor modifications on comments and log traces.
*
* Revision 1.12  2006/08/25 05:56:35  gzins
* Removed useless errors
*
* Revision 1.11  2006/03/03 15:03:28  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.10  2006/01/18 08:45:38  scetre
* Added option in generic local and remote catalog
*
* Revision 1.9  2005/11/29 08:22:23  scetre
* Minor changes
*
* Revision 1.8  2005/11/21 13:47:57  scetre
* arrange properties when the URL is written
*
* Revision 1.7  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.6  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.5  2005/06/01 14:16:55  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.4  2005/03/04 15:35:02  gzins
* Removed printf
*
* Revision 1.3  2005/02/22 10:44:51  gzins
* Fixed bug related to the saved files; environment variables was not resolved
*
* Revision 1.2  2005/02/16 13:22:38  scetre
* changed armin in arcsec in query
*
* Revision 1.1  2005/02/11 14:14:31  gluck
* Added vobsLOCAL_CATALOG  and vobsREMOTE_CATALOG classes to have a more coherent and homogenous inheritance tree
*
******************************************************************************/


/**
 * @file
 * Definition vobsREMOTE_CATALOG class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsREMOTE_CATALOG.cpp,v 1.20 2011-03-03 13:09:43 lafrasse Exp $"; 

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
#include "vobsREMOTE_CATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"
#include "vobsPARSER.h"

/*
 * Local Variables
 */

#define vobsVIZIER_URI "http://viz-beta.u-strasbg.fr/viz-bin/asu-xml?"
//#define vobsVIZIER_URI "http://vizier.u-strasbg.fr/viz-bin/asu-xml?"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsREMOTE_CATALOG::vobsREMOTE_CATALOG(const char *name) : vobsCATALOG(name)
{
    // Initialise dynamic buffer corresponding to query
    miscDynBufInit(&_query);
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsREMOTE_CATALOG::~vobsREMOTE_CATALOG()
{
    // Destroy dynamic buffer corresponding to query
    miscDynBufDestroy(&_query);
}


/*
 * Public methods
 */

/**
 * Search in the catalog a list of star.
 *
 * Search int the catalog a list of star according to a vobsREQUEST
 *
 * @param request a vobsREQUEST which have all the contraints for the search
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \sa vobsREQUEST
 *
 * \b Errors codes:\n 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::Search(vobsREQUEST &request, vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::Search()");
    
    // Prepare file name to log result of the catalog request
    mcsSTRING512 logFileName;
    // if the log level is higher or equal to the debug level
    if (logGetStdoutLogLevel() >= logDEBUG)
    {
        // Get band used for search
        const char *band;
        band = request.GetSearchBand();

        // build the first part of the file name in the MCSDATA directory
        sprintf(logFileName, "$MCSDATA/tmp/list_%s", band);

        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalogName;
        strcpy (catalogName, _name.c_str());
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
        // Resolve path
        const char   *resolvedPath;
        resolvedPath = miscResolvePath(logFileName);
        if (resolvedPath != NULL)
        {
            strcpy(logFileName, resolvedPath);
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
        if (PrepareQuery(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // else, the asking is writing according to the request and the star list
    else 
    {
        if (PrepareQuery(request, list) == mcsFAILURE)
        { 
            return mcsFAILURE; 
        }
    }
    
    
    // create a parser object
    vobsPARSER parser;
    // the parser get the internet of the query and analyse th file coming
    // from this address
    if (parser.Parse(vobsVIZIER_URI, miscDynBufGetBuffer(&_query),
                     GetName(), list, logFileName) == mcsFAILURE)
    {
        return mcsFAILURE; 
    }
    return mcsSUCCESS;
}

/**
 * Set catalog option
 *
 * @param option the option to add in the query
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::SetOption(string option)
{
    logTrace("vobsREMOTE_CATALOG::SetOption()");

    _option = option;

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
 * @param request vobsREQUEST which have all the contraints for the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(vobsREQUEST &request)
{
    logTrace("vobsREMOTE_CATALOG::PrepareQuery()");
    
    // Reset the dynamic buffer which contain the query    
    miscDynBufReset(&_query);

    // in this case of request, there are three parts to write :
    // the location
    // the position of the reference star
    // the specific part of the query
    if (WriteQueryURIPart()==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteReferenceStarPosition(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQuerySpecificPart(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteOption() == mcsFAILURE)
    {
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
 * @param request vobsREQUEST which have all the contraints for the search  
 * @param tmpList vobsSTAR_LIST which come from an older ask to the CDS. 
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(vobsREQUEST &request, 
                                               vobsSTAR_LIST &tmpList)
{
    logTrace("vobsREMOTE_CATALOG::PrepareQuery()");
    
    miscDynBufReset(&_query);
    // in this case of request, there are four parts to write :
    // the location
    // the constant part of the query
    // the specific part of the query
    // the list to complete
    if (WriteQueryURIPart()==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQueryConstantPart() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQuerySpecificPart() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteOption() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQueryStarListPart(tmpList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Build the destination part of the query.
 *
 * Build the destination part of the query. All catalog files are located on
 * web server. It is possible to find them on the URL : 
 * http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source= ...
 * * &-out.meta=hudU1&-oc.form=sexa has been added o get previous UCD1 instead
 * of UCD1+ with the
 *  * rest of information
 *   * more info found here http://cdsweb.u-strasbg.fr/doc/asu-summary.htx
 *  -oc.form forces rigth coordinates h:m:s (dispite given param -oc=hms)
 *  -out.meta=
 *    h -> add column names into cdata header (required by our parser)
 *    u -> retrieve column units as viz1bin used to do by default
 *    d -> retrieve column descriptions as viz1bin used to do by default
 *    U1 -> request ucd1 instead of ucd1+
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryURIPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryURIPart()");

    if (miscDynBufAppendString(&_query, "-source=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, _name.c_str()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, "&-out.meta=hudU1&-oc.form=sexa") == mcsFAILURE)
    {
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryConstantPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryConstantPart()");

    if (miscDynBufAppendString(&_query,
                               "&-file=-c&-c.eq=J2000&-c.r=5&-c.u=arcsec")
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (miscDynBufAppendString(&_query, "&-out.max=1000") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms")
        == mcsFAILURE)
    {
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQuerySpecificPart()");

    
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsREMOTE_CATALOG::WriteQuerySpecificPart()");

    return mcsSUCCESS;
}

/**
 * Build the position box part of the asking.
 *
 * Build the position box part of the asking. This method is used in case of
 * restrictive search.
 *
 * @param request vobsREQUEST which help to restrict the search 
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteReferenceStarPosition(vobsREQUEST &request)
{
    logTrace("vobsREMOTE_CATALOG::WriteReferenceStarPosition()");

    const char *ra;
    ra = request.GetObjectRa();
    const char *dec;
    dec = request.GetObjectDec();
    
    if (miscDynBufAppendString(&_query, "&-c.ra=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, "&-c.dec=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Buil the end part of the asking.
 *
 * The end part of the asking for a search from a star list.
 *
 * @param list vobsSTAR_LIST which help to build the end part
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryStarListPart(vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryStarListPart()");
    // Build of the stringlist
    miscDYN_BUF strList;
    miscDynBufInit(&strList);
    // write a star list object as a dynamic buffer in order to write it in a
    // string format in the query
    StarList2Sring(strList, list);
    
    if ( (miscDynBufAppendString(&_query,"&-out.form=List") == mcsFAILURE) ||
         (miscDynBufAppendString(&_query,
                                 miscDynBufGetBuffer(&strList)) == mcsFAILURE))
    {
        miscDynBufDestroy(&strList);
        return mcsFAILURE;
    }

    miscDynBufDestroy(&strList);
    
    return mcsSUCCESS;
}

/**
 * Write option
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteOption()
{
    logTrace("vobsREMOTE_CATALOG::WriteOption()");

    miscDynBufAppendString(&_query, _option.c_str());            

    return mcsSUCCESS;
}

/**
 * Convert a star list to a string list.
 *
 * The research of specific star knowong their coordonate need to write in the
 * asking the list of coordonate as a string. This method convert the position
 * of all star present in a star list in a string.
 *
 * @param strList string list as a string
 * @param list star list to cnvert
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::StarList2Sring(miscDYN_BUF &strList,
                                          vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::StarList2Sring()");
    
    // if the list is not empty
    if (list.Size()!=0)
    {
        miscDynBufAppendString(&strList,
                               "&-c=%3C%3C%3D%3D%3D%3Dresult1%5F280%2Etxt&");
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

/*___oOo___*/

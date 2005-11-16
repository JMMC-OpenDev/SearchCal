/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.33 2005-11-16 12:53:18 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.32  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.31  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.30  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.29  2005/09/08 08:30:44  scetre
* Changed position range from 1'' to 10''
* Removed mgK criteria before 2MASS secondary request
*
* Revision 1.28  2005/08/03 14:02:00  scetre
* Reorder catalog for the K band research. Add in this research 2mass as a primary catalog
*
* Revision 1.27  2005/06/21 06:20:45  scetre
* Changed method Disable() and Enable() to Disabled() and Enabled()
* Changed '\' in doxygen documentatiuon to '@'
*
* Revision 1.26  2005/06/13 10:19:47  scetre
* Added Charm2, Merand and new Denis catalogs
*
* Revision 1.25  2005/06/01 14:16:56  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.24  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.23  2005/02/24 13:12:43  scetre
* Added user error if the CDS doesn't answered
*
* Revision 1.22  2005/02/22 13:23:16  scetre
* Removed association criteria on magnitude
*
* Revision 1.21  2005/02/16 17:02:04  scetre
* Updated criteria association
*
* Revision 1.20  2005/02/16 14:28:31  scetre
* *** empty log message ***
*
* Revision 1.19  2005/02/10 08:07:38  scetre
* changed parser and hd, hip, dm number id in order to get all of them even if they have the same UCD
*
* Revision 1.18  2005/02/09 06:10:44  gzins
* Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
*
* Revision 1.17  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.16  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.15  2005/02/08 04:44:14  gzins
* Cleared all input/output lists when creating scenario
*
* Revision 1.14  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
*
* Revision 1.13  2005/02/07 17:36:00  scetre
* updated band V scenario
*
* Revision 1.12  2005/02/04 14:31:50  scetre
* updated documentation
*
* Revision 1.11  2005/02/04 10:50:37  gzins
* Added N band scenario
*
* Revision 1.10  2005/02/04 07:51:15  gzins
* Changed printf to logTest
*
* Revision 1.9  2005/02/04 07:43:16  gzins
* Fixed bug related to creation of primary list when porviding a existing list of stars; list was merged instaed of copied.
*
* Revision 1.8  2005/01/28 12:48:24  scetre
* changed association criteria
*
* Revision 1.7  2005/01/27 13:46:57  scetre
* change order of entry in K band
*
* Revision 1.6  2005/01/26 08:20:00  scetre
* Chamge history. Use instence of criteria list instead of pointeur. Update criteria list affectation in LoadScenario()
*
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * @file
 * vobsVIRTUAL_OBSERVATORY class definition.
 */

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.33 2005-11-16 12:53:18 scetre Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/*
 * System Headers
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers
 */
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a virtual observatory object.
 */

vobsVIRTUAL_OBSERVATORY::vobsVIRTUAL_OBSERVATORY()
{
}

/*
 * Class destructor
 */

/**
 * Delete a virtual observatory object.
 */
vobsVIRTUAL_OBSERVATORY::~vobsVIRTUAL_OBSERVATORY()
{
}

/**
 * Method to start the research according to the constarints of the request.
 *
 * @param scenario query scenario
 * @param request a vobsREQUEST build
 * @param starList list of Stars to build and to send as the result of the
 * research
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */

/*
 * Public methods
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Search(vobsSCENARIO *scenario,
                                              vobsREQUEST &request,
                                              vobsSTAR_LIST &starList)
{
    logTrace("vobsVIRTUAL_OBSERVATORY::Research()");

    //Set the catalogList 
    scenario->SetCatalogList(&_catalogList);

    // Run the method to execute the scenario which had been loaded into memory
    if (scenario->Execute(starList) == mcsFAILURE)
    {
        errUserAdd(vobsERR_NO_CDS_RETURN);
        return mcsFAILURE;
    }
    
    //starList.Display();
    logTest("Number of stars found : %d\n", starList.Size());

    return mcsSUCCESS;
}


/*
 * Protected methods
 */


/*___oOo___*/

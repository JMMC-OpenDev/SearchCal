/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsServer.C,v 1.2 2004-07-19 09:25:00 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsServer.C,v 1.2 2004-07-19 09:25:00 scetre Exp $"; 
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
#define MODULE_ID "vobs"

/*
 * Local Headers 
 */

#include "vobs.h"
/*
 * Local Variables
 */


/*
 * Local Functions
 */
// main
int main(int argc, char *argv[])
{
    logSetVerboseLevel(logEXTDBG);
    logInfo("Starting ...");
    
    char *alpha="1"; 
    char *delta="2";

    char *starName="ETA_TAU";
    char *wavelength="2.2";
    char *magnitudeB="magnitudeB";
    char *band="K";
    char *soMagnitude="2.96";
    char *magnitudeRange="1..5";
    char *searchRA="03+47+29.08";
    char *searchDEC="24+06+18.5";
    char *visibility="0.922";
    char *reqAbsErr="reqAbsErr";
    char *visErr="4.5E-02";
    char *basemin="25";
    char *basemax="100";


/*    char *starName="ZET_GEM";
    char *wavelength="0.65";
    char *magnitudeB="magnitudeB";
    char *band="V";
    char *soMagnitude="3.62";
    char *magnitudeRange="1.5..5.5";
    char *searchRA="07+04+06.53";
    char *searchDEC="20+34+13.1";
    char *visibility="0.245";
    char *reqAbsErr="reqAbsErr";
    char *visErr="1.2E-02";
    char *basemin="25";
    char *basemax="60";
*/ 

    vobsREQUEST request;
    request.Create(starName, wavelength, soMagnitude, magnitudeRange, searchRA, searchDEC, visibility, reqAbsErr, band);
   
    vobsVIRTUAL_OBSERVATORY virtualObservatory;
    
    vobsCALIBRATOR_STAR_LIST calibObjList;
    calibObjList=virtualObservatory.Research(request);
    
    logInfo("Server exiting ...");
    return 0;
}

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSearch.C,v 1.1 2004-07-13 13:25:16 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    12-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsSearch.C,v 1.1 2004-07-13 13:25:16 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"

#define MODULE_ID "vobs"

/*
 * Local Headers 
 */
#include "vobsCALIBRATOR_STAR.h"
#include "vobsCALIBRATOR_STAR_LIST.h"
#include "vobsCATALOG.h"
#include "vobsPARSER.h"
#include "vobsREQUEST.h"
#include "vobsVIRTUAL_OBSERVATORY.h"

/*
 * Local Variables
 */

 
/* 
 * Signal catching functions  
 */


/*
 * Local Functions
 */



int main(int argc, char *argv[])
{
    logSetVerboseLevel(logINFO );

    char *starInit=argv[1];
    //star=new char[strlen(argv)+1];
    //strcpy(star,argv);
    cout << starInit << endl;
    
    char *star;
    strcpy(star,"");
    char *tmp=(char *)strtok(starInit," ");
    while (tmp!=NULL)
    {
        strcat(star,tmp);
        tmp=(char *)strtok(NULL," ");
        if (tmp!=NULL)
            strcat(star,"%20");
    }

    //cout << star << endl;

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
vobsREQUEST request;
    request.Create(star, wavelength, soMagnitude, magnitudeRange, searchRA, searchDEC, visibility, reqAbsErr, band);
    vobsVIRTUAL_OBSERVATORY virtualObservatory;
    
    vobsCALIBRATOR_STAR_LIST calibObjList;
    calibObjList=virtualObservatory.ResearchOne(request);
    calibObjList.Print();
    return 0;

}


/*___oOo___*/

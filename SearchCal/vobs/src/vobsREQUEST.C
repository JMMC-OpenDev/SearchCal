/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.C,v 1.2 2004-07-20 07:21:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsREQUEST.C,v 1.2 2004-07-20 07:21:48 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

#define MODULE_ID "vobs"
/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "vobs.h"
//#include "vobsPrivate.h"

/*
 * Local Headers 
 */
#include "vobs.h"
//#include "vobsREQUEST.h"

/*
 * Local Variables
 */


/*
 * Local Functions
 */
// Class constructor 
vobsREQUEST::vobsREQUEST()
{
    starName=NULL;
    wavelength=NULL;
    soMagnitude=NULL;
    magnitudeRange=NULL;
    searchRA=NULL;
    searchDEC=NULL;
    visibility=NULL;
    reqAbsErr=NULL;
    band=NULL;
    build=0;
}

// Class destructor 
vobsREQUEST::~vobsREQUEST()
{
}

void vobsREQUEST::SetStarName(char *x)
{
  logExtDbg("vobsREQUEST::SetStarName()\n");
  starName=new char[strlen(x)+1];
  strcpy(starName,x);
}

void vobsREQUEST::SetWaveLength(char *x)
{
  logExtDbg("vobsREQUEST::SetWaveLength()\n");
  wavelength=new char[strlen(x)+1];
  strcpy(wavelength,x);
}

void vobsREQUEST::SetSoMagnitude(char *x)
{
  logExtDbg("vobsREQUEST::SetSoMagnitude()\n");
  soMagnitude=new char[strlen(x+1)];
  strcpy(soMagnitude,x);
}

void vobsREQUEST::SetMagnitudeRange(char *x)
{
  logExtDbg("vobsREQUEST::SetMagnitudeRange()\n");
  magnitudeRange=new char[strlen(x)+1];
  strcpy(magnitudeRange,x);
}

void vobsREQUEST::SetSearchRA(char *x)
{
  logExtDbg("vobsREQUEST::SetSearchRA()\n");
  searchRA=new char[strlen(x)+1];
  strcpy(searchRA,x);
}

void vobsREQUEST::SetSearchDEC(char *x)
{
  logExtDbg("vobsREQUEST::SetSearchDEC()\n");
  searchDEC=new char[strlen(x)+1];
  strcpy(searchDEC,x);
}

void vobsREQUEST::SetVisibility(char *x)
{
  logExtDbg("vobsREQUEST::SetVisibility()\n");
  visibility=new char[strlen(x)+1];
  strcpy(visibility,x);
}

void vobsREQUEST::SetReqAbsErr(char *f)
{
  logExtDbg("vobsREQUEST::SetReqAbsErr()\n");
  reqAbsErr=new char[strlen(f)+1];
  strcpy(reqAbsErr,f);
}

void vobsREQUEST::SetBand(char *c)
{
  logExtDbg("vobsREQUEST::SetBand()\n");
  band=new char[strlen(c)+1];
  strcpy(band,c);
}
/************************************************************/

char *vobsREQUEST::GetStarName()
{
  logExtDbg("vobsREQUEST::GetStarName()\n");

  return starName;
}

char *vobsREQUEST::GetWaveLength()
{
  logExtDbg("vobsREQUEST::GetWaveLength()\n");

  return wavelength;
}
char *vobsREQUEST::GetSoMagnitude()
{
  logExtDbg("vobsREQUEST::GetSoMagnitude()\n");

  return soMagnitude;
}
char *vobsREQUEST::GetMagnitudeRange()
{
  logExtDbg("vobsREQUEST::GetGetMagnitudeRange()\n");

  return magnitudeRange;
}
char *vobsREQUEST::GetSearchRA()
{
  logExtDbg("vobsREQUEST::GetSearchRA()\n");

  return searchRA;
}
char *vobsREQUEST::GetSearchDEC()
{
  logExtDbg("vobsREQUEST::GetSearchDEC()\n");

  return searchDEC;
}
char *vobsREQUEST::GetVisibility()
{
  logExtDbg("vobsREQUEST::GetVisibility()\n");

  return visibility;
}
char *vobsREQUEST::GetReqAbsErr()
{
  logExtDbg("vobsREQUEST::GetReqAbsErr()\n");

  return reqAbsErr;
}
char *vobsREQUEST::GetBand()
{
  logExtDbg("vobsREQUEST::GetBand()\n");

  return band;
}
int vobsREQUEST::Build()
{
    return build;
}

/**
 * Method to create the request according to the parameters
 */
void vobsREQUEST::Create(char *name, char *wvl ,char *sMg,char *mRg,char *sRa,char *sDEC,char *vis,char *rAerr, char*b)
{
  logExtDbg("vobsREQUEST::Create()\n");

  SetStarName(name);
  SetWaveLength(wvl);
  SetSoMagnitude(sMg);
  SetMagnitudeRange(mRg);
  SetSearchRA(sRa);
  SetSearchDEC(sDEC);
  SetVisibility(vis);
  SetReqAbsErr(rAerr);
  SetBand(b);
  build=1;
  
  logTest("star name : %s\n",starName);
  logTest("wavelength : %s\n",wavelength);
  logTest("soMagnitude : %s\n",soMagnitude);
  logTest("magnitudeRange : %s\n",magnitudeRange);
  logTest("searchRA : %s\n",searchRA);
  logTest("searchDEC : %s\n",searchDEC);
  logTest("visibility : %s\n",visibility);
  logTest("reqAbsErr : %s\n",reqAbsErr);
  logTest("band : %s\n",band);


  
}


/*___oOo___*/

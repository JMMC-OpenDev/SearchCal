/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.C,v 1.1 2004-07-13 13:41:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsCATALOG.C,v 1.1 2004-07-13 13:41:09 scetre Exp $"; 
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


/*
 * Local Variables
 */

#include"vobsCATALOG.h"

/*
 * Local Functions
 */
// Class constructor
vobsCATALOG::vobsCATALOG()
{
    req=NULL;
    _name=NULL;
}
 
vobsCATALOG::vobsCATALOG(char *x)
{
    SetCatalogName(x);
}

// Class destructor 
vobsCATALOG::~vobsCATALOG()
{
    /*delete[]req;
    delete[]_name;*/
}
/**
 * Return the name of one catalog
 * return name of catalog
 */
char *vobsCATALOG::GetCatalogName()
{
	logExtDbg("vobsCATALOG::GetCatalogName()\n");
	return _name;
}

/**
 * Set the catalog name
 */
void vobsCATALOG::SetCatalogName(char *x)
{
	logExtDbg("vobsCATALOG::SetCatalogName()\n");
	_name=x;
    //strcpy(_name,x);
}


// Method to get the request
/*char vobsCATALOG::GetRequest()
  {
  return req;
  }*/

/**
 * Prepare a first request
 */
char *vobsCATALOG::PrepareFirst(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::PrepareFirst()\n");
    char requete[10000];
    char *ptr_req=requete;
    strcpy(requete,"http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=");
    
    if ((strcmp(request.GetBand(),"K")==0)||(strcmp(request.GetBand(),"J")==0)||(strcmp(request.GetBand(),"H")==0))
    {
        // request on II/225/catalog
        if (strcmp(_name,"II/225/catalog")==0)
        {
            strcat(requete,_name);

            strcat(requete,"&-c.ra=");
            strcat(requete,request.GetSearchRA()); 

            strcat(requete,"&-c.dec=");
            strcat(requete,request.GetSearchDEC()); 

            strcat(requete,"&x_F(IR)=M");
            strcat(requete,"&F(IR)=");
            strcat(requete,request.GetMagnitudeRange()); 

            strcat(requete,"&lambda=");
            if (strcmp(request.GetBand(),"K")==0)
                strcat(requete,"2.20");
            else if (strcmp(request.GetBand(),"H")==0)
                strcat(requete,"1.65");
            else if (strcmp(request.GetBand(),"J")==0)
                strcat(requete,"1.25");
            else logWarning("Pb to define wavelength in II/225 according to the band");
            strcat(requete,"&-out.max=50&-c.bm=1800/300&-c.u=arcmin");

            strcat(requete,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=lambda&-out=F(IR)&-out=x_F(IR)&-sort=_r"); 

            //logTest("%s\n",ptr_req);
        }
        // request on II/7A/catalog
        else if (strcmp(_name,"II/7A/catalog")==0)
        {
            strcat(requete,_name);

            strcat(requete,"&-c.ra=");
            strcat(requete,request.GetSearchRA()); 

            strcat(requete,"&-c.dec=");
            strcat(requete,request.GetSearchDEC()); 

            strcat(requete,"&");
            strcat(requete,request.GetBand());
            strcat(requete,"=");
            strcat(requete,request.GetMagnitudeRange());
            strcat(requete,"&-out.max=50&-c.bm=1800/300&-c.u=arcmin");


            strcat(requete,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=*PHOT_JHN_R");
            strcat(requete,"&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J&-out=*PHOT_JHN_H&-out=*PHOT_JHN_K&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4&-sort=_r");
            logTest("%s\n",ptr_req);


        }
        else logWarning("no request to build in prepare first in K scenario\n");
    }
    else if (strcmp(request.GetBand(),"V")==0)
    {
        logExtDbg("V scenario\n");
        if (strcmp(_name,"I/280")==0)
        {
            strcat(requete,_name);
            strcat(requete,"&-c.ra=");
            strcat(requete,request.GetSearchRA()); 
            strcat(requete,"&-c.dec=");
            strcat(requete,request.GetSearchDEC());
            strcat(requete,"&");
            strcat(requete,request.GetBand());
            strcat(requete,"mag=");
            strcat(requete,request.GetMagnitudeRange());
            strcat(requete,"&-c.eq=J2000&-out.max=50&-c.bm=1800/300&-c.u=arcmin");
            strcat(requete,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(requete,"&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
            strcat(requete,"&-out=*POS_PARLX_TRIG&-out=*SPECT_TYPE_MK");
            strcat(requete,"&SpType=%5bOBAFGKM%5d*");
            strcat(requete,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
            strcat(requete,"&-out=v1&-out=d5");
            strcat(requete,"&-out=HIP&-out=HD&-out=DM&-sort=_r");
        }
        else
            logWarning("no request to build in prepare first in V scenario\n");
    }
    return ptr_req;

}

/**
 * Prepare a request
 */
char *vobsCATALOG::Prepare(vobsREQUEST request, char *listForKV)
{
	//logExtDbg("vobsCATALOG::Prepare()\n");
    char requete[100000];
    char *ptr_req=requete;
    strcpy(requete,"http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=");
    if ((strcmp(request.GetBand(),"K")==0)||(strcmp(request.GetBand(),"J")==0)||(strcmp(request.GetBand(),"H")==0))
    {
        // build of request for band K,J,H
        
        // request on II/225/catalog
        if (strcmp(_name,"II/225/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&x_F(IR)=M"); 
            strcat(ptr_req,"&-out.max=50");     
            strcat(ptr_req,"&lambda=1.25,1.65,2.20");
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-c.r=1&-c.u=arcmin");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=lambda&-out=F(IR)&-out=x_F(IR)&-sort=_r");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);    

            logTest("%s\n",ptr_req);
        }
        // request on II/7A/catalog
        else if (strcmp(_name,"II/7A/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(ptr_req,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=*PHOT_JHN_R&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J");
            strcat(ptr_req,"-out=*PHOT_JHN_H&-out=*PHOT_JHN_K&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);

        }
        // request on I/280
        else if (strcmp(_name,"I/280")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            //strcat(ptr_req,"%5bSpType=%5bOBAFGKM%5d*");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA&-out=*POS_PARLX_TRIG");
            strcat(ptr_req,"&-out=*SPECT_TYPE_MK&SpType=%5bOBAFGKM%5d*&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=v1&-out=d5&-out=HIP&-out=HD&-out=DM&-sort=_r");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);

        }
        // request on I/196/main
        else if (strcmp(_name,"I/196/main")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(ptr_req,"&-out=*POS_GAL_LAT&-out=*POS_GAL_LON&-out=*VELOC_HC");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on B/2mass/out ou II/246/out
        else if (strcmp(_name,"II/246/out")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=Jmag&-out=Hmag&-out=Kmag&-out=*POS_GAL_LAT&-out=*POS_GAL_LON");        
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on J/A+A/393/183/catalog
        else if (strcmp(_name,"J/A+A/393/183/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.add=RAJ2000,DEJ2000&-oc=hms&-out=Bmag,Vmag,Jmag,Hmag,Kmag&-out=UDDK,e_UDDK");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on J/A+A/386/492/charm
        else if (strcmp(_name,"J/A+A/386/492/charm")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out=*POS_EQ_RA_MAIN&-out=*POS_EQ_DEC_MAIN&-out=LD&-out=e_LD&-out=UD&-out=e_UD&-out=*OBS_METHOD");
            strcat(ptr_req,"&-out=*PHOT_JHN_K&Inst=,LBI,LO,SPE&-out=Lambda");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on V/50/catalog
        else if (strcmp(_name,"V/50/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*VELOC_ROTAT&-out=HD");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on V/36B/bsc4s
        else if (strcmp(_name,"V/36B/bsc4s")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*VELOC_ROTAT");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on B/denis
        else if (strcmp(_name,"B/denis")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=Kmag&-out=Jmag&-out=Imag");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        else logWarning("pas de catalogue correspondant\n");
    }
    // Build of request for band V
    else if (strcmp(request.GetBand(),"V")==0)
    {
        // request on I/196/main
        if (strcmp(_name,"I/196/main")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(ptr_req,"&-out=*POS_GAL_LAT&-out=*POS_GAL_LON&-out=*VELOC_HC&-out=HD");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on II/246/out
        else if (strcmp(_name,"II/246/out")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=Jmag&-out=Hmag&-out=Kmag&-out=*POS_GAL_LAT&-out=*POS_GAL_LON");        
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on II/225/catalog
        else if (strcmp(_name,"II/225/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(ptr_req,"&x_F(IR)=M");
            strcat(ptr_req,"&F(IR)=");
            strcat(ptr_req,request.GetMagnitudeRange());
            strcat(ptr_req,"&lambda=1.25,1.65,2.20");
            strcat(ptr_req,"&-out=lambda&-out=F(IR)&-out=x_F(IR)&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        /*else if (strcmp(_name,"II/224/cadars")==0)
        {
            
        }*/
        // request on J/A+A/393/183/catalog
        else if (strcmp(_name,"J/A+A/393/183/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.add=RAJ2000,DEJ2000&-oc=hms&-out=Bmag,Vmag,Jmag,Hmag,Kmag&-out=UDDK,e_UDDK");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on J/A+A/386/492/charm
        else if (strcmp(_name,"J/A+A/386/492/charm")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out=*POS_EQ_RA_MAIN&-out=*POS_EQ_DEC_MAIN&-out=LD&-out=e_LD&-out=UD&-out=e_UD&-out=*OBS_METHOD");
            strcat(ptr_req,"&-out=*PHOT_JHN_K&Inst=,LBI,LO,SPE&-out=Lambda");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on II/7A/catalog
        else if (strcmp(_name,"II/7A/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
            strcat(ptr_req,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=*PHOT_JHN_R&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J");
            strcat(ptr_req,"-out=*PHOT_JHN_H&-out=*PHOT_JHN_K&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on V/50/catalog
        else if (strcmp(_name,"V/50/catalog")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*VELOC_ROTAT&-out=HD");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on V/36B/bsc4s
        else if (strcmp(_name,"V/36B/bsc4s")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*VELOC_ROTAT");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        // request on B/denis
        else if (strcmp(_name,"B/denis")==0)
        {
            strcat(ptr_req,_name);
            strcat(ptr_req,"&-file=-c&-c.eq=J2000&-c.r=1&-c.u=arcmin"); 
            strcat(ptr_req,"&-out.max=50");
            strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=Kmag&-out=Jmag&-out=Imag");
            strcat(ptr_req,"&-out.form=List");
            strcat(ptr_req,listForKV);
            logTest("%s\n",ptr_req);
        }
        else logWarning("No catalog\n");
    }
    return ptr_req;
    
    
	// we wrote the request as a char in order to be send to the CDS
	

}

/**
 * Search begin the research of information in the CDS
 */
std::list<vobsCALIBRATOR_STAR> vobsCATALOG::Search(vobsREQUEST request, char *catName, int kindOfReq, char *listForKV)
{
	logExtDbg("vobsCATALOG::Search()\n");

	logInfo("-------------------recherche depuis catalog----------------\n \n");
	//  we write the good char request and we create the connexion with the CDS
	// char *f;
    
    SetCatalogName(catName);
    
    if (request.Build()==1) // test if the request is build
    {
        if (kindOfReq==1)
        {
            req=PrepareFirst(request);
            logTest("requete de PrepareFirst : \n \t%s\n",req);
        }
        else 
        {
            req=Prepare(request,listForKV);
            logTest("requete de Prepare : \n \t%s\n",req);
        }
    }
    else logWarning("l'objet requete n'est pas encore construit\n");
    
    if (req!=NULL)
    {
        parser.MainParser(req);
        listOfStar=parser.GetList();
    }
    /*if (listOfStar.size()!=0)
    {
        std::list<vobsCALIBRATOR_STAR>::iterator Q=listOfStar.begin();
        while (Q!=listOfStar.end())
        {
            (*Q).View();
            ++Q;
        }
    }*/
    //vobsCALIBRATOR_STAR_LIST L;
	return listOfStar;
}
std::list<vobsCALIBRATOR_STAR> vobsCATALOG::SearchOne(vobsREQUEST request, int requestNumber, char *listOne)
{
    char requete[10000];
    char *ptr_req=requete;
    strcpy(requete,"http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=");
    if (requestNumber==1)
    {
        strcat(requete,"II/225/catalog");
        strcat(requete,"&name=");
        strcat(requete,request.GetStarName());
        strcat(requete,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=lambda&-out=F(IR)&-out=x_F(IR)&-sort=_r");
        
        parser.MainParser(ptr_req);
        listOfStar=parser.GetList();
        return listOfStar;
    }
    else if (requestNumber==2)
    {
        strcat(requete,"II/7A/catalog");
        strcat(ptr_req,"&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
        strcat(ptr_req,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=*PHOT_JHN_R&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J");
        strcat(ptr_req,"-out=*PHOT_JHN_H&-out=*PHOT_JHN_K&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
        
        strcat(ptr_req,listOne);
        parser.MainParser(ptr_req);
        listOfStar=parser.GetList();
        return listOfStar;    
    }
	return listOfStar;

}
/*___oOo___*/

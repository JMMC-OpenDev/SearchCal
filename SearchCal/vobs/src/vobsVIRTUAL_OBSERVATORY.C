/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.5 2004-07-28 14:19:41 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.5 2004-07-28 14:19:41 scetre Exp $"; 
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
#include "err.h"
/*
 * Local Headers 
 */

#include "vobs.h"
//#include "vobsVIRTUAL_OBSERVATORY.h"


/* 
 * Signal catching functions  
 */


/* 
 *Local functions  
 */

// Class constructor 
vobsVIRTUAL_OBSERVATORY::vobsVIRTUAL_OBSERVATORY()
{
    listForK=NULL;
    listForV=NULL;

}

// Class destructor 
vobsVIRTUAL_OBSERVATORY::~vobsVIRTUAL_OBSERVATORY()
{

}
/*
 * Return a catalog
 * \return catalog
 */
/*vobsCATALOG *vobsVIRTUAL_OBSERVATORY::GetCatalog()
  {
  logExtDbg("vobsVIRTUAL_OBSERVATORY::GetCatalog()\n");

  return &catalog;
  }*/

/*
 * Return a calibrator star list
 * \return calibrator star list
 */ 

vobsSTAR_LIST vobsVIRTUAL_OBSERVATORY::GetCalibStarList()
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::GetCalibStarList()\n");

    return StarList;
}

/**
 * Compare the coordonate in Ra and Dec and return 1 if they are equal, else
 * return 0
 * \return 1 if succes, 0 else
 */
int vobsVIRTUAL_OBSERVATORY::CompareRaDec(char *ra1, char *dec1, char *ra2, char *dec2, float raInterval, float decInterval)
{
  logExtDbg("vobsCATALOG::CompareRaDec()\n");
    float ra1_sec = 0;
  float dec1_sec = 0;
  float ra2_sec = 0;
  float dec2_sec = 0;
  int i = 1;
  char *ch;
  char ch1[50];
    logTest("\tra1 : %s\n",ra1);
    logTest("\tdec1 : %s\n",dec1);
    logTest("\tra2 : %s\n",ra2);
    logTest("\tdec2 : %s\n\n",dec2);
    

  strcpy (ch1, ra1);
  ch = (char *) strtok (ch1, " ");
  while (ch != NULL)
    {
      if (i == 1)
	ra1_sec = atof (ch) * 3600;
      if (i == 2)
	ra1_sec = ra1_sec + atof (ch) * 60;
      if (i == 3)
	ra1_sec = ra1_sec + atof (ch);
      i++;
      ch = (char *) strtok (NULL, " ");
    }
  i = 1;
  strcpy (ch1, ra2);
  ch = (char *) strtok (ch1, " ");
  while (ch != NULL)
    {
      if (i == 1)
	ra2_sec = atof (ch) * 3600;
      if (i == 2)
	ra2_sec = ra2_sec + atof (ch) * 60;
      if (i == 3)
	ra2_sec = ra2_sec + atof (ch);
      i++;
      ch = (char *) strtok (NULL, " ");
    }
  i = 1;
  strcpy (ch1, dec1);
  ch = (char *) strtok (ch1, " ");
  while (ch != NULL)
    {
      if (i == 1)
	dec1_sec = atof (ch) * 3600;
      if (i == 2)
	dec1_sec = dec1_sec + atof (ch) * 60;
      if (i == 3)
	dec1_sec = dec1_sec + atof (ch);
      i++;
      ch = (char *) strtok (NULL, " ");
    }
  i = 1;
  strcpy (ch1, dec2);
  ch = (char *) strtok (ch1, " ");
  while (ch != NULL)
    {
      if (i == 1)
	dec2_sec = atof (ch) * 3600;
      if (i == 2)
	dec2_sec = dec2_sec + atof (ch) * 60;
      if (i == 3)
	dec2_sec = dec2_sec + atof (ch);
      i++;
      ch = (char *) strtok (NULL, " ");
    }
  if ((fabs (ra1_sec - ra2_sec) <= raInterval) && (fabs (dec1_sec - dec2_sec) <= decInterval))
  {
      logTest("\t\tegal\n\n");
      return 1;
  }
  else
  {
      logTest("\t\tnon egal\n\n");      
      return 0;
  }
}

/**
 * Update in one list all the element of an other list that are not in the
 * list
 */
void vobsVIRTUAL_OBSERVATORY::UpdateCalibStar(vobsSTAR &calibStar1, vobsSTAR &calibStar2)
{
    logExtDbg("vobsCATALOG::UpdateCalibStar()\n");
    if ((strcmp(calibStar2.Gethip(),"99.99")!=0)&&((calibStar1.Gethip()==NULL)||(strcmp(calibStar1.Gethip(),"99.99")==0)))
    {
        calibStar1.Sethip(calibStar2.Gethip());
    }
    if ((strcmp(calibStar2.Gethd(),"99.99")!=0)&&((calibStar1.Gethd()==NULL)||(strcmp(calibStar1.Gethd(),"99.99")==0)))
    {
        calibStar1.Sethd(calibStar2.Gethd());
    }
    if ((strcmp(calibStar2.GetpmDE(),"99.99")!=0)&&((calibStar1.GetpmDE()==NULL)||(strcmp(calibStar1.GetpmDE(),"99.99")==0)))
    {
        calibStar1.SetpmDE(calibStar2.GetpmDE());
    }
    if ((strcmp(calibStar2.GetpmRA(),"99.99")!=0)&&((calibStar1.GetpmRA()==NULL)||(strcmp(calibStar1.GetpmRA(),"99.99")==0)))
    {
        calibStar1.SetpmRA(calibStar2.GetpmRA());
    }
    if ((strcmp(calibStar2.Getplx(),"99.99")!=0)&&((calibStar1.Getplx()==NULL)||(strcmp(calibStar1.Getplx(),"99.99")==0)))
    {
        calibStar1.Setplx(calibStar2.Getplx());
    }
    if ((strcmp(calibStar2.GetSpType(),"99.99")!=0)&&((calibStar1.GetSpType()==NULL)||(strcmp(calibStar1.GetSpType(),"99.99")==0)))
    {
        calibStar1.SetSpType(calibStar2.GetSpType());
    }
    if ((strcmp(calibStar2.GetvFlag(),"99.99")!=0)&&((calibStar1.GetvFlag()==NULL)||(strcmp(calibStar1.GetvFlag(),"99.99")==0)))
    {
        calibStar1.SetvFlag(calibStar2.GetvFlag());
    }
    if ((strcmp(calibStar2.GetmFlag(),"99.99")!=0)&&((calibStar1.GetmFlag()==NULL)||(strcmp(calibStar1.GetmFlag(),"99.99")==0)))
    {
        calibStar1.SetmFlag(calibStar2.GetmFlag());
    }
    if ((strcmp(calibStar2.GetgLat(),"99.99")!=0)&&((calibStar1.GetgLat()==NULL)||(strcmp(calibStar1.GetgLat(),"99.99")==0)))
    {
        calibStar1.SetgLat(calibStar2.GetgLat());
    }
    if ((strcmp(calibStar2.GetgLon(),"99.99")!=0)&&((calibStar1.GetgLon()==NULL)||(strcmp(calibStar1.GetgLon(),"99.99")==0)))
    {
        calibStar1.SetgLon(calibStar2.GetgLon());
    }
    if ((strcmp(calibStar2.GetradVel(),"99.99")!=0)&&((calibStar1.GetradVel()==NULL)||(strcmp(calibStar1.GetradVel(),"99.99")==0)))
    {
        calibStar1.SetradVel(calibStar2.GetradVel());
    }
    if ((strcmp(calibStar2.Getld(),"99.99")!=0)&&((calibStar1.Getld()==NULL)||(strcmp(calibStar1.Getld(),"99.99")==0)))
    {
        calibStar1.Setld(calibStar2.Getld());
    }
    if ((strcmp(calibStar2.Gete_ld(),"99.99")!=0)&&((calibStar1.Gete_ld()==NULL)||(strcmp(calibStar1.Gete_ld(),"99.99")==0)))
    {
        calibStar1.Sete_ld(calibStar2.Gete_ld());
    }
    if ((strcmp(calibStar2.Getud(),"99.99")!=0)&&((calibStar1.Getud()==NULL)||(strcmp(calibStar1.Getud(),"99.99")==0)))
    {
        calibStar1.Setud(calibStar2.Getud());
    }
    if ((strcmp(calibStar2.Gete_ud(),"99.99")!=0)&&((calibStar1.Gete_ud()==NULL)||(strcmp(calibStar1.Gete_ud(),"99.99")==0)))
    {
        calibStar1.Sete_ud(calibStar2.Gete_ud());
    }
    if ((strcmp(calibStar2.Getmeth(),"99.99")!=0)&&((calibStar1.Getmeth()==NULL)||(strcmp(calibStar1.Getmeth(),"99.99")==0)))
    {
        calibStar1.Setmeth(calibStar2.Getmeth());
    }
    if ((strcmp(calibStar2.Getlambda(),"99.99")!=0)&&((calibStar1.Getlambda()==NULL)||(strcmp(calibStar1.Getlambda(),"99.99")==0)))
    {
        calibStar1.Setlambda(calibStar2.Getlambda());
    }
    if ((strcmp(calibStar2.Getuddk(),"99.99")!=0)&&((calibStar1.Getuddk()==NULL)||(strcmp(calibStar1.Getuddk(),"99.99")==0)))
    {
        calibStar1.Setuddk(calibStar2.Getuddk());
    }
    if ((strcmp(calibStar2.Gete_uddk(),"99.99")!=0)&&((calibStar1.Gete_uddk()==NULL)||(strcmp(calibStar1.Gete_uddk(),"99.99")==0)))
    {
        calibStar1.Sete_uddk(calibStar2.Gete_uddk());
    }
    if ((strcmp(calibStar2.GetU(),"99.99")!=0)&&((calibStar1.GetU()==NULL)||(strcmp(calibStar1.GetU(),"99.99")==0)))
    {
        calibStar1.SetU(calibStar2.GetU());
    }
    if ((strcmp(calibStar2.GetB(),"99.99")!=0)&&((calibStar1.GetB()==NULL)||(strcmp(calibStar1.GetB(),"99.99")==0)))
    {
        calibStar1.SetB(calibStar2.GetB());
    }
    if ((strcmp(calibStar2.GetV(),"99.99")!=0)&&((calibStar1.GetV()==NULL)||(strcmp(calibStar1.GetV(),"99.99")==0)))
    {
        calibStar1.SetV(calibStar2.GetV());
    }
    if ((strcmp(calibStar2.GetR(),"99.99")!=0)&&((calibStar1.GetR()==NULL)||(strcmp(calibStar1.GetR(),"99.99")==0)))
    {
        calibStar1.SetR(calibStar2.GetR());
    }
    if ((strcmp(calibStar2.GetI(),"99.99")!=0)&&((calibStar1.GetI()==NULL)||(strcmp(calibStar1.GetI(),"99.99")==0)))
    {
        calibStar1.SetI(calibStar2.GetI());
    }
    if ((strcmp(calibStar2.GetJ(),"99.99")!=0)&&((calibStar1.GetJ()==NULL)||(strcmp(calibStar1.GetJ(),"99.99")==0)))
    {
        calibStar1.SetJ(calibStar2.GetJ());
    }
    if ((strcmp(calibStar2.GetH(),"99.99")!=0)&&((calibStar1.GetH()==NULL)||(strcmp(calibStar1.GetH(),"99.99")==0)))
    {
        calibStar1.SetH(calibStar2.GetH());
    }
    if ((strcmp(calibStar2.GetK(),"99.99")!=0)&&((calibStar1.GetK()==NULL)||(strcmp(calibStar1.GetK(),"99.99")==0)))
    {
        calibStar1.SetK(calibStar2.GetK());
    }
    if ((strcmp(calibStar2.GetL(),"99.99")!=0)&&((calibStar1.GetL()==NULL)||(strcmp(calibStar1.GetL(),"99.99")==0)))
    {
        calibStar1.SetL(calibStar2.GetL());
    }
    if ((strcmp(calibStar2.GetM(),"99.99")!=0)&&((calibStar1.GetM()==NULL)||(strcmp(calibStar1.GetM(),"99.99")==0)))
    {
        calibStar1.SetM(calibStar2.GetM());
    }
    if ((strcmp(calibStar2.GetN(),"99.99")!=0)&&((calibStar1.GetN()==NULL)||(strcmp(calibStar1.GetN(),"99.99")==0)))
    {
        calibStar1.SetN(calibStar2.GetN());
    }
    if ((strcmp(calibStar2.GetrotVel(),"99.99")!=0)&&((calibStar1.GetrotVel()==NULL)||(strcmp(calibStar1.GetrotVel(),"99.99")==0)))
    {
        calibStar1.SetrotVel(calibStar2.GetrotVel());
    }
    if ((strcmp(calibStar2.Gete_B_V(),"99.99")!=0)&&((calibStar1.Gete_B_V()==NULL)||(strcmp(calibStar1.Gete_B_V(),"99.99")==0)))
    {
        calibStar1.Sete_B_V(calibStar2.Gete_B_V());
    }
}

/**
 * Merge two list of calibratorStar
 */
void vobsVIRTUAL_OBSERVATORY::Merge(std::list<vobsSTAR>list2)
{
    logExtDbg("vobsCATALOG::Merge()\n");
    std::list<vobsSTAR>::iterator P=list.begin();
    std::list<vobsSTAR>::iterator Q;
    std::list<vobsSTAR>::iterator R;
    logTest("taille de la liste : %d\n",list.size());
    logTest("taille de la list2 avant merge : %d\n",list2.size());
    
    while (P!=list.end())
    {
        Q=list2.begin();
        while (Q!=list2.end())
        {
            if (CompareRaDec( (*P).Getraj2000(), (*P).Getdej2000(), (*Q).Getraj2000(), (*Q).Getdej2000(), 0.1, 0.1)==1)
            {
                UpdateCalibStar(*P,*Q); 
                R=Q;
                ++Q;
                list2.erase(R);
            }
            else 
            {
                list.push_back(*Q);
                R=Q;
                ++Q;
                list2.erase(R);                
            }
        }
        ++P;
    }
    logTest("taille de la liste apres merge : %d\n",list.size());
    logTest("taille de la list2 apres merge : %d\n",list2.size());
}
/**
 * Read a calibrator Star List and update and remove element
 */
std::list<vobsSTAR> vobsVIRTUAL_OBSERVATORY::FillAndErase( std::list<vobsSTAR>SList)
{
    std::list<vobsSTAR> listEnd;
    listEnd.push_back(*SList.begin());
    
    if ((SList.size()==1)||(SList.size()==0))
        return listEnd;
    else
    {
        std::list<vobsSTAR>::iterator Q=SList.begin();        
        ++Q;
        
        //listEnd.push_back(*SList.begin());
        std::list<vobsSTAR>::iterator P=listEnd.begin();

        while(Q!=SList.end())
        {
            if (CompareRaDec( (*P).Getraj2000(), (*P).Getdej2000(), (*Q).Getraj2000(), (*Q).Getdej2000(), 0.1, 0.1)==1)
            {
                UpdateCalibStar(*P,*Q); 
                ++Q;
            }
            else ++Q;
        } 
        return listEnd;
    }
}


/**
 * Load a scenarion according to the band of the request
 * \return list of catalog name
 */
void vobsVIRTUAL_OBSERVATORY::LoadScenario(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG::LoadScenario()\n");
    
    if ((strcmp(request.GetBand(),"K")==0)||(strcmp(request.GetBand(),"J")==0)||(strcmp(request.GetBand(),"H")==0))
    {
        // 1ere interrogation de II/225/catalog -> liste L1
        vobsCATALOG cat1;
        std::list<vobsSTAR>list1=cat1.Search(request,"II/225/catalog",1,listForK); 
        
        // 1ere interrogation de II/7A/catalog -> liste L2
        vobsCATALOG cat2;
        std::list<vobsSTAR>list2=cat2.Search(request,"II/7A/catalog",1,listForK);
        
        // concatenation L1 et L2 -> L
        std::list<vobsSTAR>::iterator It=list2.begin();
        while (It!=list2.end())
        {
            list1.push_back(*It);
            ++It;
        }
               
        // creation de la liste de coordonnees
        std::list<char *>coordRaList;
        std::list<char *>coordDeList; 
        char *tmp1;
        char *tmp2;
        std::list<vobsSTAR>::iterator Q=list1.begin();
        printf("taille de la liste %d\n",list1.size());
        while (Q!=list1.end())
        {
            tmp1=new char[strlen((*Q).Getraj2000())+1];
            tmp2=new char[strlen((*Q).Getdej2000())+1];
            strcpy(tmp1,(*Q).Getraj2000());
            strcpy(tmp2,(*Q).Getdej2000());

            coordRaList.push_back(tmp1);
            coordDeList.push_back(tmp2);
            ++Q;
        }

        char listK[50000];
        listForK=listK;
        strcpy(listK,"");
        char *tmp;
        std::list<char *>::iterator I=coordRaList.begin();
        std::list<char *>::iterator J=coordDeList.begin();
        
        while ((I!=coordRaList.end())&&(J!=coordDeList.end())&&(coordRaList.size()!=0)&&(coordDeList.size()!=0))
        {
            if (I==coordRaList.begin())
                strcat(listK,"&-c=");
            else
                strcat(listK,"&+");
            tmp = (char *)strtok(*I," ");
            while (tmp!=NULL)
            {
                strcat(listK,tmp);
                tmp=(char *) strtok(NULL," ");
                if (tmp!=NULL)
                    strcat(listK,"+");
            }
            ++I;
            tmp = (char *)strtok(*J," ");
            while (tmp!=NULL)
            {
                if(tmp[0]=='+')
                {
                    strcat(listK,"+%2b");
                    strcat(listK, &tmp[1]);
                }
                else
                    strcat(listK,tmp);
                tmp=(char *) strtok(NULL," ");
                if (tmp!=NULL)
                    strcat(listK,"+");
            }
            ++J;
        }
        
        logTest("%s\n",listForK);
        if (strcmp(listK,"")!=0)
        {
            // interrogation de I/280 avec liste de coord -> liste L3
            vobsCATALOG cat3; 
            list=cat3.Search(request,"I/280",2,listForK);
            //Merge(list3);
            // merge L3 et L -> L

            // creation de la liste de coordonnees
                        
            // interrogation de II/225/catalog
            vobsCATALOG cat4; 
            std::list<vobsSTAR>list4=cat4.Search(request,"II/225/catalog",2,listForK);
            Merge(list4);

            // interrogation de I/196/main
            vobsCATALOG cat5; 
            std::list<vobsSTAR>list5=cat5.Search(request,"I/196/main",2,listForK);
            Merge(list5);

            // interrogation de B/2mass/out
            vobsCATALOG cat6; 
            std::list<vobsSTAR>list6=cat6.Search(request,"II/246/out",2,listForK);
            Merge(list6);

            // interrogation de J/A+A/393/183/catalog
            vobsCATALOG cat7; 
            std::list<vobsSTAR>list7=cat7.Search(request,"J/A+A/393/183/catalog",2,listForK);
            Merge(list7);

            // interrogation de J/A+A/386/492/charm
            vobsCATALOG cat8; 
            std::list<vobsSTAR>list8=cat8.Search(request,"J/A+A/386/492/charm",2,listForK);
            Merge(list8);

            // interrogation de II/7A/catalog
            vobsCATALOG cat9; 
            std::list<vobsSTAR>list9=cat9.Search(request,"II/7A/catalog",2,listForK);
            Merge(list9);

            // interrogation de V/50/catalog
            vobsCATALOG cat10; 
            std::list<vobsSTAR>list10=cat10.Search(request,"V/50/catalog",2,listForK);     
            Merge(list10);
            
            // interrogation de V/36B/bsc4s
            vobsCATALOG cat11; 
            std::list<vobsSTAR>list11=cat11.Search(request,"V/36B/bsc4s",2,listForK);
            Merge(list11);

            // interrogation de B/denis
            vobsCATALOG cat12; 
            std::list<vobsSTAR>list12=cat12.Search(request,"B/denis",2,listForK);
            Merge(list12);

        }
        else logWarning("no result\n");
    }
    else if (strcmp(request.GetBand(),"V")==0)
    {
        // 1ere interrogation de II/280 -> liste L1
        vobsCATALOG cat1;
        list=cat1.Search(request,"I/280",1,listForV);
               
        // creation de la liste de coordonnees
        std::list<char *>coordRaList;
        std::list<char *>coordDeList;
        char *tmp1;
        char *tmp2;
        std::list<vobsSTAR>::iterator Q=list.begin();
        while (Q!=list.end())
        {
            tmp1=new char[strlen((*Q).Getraj2000())+1];
            tmp2=new char[strlen((*Q).Getdej2000())+1];
            strcpy(tmp1,(*Q).Getraj2000());
            strcpy(tmp2,(*Q).Getdej2000());

            coordRaList.push_back(tmp1);
            coordDeList.push_back(tmp2);
            ++Q;
        }

        char listV[50000];
        listForV=listV;
        strcpy(listV,"");
        char *tmp;
        std::list<char *>::iterator I=coordRaList.begin();
        std::list<char *>::iterator J=coordDeList.begin();

        while ((I!=coordRaList.end())&&(J!=coordDeList.end())&&(coordRaList.size()!=0)&&(coordDeList.size()!=0))
        {
            if (I==coordRaList.begin())
                strcat(listV,"&-c=");
            else
                strcat(listV,"&+");
            tmp = (char *)strtok((*I)," ");

            while (tmp!=NULL)
            {
                strcat(listV,tmp);
                tmp=(char *) strtok(NULL," ");
                if (tmp!=NULL)
                    strcat(listV,"+");
            }
            ++I;
            tmp = (char *)strtok(*J," ");
            while (tmp!=NULL)
            {
                if(tmp[0]=='+')
                {
                    strcat(listV,"+%2b");
                    strcat(listV, &tmp[1]);
                }
                else
                    strcat(listV,tmp);
                tmp=(char *) strtok(NULL," ");
                if (tmp!=NULL)
                    strcat(listV,"+");
            }
            ++J;
        }
        logTest("%s\n",listForV);
        
        if (strcmp(listV,"")!=0)
        {
            

            // interrogation of
            vobsCATALOG cat2; 
            std::list<vobsSTAR>list2=cat2.Search(request,"I/196/main",2,listForV);
            Merge(list2);
            
            // interrogation of
            vobsCATALOG cat3; 
            std::list<vobsSTAR>list3=cat3.Search(request,"II/246/out",2,listForV);
            Merge(list3);
            
            // interrogation of
            vobsCATALOG cat4; 
            std::list<vobsSTAR>list4=cat4.Search(request,"II/225/catalog",2,listForV);
            Merge(list4);
            
            // interrogation of
            vobsCATALOG cat5; 
            std::list<vobsSTAR>list5=cat5.Search(request,"J/A+A/393/183/catalog",2,listForV);
            Merge(list5);
            
            // interrogation of
            vobsCATALOG cat6; 
            std::list<vobsSTAR>list6=cat6.Search(request,"J/A+A/386/492/charm",2,listForV);
            Merge(list6);
            
            // interrogation of
            vobsCATALOG cat7; 
            std::list<vobsSTAR>list7=cat7.Search(request,"II/7A/catalog",2,listForV);
            Merge(list7);
            
            // interrogation of
            vobsCATALOG cat8; 
            std::list<vobsSTAR>list8=cat8.Search(request,"V/50/catalog",2,listForV);
            Merge(list8);
            
            // interrogation of
            vobsCATALOG cat9; 
            std::list<vobsSTAR>list9=cat9.Search(request,"V/36B/bsc4s",2,listForV);
            Merge(list9);
            
            // interrogation of
            vobsCATALOG cat10; 
            std::list<vobsSTAR>list10=cat10.Search(request,"B/denis",2,listForV);
            Merge(list10);
               
        }
    }
    else logWarning("No catalog to search\n");
}

/*
 * Return a list of calibrator after a call to the cds according to the
 * parameters of the users which are use in the request.
 * \return calibrator star list
 */
vobsSTAR_LIST vobsVIRTUAL_OBSERVATORY::Research(vobsREQUEST request)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::Research()\n");

    // here we will choose in which catalog we search the data 
    // according to the parameters we have in the request and the science star
    logInfo("-------The Observatory begin the research in the catalog------- \n\n");

    // we load the scenario
    logInfo("avant LoadScenario\n");
    LoadScenario(request);
    logInfo("apres LoadScenario\n");

    FillAndErase(list);
    /*std::list<vobsSTAR>::iterator Q=list.begin();
    while (Q!=list.end())
    {
        (*Q).View();
        ++Q;
    }*/

    // here we take the data the catalog class give to us
    logInfo("--------- return of the list gave by the catalog -----------------\n \n");

    StarList.Set(list);
    // at the end, we return the list
    return StarList;
}
/**
 * Return a list of calibrator after a call to the cds according to the
 * parameters of the users which are use in the request. The use of this
 * function is for the research of only one star.
 * \return calibrator star list
 */
vobsSTAR_LIST vobsVIRTUAL_OBSERVATORY::ResearchOne(vobsREQUEST request)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::ResearchOne()\n");

    vobsCATALOG cat1;
    std::list<vobsSTAR>list1=cat1.SearchOne(request,1,listOne);

    // creation de la liste de coordonnees
    std::list<char *>coordRaList;
    std::list<char *>coordDeList; 
    char *tmp1;
    char *tmp2;
    std::list<vobsSTAR>::iterator Q=list1.begin();
    while (Q!=list1.end())
    {
        tmp1=new char[strlen((*Q).Getraj2000())+1];
        tmp2=new char[strlen((*Q).Getdej2000())+1];
        strcpy(tmp1,(*Q).Getraj2000());
        strcpy(tmp2,(*Q).Getdej2000());

        coordRaList.push_back(tmp1);
        coordDeList.push_back(tmp2);
        ++Q;
    }

    char listO[50000];
    listOne=listO;
    strcpy(listO,"");
    char *tmp;
    std::list<char *>::iterator I=coordRaList.begin();
    std::list<char *>::iterator J=coordDeList.begin();

    while ((I!=coordRaList.end())&&(J!=coordDeList.end())&&(coordRaList.size()!=0)&&(coordDeList.size()!=0))
    {
        if (I==coordRaList.begin())
            strcat(listO,"&-c=");
        else
            strcat(listO,"&+");
        tmp = (char *)strtok(*I," ");
        while (tmp!=NULL)
        {
            strcat(listO,tmp);
            tmp=(char *) strtok(NULL," ");
            if (tmp!=NULL)
                strcat(listO,"+");
        }
        ++I;
        tmp = (char *)strtok(*J," ");
        while (tmp!=NULL)
        {
            if(tmp[0]=='+')
            {
                strcat(listO,"+%2b");
                strcat(listO, &tmp[1]);
            }
            else
                strcat(listO,tmp);
            tmp=(char *) strtok(NULL," ");
            if (tmp!=NULL)
                strcat(listO,"+");
        }
        ++J;
    }
    printf("%s\n",listOne);
    vobsCATALOG cat2;
    std::list<vobsSTAR>list2=cat2.SearchOne(request,2,listOne);
    std::list<vobsSTAR>list3=FillAndErase(list2);
    
    //list=list3;
    StarList.Set(list3);
    
    return StarList;

}
/*___oOo___*/

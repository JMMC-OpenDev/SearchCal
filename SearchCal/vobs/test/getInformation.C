
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: getInformation.C,v 1.5 2004-11-24 14:39:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    25-Aug-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: getInformation.C,v 1.5 2004-11-24 14:39:09 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


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
#include "vobs.h"
#include "vobsPrivate.h"

extern "C"{
#include "simcli.h"
}
/* 
 * Main
 */

int main(int argc, char *argv[])
{
        // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }
    miscDYN_BUF starName;
    if (argv[1]!=NULL)
    {
        miscDynBufAppendString(&starName, argv[1]);
        int i=2;
        while (argv[i]!=NULL)
        {
            miscDynBufAppendString(&starName, "%20");
            miscDynBufAppendString(&starName, argv[i]);
            i++;
        }
        //printf("%s\n",miscDynBufGetBufferPointer(&starName));
    }
    else 
    {
        printf("NOT VALID NAME\n");
    }

    mcsSTRING32 ra, dec;

    char /*hostname[256], service[8], */
        userid[MAX_USERID+1]={"t039ga"}, 
    passwd[MAX_USERID+1]={"codeporte"} ;
    char rec[256] ;
    char *p;
    char* TheData;
    int hh ;
    int HH=0,MM=0;
    float SS=0;
    double ss;
    double mv;
    int DH=0, DM=0;
    float DS=0;
    double ds;
    char sign=' ';
    int nitems, ndata,i ;

    hh = simbad_connect(NULL,NULL,userid,passwd) ;
    if (hh < 0)
    {
        exit(1) ;
    }
    if (argc==1) 
    {
        exit(0);
    }
    *rec=0;
    if (argc==2)
    {
        strcat(rec,argv[1]);
        p=strchr(rec,'_');
        while ( p !=NULL) 
        {
            *p = ' ';
            p=strchr(rec,'_');  
        }
    }
    else
    {
        for (i=1; i<argc-1; i++) 
        {
            strcat(rec,argv[i]); 
            strcat(rec," ");
        }
        strcat(rec,argv[i]);
    }
    if (rec[0] == '\0')
    {
        exit(0);
    }
    strcat(rec,"!") ; 
    nitems = simbad_query(hh,rec,"") ; /* query the object */
    *rec=0;
    for (i=1; i<argc-1; i++)
    {
        strcat(rec,argv[i]);
        strcat(rec,"_");
    } 
    strcat(rec,argv[i]);   
    if (nitems >= 1)
    {
        simbad_retrieve(hh,0) ;  /* retrieve 1st object */
        ndata = simbad_findata(hh,"J","") ;
        if (ndata == 1) 
        {
            TheData=simbad_getdata(hh,1);
            sscanf (strtok(TheData," "), "%lf", &ss);
            sscanf (strtok(NULL," "), "%lf", &ds);
            ss=ss/15.0;
            HH=(int)ss;
            MM=(int)((ss-HH)*60.0);
            SS=(ss-HH)*3600-(MM*60);
            sign=(ds<0.0)?'-':'+'; 
            ds = (ds<0.0)?-ds:ds;
            DH=(int)ds;
            DM=(int)((ds-DH)*60.0);
            DS=(ds-DH)*3600-(DM*60);
        }
        /*printf("%s 2000.0 %d:%02d:%07.4f %c%d:%02d:%06.3f ",rec,HH,MM,SS,sign,DH,DM,DS);*/

        sprintf(ra, "%02d %02d %07.4f", HH, MM, SS );
        sprintf(dec,"%c%d %02d %06.3f",sign,DH,DM,DS); 
        /* 	printf("%s\n",simbad_telldata(hh)); */
        /*  	ndata = simbad_findata(hh,"C","") ; */
        /* 	printf("%d\n",ndata); */
        /* 	for (i=0; i<ndata; i++) { */
        /* 	  TheData=simbad_getdata(hh,1); */
        /* 	  printf("%s\n",TheData); */
        /* 	} */
        /*  	ndata = simbad_findata(hh,"0.JP11","") ; */
        /* 	for (i=0; i<ndata; i++) { */
        /* 	  TheData=simbad_getdata(hh,1); */
        /* 	  printf("%s\n",TheData); */
        /* 	} */
        ndata = simbad_findata(hh,"M.V","") ;
        if (ndata == 1)
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":"))
            {
                sscanf (TheData, "%lf", &mv);
            }
            else
            {
                sscanf (TheData, "%lf:", &mv);
            }
        } 
        else
        {
            mv=99.99;
        }
        /*printf("%6.3f ",mv);*/
        ndata = simbad_findata(hh,"M.R","") ;
        if (ndata == 1)
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":"))
            {
                sscanf (TheData, "%lf", &mv);
            }
            else 
            {
                sscanf (TheData, "%lf:", &mv);
            }
        } 
        else 
        {
            mv=99.99;
        }
        /*printf("%6.3f ",mv);*/
        ndata = simbad_findata(hh,"M.I","") ;
        if (ndata == 1) 
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":"))
            {
                sscanf (TheData, "%lf", &mv);
            }
            else
            {
                sscanf (TheData, "%lf:", &mv);
            }
        } 
        else
        {
            mv=99.99;
        }
        /*printf("%6.3f ",mv);*/
        ndata = simbad_findata(hh,"M.J","") ;
        if (ndata == 1) 
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":")) 
            {
                sscanf (TheData, "%lf", &mv);
            }
            else 
                sscanf (TheData, "%lf:", &mv);
        } 
        else
        {
            mv=99.99;
        }
        /*printf("%6.3f ",mv);*/
        ndata = simbad_findata(hh,"M.H","") ;
        if (ndata == 1) 
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":"))
            {
                sscanf (TheData, "%lf", &mv);
            }
            else 
            {
                sscanf (TheData, "%lf:", &mv);
            }
        } 
        else 
        {
            mv=99.99;
        }
        /*printf("%6.3f ",mv);*/
        ndata = simbad_findata(hh,"M.K","") ;
        if (ndata == 1)
        {
            TheData=simbad_getdata(hh,1);
            if (!strstr(TheData, ":"))
            {
                sscanf (TheData, "%lf", &mv);
            }
            else
            {
                sscanf (TheData, "%lf:", &mv);
            }
        } 
        else 
        {
            mv=99.99;
        }
        /*printf("%6.3f\n",mv);*/
    }
    simbad_disconnect(hh) ;

    /*printf("ra = %s\n", ra);
      printf("dec = %s\n", dec);*/

    logSetStdoutLogLevel(logQUIET);

    logInfo("Starting ...");
    vobsREQUEST request;


    if (request.SetConstraint(STAR_NAME_ID,miscDynBufGetBufferPointer(&starName)) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    if (request.SetConstraint(OBSERVED_BAND_ID,"V") == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    vobsSTAR star;
    star.SetProperty(POS_EQ_RA_MAIN_ID, ra);
    star.SetProperty(POS_EQ_DEC_MAIN_ID, dec);
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    vobsVIRTUAL_OBSERVATORY vobs;

    if (vobs.Search(request, starList)==FAILURE)
    {
        //errDisplayStack();
        //errCloseStack();
        exit(EXIT_FAILURE);
    }

    //errDisplayStack();
    starList.Display();


    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/

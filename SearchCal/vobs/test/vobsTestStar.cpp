/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestStar.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    23-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStar.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $"; 
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
#include "vobsSTAR.h"
#include "vobsPrivate.h"


int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogStateLevel(logEXTDBG);

    logInfo("Starting ...");

    vobsSTAR star1;
    vobsSTAR star2;
    float ra=0;
    float dec=0;
    
    char *ucd1="POS_EQ_RA_MAIN";    char *val1="12 00 00.0";
    char *ucd2="POS_EQ_PMDEC";      char *val2="36.5";
    char *ucd3="POS_EQ_DEC_MAIN";   char *val3="+36 51 64";
    
    if ((star1.SetProperty(ucd1, val1)==SUCCESS) &&
        (star1.SetProperty(ucd2, val2)==SUCCESS) &&
        (star2.SetProperty(ucd3, val3)==SUCCESS))
    {
        if (star1.IsSame(star2)==mcsFALSE)
        {
            star1.Update(star2);
            star1.Display();
            if ((star1.GetRa(ra)==SUCCESS)&&(star1.GetDec(dec)==SUCCESS))
            {
                cout << "************" << endl;
                cout << "*  ra= " << ra << endl;
                cout << "* dec= " << dec << endl;
                cout << "************" << endl;                
            }
        }
        else 
        {
            star1.Display();
        }
    }
    else 
    {
      errDisplayStack();
      errCloseStack();
      exit(EXIT_FAILURE);  
    }
      errDisplayStack();
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

    
}

/*___oOo___*/

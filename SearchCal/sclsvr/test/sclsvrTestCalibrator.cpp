/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrTestCalibrator.cpp,v 1.3 2005-01-26 14:17:55 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    23-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: sclsvrTestCalibrator.cpp,v 1.3 2005-01-26 14:17:55 scetre Exp $"; 
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
#include "timlog.h"

/*
 * Local Headers 
 */
#include "sclsvrCALIBRATOR.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrPrivate.h"

#include <iostream>
#include <algorithm>
#include <map>
using namespace std;

/* function object to check the value of a map element
 */
template <class K, class V>
class value_equals {
  private:
    V value;
  public:
    // constructor (initialize value to compare with)
    value_equals (const V& v)
     : value(v) {
    }
    // comparison
    bool operator() (pair<const K, V> elem) {
        return elem.second == value;
    }
};

int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logINFO);
    timlogStart(MODULE_ID, logINFO, "98", "testCalibrator");
    
    // add property to a star
    vobsSTAR star;
    star.SetPropertyValue(vobsSTAR_ID_MAIN, "25123");
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08");
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5");
    printf("STAR\n");
    for (int el2 = 0; el2 < star.NbProperties(); el2++)
    {
        printf("%s\t",star.GetNextProperty((mcsLOGICAL)(el2==0))
               ->GetName());
    }
    printf("\n");

    for (int el = 0; el < star.NbProperties(); el++)
    {
        printf("%s\t",star.GetNextProperty((mcsLOGICAL)(el==0))
               ->GetValue());
    }
    printf("\n");

    // create a calibrator from a star
    sclsvrCALIBRATOR calibrator(star);
    printf("CALIBRATOR\n");
    for (int el3 = 0; el3 < calibrator.NbProperties(); el3++)
     {
         printf("%s\t",calibrator.GetNextProperty((mcsLOGICAL)(el3==0))
                ->GetName());
     }
    printf("\n");
    
    for (int el4 = 0; el4 < calibrator.NbProperties(); el4++)
    {
        printf("%s\t",calibrator.GetNextProperty((mcsLOGICAL)(el4==0))->GetValue());
    }

    printf("\n");

    errCloseStack();
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

    
}

/*___oOo___*/

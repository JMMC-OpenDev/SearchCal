/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrTestCalibrator.cpp,v 1.1 2004-12-22 10:07:42 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    23-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: sclsvrTestCalibrator.cpp,v 1.1 2004-12-22 10:07:42 scetre Exp $"; 
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

    sclsvrCALIBRATOR calibrator;
    for (unsigned int el = 0; el < calibrator.NbProperties(); el++)
    {
        printf("%s\n",calibrator.GetNextProperty((mcsLOGICAL)(el==0))->GetName());
    }

    errCloseStack();
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

    
}

/*___oOo___*/

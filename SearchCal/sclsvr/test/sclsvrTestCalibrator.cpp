/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrTestCalibrator.cpp,v 1.8 2005-11-25 13:14:45 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.6  2005/03/03 16:49:05  scetre
 * updated test
 *
 * Revision 1.5  2005/02/15 15:52:00  gzins
 * Added CVS log as file modification history
 *
 * scetre    23-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: sclsvrTestCalibrator.cpp,v 1.8 2005-11-25 13:14:45 scetre Exp $"; 
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
#include "sclsvr.h"
#include "vobs.h"

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
    star.SetPropertyValue(vobsSTAR_ID_HIP, "25123", "perso");
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", "perso");
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "perso");
    star.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "2.01", "perso");
    // add property to a star
    vobsSTAR star2;
    star2.SetPropertyValue(vobsSTAR_ID_HIP, "25233", "perso");
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 45 01.32", "perso");
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 23 02.23", "perso");
    star2.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "2.01", "perso");
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
    sclsvrCALIBRATOR calibrator2(star2);
    sclsvrCALIBRATOR calibrator3(star2);
    
    sclsvrCALIBRATOR_LIST list1;
    sclsvrCALIBRATOR_LIST list2;
    list2.Copy(list1);
   
    list2.AddAtTail(calibrator2);
    list2.AddAtTail(calibrator);
    list2.AddAtTail(calibrator3);
    printf("before filter\n");
    list2.Display();
    sclsvrCALIBRATOR_LIST listTest;

    // create a list of filter
    vobsFILTER_LIST filterList;
    vobsDISTANCE_FILTER distanceFilter;
    // add distance filter in the list
    filterList.Add(&distanceFilter);
   
    vobsDISTANCE_FILTER *distanceFilterbis;
    // get distance filter of the list
    distanceFilterbis = (vobsDISTANCE_FILTER*)filterList.GetFilter(vobsDISTANCE_FILTER_NAME);
    // Set as enable the filter
    distanceFilterbis->Enable();
    distanceFilterbis->SetDistanceValue("03 47 29.08", "+24 06 18.5", 0.1, 0.1);
    cout << distanceFilterbis->GetName() << endl;
  
    // apply filter
    filterList.Apply(&list2);
    //distanceFilter.Apply(&list2);
    //list2.FilterByDistanceSeparation( "03 47 29.08", "+24 06 18.5", 0.1, 0.1);
/*    list2.GetMaximalMagnitudeSeparation("K", 2.5, 0.1);*/
    printf("filter list\n");
    list2.Display();

    sclsvrCALIBRATOR scienceObject;
    scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", "perso");
    scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "perso");


    printf("other display\n");
    scienceObject.Display();    
    printf("science object of the list : \n");
    list2.GetScienceObject(scienceObject);
    scienceObject.Display();
    errCloseStack();
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

    
}

/*___oOo___*/

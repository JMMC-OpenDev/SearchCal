/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


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
class value_equals
{
private:
    V value;
public:
    // constructor (initialize value to compare with)

    value_equals(const V& v)
    : value(v)
    {
    }
    // comparison

    bool operator() (pair<const K, V> elem)
    {
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
    star.SetPropertyValue(vobsSTAR_ID_HIP, "25123", vobsNO_CATALOG_ID);
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", vobsNO_CATALOG_ID);
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", vobsNO_CATALOG_ID);
    star.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "2.01", vobsNO_CATALOG_ID);

    // add property to a star
    vobsSTAR star2;
    star2.SetPropertyValue(vobsSTAR_ID_HIP, "25233", vobsNO_CATALOG_ID);
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 45 01.32", vobsNO_CATALOG_ID);
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 23 02.23", vobsNO_CATALOG_ID);
    star2.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "2.01", vobsNO_CATALOG_ID);
    printf("STAR\n");
    for (int el2 = 0; el2 < star.NbProperties(); el2++)
    {
        printf("%s\t", star.GetProperty(el2)->GetName());
    }
    printf("\n");

    for (int el = 0; el < star.NbProperties(); el++)
    {
        printf("%s\t", star.GetProperty(el)->GetValue());
    }
    printf("\n");

    // create a calibrator from a star
    sclsvrCALIBRATOR calibrator(star);
    printf("CALIBRATOR\n");
    for (int el3 = 0; el3 < calibrator.NbProperties(); el3++)
    {
        printf("%s\t", calibrator.GetProperty(el3)->GetName());
    }
    printf("\n");

    for (int el4 = 0; el4 < calibrator.NbProperties(); el4++)
    {
        printf("%s\t", calibrator.GetProperty(el4)->GetValue());
    }

    printf("\n");
    sclsvrCALIBRATOR calibrator2(star2);
    sclsvrCALIBRATOR calibrator3(star2);

    sclsvrCALIBRATOR_LIST list1("TestList1");
    sclsvrCALIBRATOR_LIST list2("TestList2");
    list2.Copy(list1);

    list2.AddAtTail(calibrator2);
    list2.AddAtTail(calibrator);
    list2.AddAtTail(calibrator3);
    printf("before filter\n");
    list2.Display();
    sclsvrCALIBRATOR_LIST listTest("TestList");

    // create a list of filter
    vobsFILTER_LIST filterList("Filter List");
    vobsDISTANCE_FILTER distanceFilter("Distance Filter");
    // add distance filter in the list
    filterList.Add(&distanceFilter, "Distance Filter");

    vobsDISTANCE_FILTER *distanceFilterbis;
    // get distance filter of the list
    distanceFilterbis = (vobsDISTANCE_FILTER*) filterList.GetFilter("Distance Filter");
    // Set as enable the filter
    distanceFilterbis->Enable();
    distanceFilterbis->SetDistanceValue("03 47 29.08", "+24 06 18.5", 0.1);

    // apply filter
    filterList.Apply(&list2);
    //distanceFilter.Apply(&list2);
    //list2.FilterByDistanceSeparation( "03 47 29.08", "+24 06 18.5", 0.1, 0.1);
    //list2.GetMaximalMagnitudeSeparation("K", 2.5, 0.1);
    printf("filter list\n");
    list2.Display();

    sclsvrCALIBRATOR scienceObject;
    scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", vobsNO_CATALOG_ID);
    scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", vobsNO_CATALOG_ID);


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

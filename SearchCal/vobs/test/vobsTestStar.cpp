/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestStar.cpp,v 1.3 2004-12-20 09:51:06 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    23-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStar.cpp,v 1.3 2004-12-20 09:51:06 scetre Exp $"; 
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
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsPrivate.h"

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

#if 0
int main()
{
    typedef map<float,float> FloatFloatMap;
    FloatFloatMap coll;
    FloatFloatMap::iterator pos;

    // fill container
    coll[1]=7;
    coll[2]=4;
    coll[3]=2;
    coll[4]=3;
    coll[5]=6;
    coll[6]=1;
    coll[7]=3;

    // search an element with key 3.0
    pos = coll.find(3.0);                     // logarithmic complexity
    if (pos != coll.end()) {
        cout << pos->first << ": "
             << pos->second << endl;
    }

    // search an element with value 3.0
    pos = find_if(coll.begin(),coll.end(),    // linear complexity
                  value_equals<float,float>(3.0));
    if (pos != coll.end()) {
        cout << pos->first << ": "
             << pos->second << endl;
    }
    cout << coll[7] << endl;
}
#else
int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logINFO);
    timlogStart(MODULE_ID, logINFO, "98", "testStar");

    logInfo("Starting ...");

    vobsSTAR_LIST list;
    
    for (int i=0; i<10000; i++)
    {
        vobsSTAR star;
        vobsSTAR star2(star);
        list.AddAtTail(star);
    }
    timlogStop("testStar");
    
#if 0
    vobsSTAR star1;
    star1.SetPropertyValue(vobsSTAR_PHOT_JHN_U, 12);
    star1.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "12 00 55");
    star1.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "12 00 55");
    star1.Display();
    vobsSTAR star2(star1);
    star2.Display();
    mcsFLOAT ra, dec;
    star2.GetRa(ra);   
    star2.GetDec(dec);   
    printf("star2.IsSame(star1) = %d\n", star2.IsSame(star1)); 
    printf("star2.IsSame(star2) = %d\n", star2.IsSame(star2)); 
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
#endif
    errCloseStack();
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

    
}
#endif

/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR_LIST.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    15-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR_LIST class definition.
  */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"; 
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
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"
#include "sclsvr.h"

/* Local variables */
static char *nameList[] =
{
   "hd",
   "hip",
   "ra",
   "dec",
   "pmdec",
   "pmra",
   "plx",
   "tsp",
   "varflag",
   "multflag",
   "glat",
   "glon",
   "radvel",
   "diam",
   "meth",
   "wlen",
   "photflux",
   "units",
   "U",
   "B",
   "V",
   "R",
   "I",
   "J",
   "H",
   "K",
   "L",
   "M",
   "N",
   "velocrotat",
   "color",
   NULL
};

static char *propertyNameList[] =
{
    "diamAngul",
    "diamAngulError",
    "Mo",
    "Lo",
    "Ko",
    "Ho",
    "Jo",
    "Io",
    "Ro",
    "Vo",
    "Bo",
    "mult",
    "vis",
    "visError",
    NULL
};

/*
 * Class constructor
 */
sclsvrCALIBRATOR_LIST::sclsvrCALIBRATOR_LIST()
{
}

/*
 * Class destructor
 */
sclsvrCALIBRATOR_LIST::~sclsvrCALIBRATOR_LIST()
{
}

//Copy constructor
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Copy(vobsSTAR_LIST& list)
{
    //logExtDbg("sclsvrCALIBRATOR_LIST::Copy()");
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        AddAtTail(*(list.GetNextStar((el==0))));
    }
    return SUCCESS;
}


/**
 * Adds the element at the end of the list
 *
 * \param calibrator element to be added to the list.
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(sclsvrCALIBRATOR &calibrator)
{
    //logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
    // Put element in the list
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(calibrator);
    _starList.push_back(newCalibrator);

    return SUCCESS;
}

/**
 * Adds the element star at the end of the calibrator list
 *
 * \param star element to be added to the list.
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(vobsSTAR &star)
{
    //logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
    
    // Put element in the list
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(star);
    _starList.push_back(newCalibrator);

    return SUCCESS;
}


/**
 * Complete each calibrator of the list
 *
 * Method to complete all calibrator of the list
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Complete()");
    for (unsigned int el = 0; el < Size(); el++)
    {
        if (((sclsvrCALIBRATOR *)GetNextStar((el==0)))->Complete(request) 
            == FAILURE)
        {
            printf("star %d not a calibrator\n", el+1);
        }
        else 
        {
            printf("star %d is a calibrator\n", el+1);
        }
    }
    return SUCCESS;
}

/**
 * Method to print the list on the console
 */
/**
 * Display the elements (calibrator) of the list.
 *
 * This method display all elements of the list on the console, using the
 * sclsvrCALIBRATOR::Display method.
 */
void sclsvrCALIBRATOR_LIST::Display(void)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Display()");

    // Display all element of the list 
    std::list<sclsvrCALIBRATOR>::iterator iter;
    for (int i=0; i<vobsNB_STAR_PROPERTIES; i++)
    {
        printf("%12s",nameList[i]);
    }
    printf("\n");
    for (int i=0; i<sclsvrNB_CALIBRATOR_PROPERTIES; i++)
    {
        printf("%12s",propertyNameList[i]);
    }

    for (unsigned int el = 0; el < Size(); el++)
    {
        GetNextStar((el==0))->Display();
    }
}

/*___oOo___*/

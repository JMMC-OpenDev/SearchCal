/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR_LIST.C,v 1.3 2004-11-30 15:48:09 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.C,v 1.3 2004-11-30 15:48:09 scetre Exp $"; 
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
    logExtDbg("sclsvrCALIBRATOR_LIST::Copy()");
    // Put each star of the vobsSTAR_LIST in the list
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
    logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
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
    logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
    
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
    // for each calibrator of the list 
    for (unsigned int el = 0; el < Size(); el++)
    {
        // Complete the calibrator
        if (((sclsvrCALIBRATOR *)GetNextStar((el==0)))->Complete(request) 
            == FAILURE)
        {
            logTest("star %d not a calibrator\n", el+1);
        }
        else 
        {
            logTest("star %d is a calibrator\n", el+1);
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
    printf("\n");

    for (unsigned int el = 0; el < Size(); el++)
    {
        GetNextStar((el==0))->Display();
    }
}

/**
 * Pack a calibrator list in a dynamic buffer
 *
 * \param buffer the dynamic buffer in which the calibrator will be pack
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Pack(miscDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Pack()");
    // for each calibrator of the list
    for (unsigned int el = 0; el < Size(); el++)
    {
        // Pack in the buffer the caliobrators
        if (((sclsvrCALIBRATOR *)GetNextStar((el==0)))->Pack(buffer) 
            == FAILURE )
        {
            return FAILURE;
        }
        // if it's not the last star of the list '\n' is written in order to
        // go to the next line
        if (el != Size()-1)
        {
            miscDynBufAppendString(buffer, "\n");
        }
        // if it's the last star '\0' in written
        else 
        {
            miscDynBufAppendString(buffer, "\0");            
        }
    }
    return SUCCESS;
}

/**
 * Unpack dynamic buffer and create a list
 *
 * \param buffer the dynamic buffer where is stord the list
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(miscDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::UnPack()");
    char *bufferLine=NULL;
    
    // Fix no skip flag
    mcsLOGICAL skipFlag = mcsFALSE;
    
    // Replace the '\n' by '\0' in the buffer where is stored the list
    miscReplaceChrByChr(miscDynBufGetBufferPointer(buffer), '\n', '\0');
    
    // Get the first line of the buffer
    while ((bufferLine=miscDynBufGetNextLinePointer(buffer,
                                                    bufferLine,
                                                    skipFlag)) != NULL)
    {
        // If the line had been get
        sclsvrCALIBRATOR calibrator;
        // unpack the calibrator which is in the line
        if (calibrator.UnPack(bufferLine) == FAILURE)
        {
            return FAILURE;
        }
        // add in the list the calibrator
        AddAtTail(calibrator); 
    }
    return SUCCESS;    
}
/*___oOo___*/

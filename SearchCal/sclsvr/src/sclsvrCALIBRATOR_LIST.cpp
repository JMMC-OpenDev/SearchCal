/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.12 2005-02-03 15:50:48 gzins Exp $"
*
* History
* -------
* scetre    15-Sep-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with new mcsLOGICAL enumerate
*
*******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR_LIST class definition.
  */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.12 2005-02-03 15:50:48 gzins Exp $"; 
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
        AddAtTail(*(list.GetNextStar((mcsLOGICAL)(el==0))));
    }
    return mcsSUCCESS;
}


/**
 * Adds the element at the end of the list
 *
 * \param calibrator element to be added to the list.
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(sclsvrCALIBRATOR &calibrator)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
    // Put element in the list
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(calibrator);
    _starList.push_back(newCalibrator);

    return mcsSUCCESS;
}

/**
 * Adds the element star at the end of the calibrator list
 *
 * \param star element to be added to the list.
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(vobsSTAR &star)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::AddAtTail()");
    
    // Put element in the list
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(star);
    _starList.push_back(newCalibrator);

    return mcsSUCCESS;
}


/**
 * Complete each calibrator of the list
 *
 * Method to complete all calibrator of the list
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Complete()");
    // for each calibrator of the list 
    for (unsigned int el = 0; el < Size(); el++)
    {
        // Complete the calibrator
        if (((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0)))->Complete(request) 
            == mcsFAILURE)
        {
            errCloseStack();
            logTest("star %d not a calibrator\n", el+1);
        }
        else 
        {
            logTest("star %d is a calibrator\n", el+1);
        }
    }
    return mcsSUCCESS;
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
/*void sclsvrCALIBRATOR_LIST::Display(void)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Display()");

    for (unsigned int el = 0; el < Size(); el++)
    {
        GetNextStar((mcsLOGICAL)(el==0))->Display();
    }
}*/

/**
 * Pack a calibrator list in a dynamic buffer
 *
 * This method shoulb be call after a call to request Pack method. If it is call
 * before, or if the request Pack method is not called, it is necessary to write
 * in the buffer before the call of this method to write an empty line. 
 *
 * \param buffer the dynamic buffer in which the calibrator will be pack
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Pack(miscDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Pack()");
    // for each calibrator of the list
    for (unsigned int el = 0; el < Size(); el++)
    {
        // Pack in the buffer the caliobrators
        if (((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0)))->Pack(buffer) 
            == mcsFAILURE )
        {
            return mcsFAILURE;
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
    return mcsSUCCESS;
}

/**
 * Unpack dynamic buffer and create a list.
 *
 * \param buffer the dynamic buffer where is stord the list
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(miscDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::UnPack()");
    char *bufferLine=NULL;
   
    // Fix no skip flag
    mcsLOGICAL skipFlag = mcsFALSE;

    // Replace the '\n' by '\0' in the buffer where is stored the list
    if (miscReplaceChrByChr(miscDynBufGetBuffer(buffer), '\n', '\0') == 
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the first line of the buffer
    while ((bufferLine=miscDynBufGetNextLine(buffer,
                                             bufferLine,
                                             skipFlag)) != NULL)
    {
        // If the line had been get
        sclsvrCALIBRATOR calibrator;
        // unpack the calibrator which is in the line
        if (calibrator.UnPack(bufferLine) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // add in the list the calibrator
        AddAtTail(calibrator); 
    }
    
    return mcsSUCCESS;    
}

/**
 * Get a list of calibrator with coherent diameter
 *
 * \param list the list to get
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 **/
mcsCOMPL_STAT 
    sclsvrCALIBRATOR_LIST::GetCoherentDiameter(sclsvrCALIBRATOR_LIST *list)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetCoherentDiameter()");
    // for each calibrator of the list
    for (unsigned int el = 0; el < Size(); el++)
    {
        
        if (((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0)))->
            HadCoherentDiameter()
            == mcsTRUE )
        {
            logTest("calibrator %d had coherent diameter\n", el+1);
            //list->AddAtTail(( *(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0)) ));
        }
    }
    return mcsSUCCESS;

}

/**
 * Get a list of calibrator with visibility ok
 *
 * \param list the list to get
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 **/
mcsCOMPL_STAT 
    sclsvrCALIBRATOR_LIST::GetVisibilityOk(sclsvrCALIBRATOR_LIST *list)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetCoherentDiameterList()");
    // for each calibrator of the list
    for (unsigned int el = 0; el < Size(); el++)
    {
        if (((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0)))-> VisibilityOk()
            == mcsTRUE )
        {
            logTest("calibrator %d had visibility OK\n", el+1);
            //list->AddAtTail(( *(sclsvrCALIBRATOR *)GetNextStar((el==0)) ));
        }
    }
    return mcsSUCCESS;

}

mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetScienceObjectSeparation(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetMaximalMagnitudeSeparation(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetSpectralType(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetLuminosityClass(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetMaximalExpectedRelativeAccuracy(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetVariability(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetMultiplicity(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Delete(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load(){return mcsSUCCESS;}
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(){return mcsSUCCESS;}
/*___oOo___*/

/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.16 2005-02-07 09:22:40 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.15  2005/02/04 15:49:00  gzins
* Fixed wrong Id for visibility property
*
* Revision 1.14  2005/02/04 15:30:19  gzins
* Updated Complete method
*
* Revision 1.13  2005/02/04 14:22:50  scetre
* Sort method for GUI added
*
* scetre    15-Sep-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with new mcsLOGICAL enumerate
*
*******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR_LIST class definition.
  */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.16 2005-02-07 09:22:40 gzins Exp $"; 
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

//Copy constructor
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Copy(sclsvrCALIBRATOR_LIST& list)
{
     
    logExtDbg("vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)");
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        AddAtTail(*(sclsvrCALIBRATOR *)(list.GetNextStar((mcsLOGICAL)(el==0))));
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
        sclsvrCALIBRATOR *calibrator;
        calibrator = (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));

        if (calibrator->Complete(request) == mcsFAILURE)
        {
            return mcsFAILURE;
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
   
    // Replace the '\n' by '\0' in the buffer where is stored the list
    if (miscReplaceChrByChr(miscDynBufGetBuffer(buffer), '\n', '\0') == 
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the first line of the buffer
    while ((bufferLine=miscDynBufGetNextLine(buffer, bufferLine,
                                             mcsFALSE)) != NULL)
    {
        // If it is not an empty line
        if (miscIsSpaceStr(bufferLine) == mcsFALSE)
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
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        
        if ((calibrator =
            ((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0))))->
            HadCoherentDiameter()
            == mcsTRUE )
        {
            logTest("calibrator %d had coherent diameter\n", el+1);
            list->AddAtTail(*calibrator);
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
    logExtDbg("sclsvrCALIBRATOR_LIST::GetVisibilityOkList()");
    sclsvrCALIBRATOR *calibrator;
    // for each calibrator of the list
    for (unsigned int el = 0; el < Size(); el++)
    {
        if ((calibrator=
             ((sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0))))->
            VisibilityOk() 
            == mcsTRUE )
        {
            logTest("calibrator %d had visibility OK\n", el+1);
            list->AddAtTail(*calibrator);
        }
    }
    return mcsSUCCESS;

}

/**
 * Method to remove calibrator which are not in a specific box
 *
 * \param scienceStarRa Right ascension coordinates of the science star
 * \param scienceStarDec Declinaison coordinates of the science star
 * \param raRange range in right ascension (one size of the box)
 * \param decRange range in declinaison (the other size of the box)
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT
    sclsvrCALIBRATOR_LIST::GetScienceObjectSeparation(char *scienceStarRa,
                                                      char *scienceStarDec,
                                                      mcsFLOAT raRange,
                                                      mcsFLOAT decRange)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetScienceObjectSeparation()");
    
    // create a star correponding to the science object
    sclsvrCALIBRATOR scienceStar;
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,
                                     scienceStarRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN,
                                     scienceStarDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    // Create a criterialist
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;
    // Add criteria on right ascension
    if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, raRange) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, decRange) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        // if the calibrator is different of the science star according to the
        // criteria list
        if ((calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0))) -> IsSame(scienceStar, &criteriaList) != mcsTRUE )
        {
            // Remove it
            logTest("calibrator %d not in the box\n", el+1);
            Remove(*calibrator);
            el = el-1;
        }
        else
        {
            logTest("calibrator %d in the box\n", el+1);
            
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have not a magitude in a range
 *
 * \param band the observed band
 * \param magValue the reference magnitude value
 * \param magRange the magnitude range
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT
sclsvrCALIBRATOR_LIST::GetMaximalMagnitudeSeparation(char *band,
                                                     mcsFLOAT magValue,
                                                     mcsFLOAT magRange)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetMaximalMagnitudeSeparation()"); 

    // Create the UCD corresponding to the band
    mcsSTRING256 magnitudeUcd;
    strcpy(magnitudeUcd, "PHOT_JHN_");
    strcat(magnitudeUcd, band);
    // create a star correponding to the science object
    sclsvrCALIBRATOR scienceStar;
    if (scienceStar.SetPropertyValue(magnitudeUcd,
                                     magValue) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Create a criterialist
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;
    // Add criteria on right ascension
    if (criteriaList.Add(magnitudeUcd, magRange) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        // if the calibrator is different of the science star according to the
        // criteria list
        if ((calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0))) -> IsSame(scienceStar, &criteriaList) != mcsTRUE )
        {
            // Remove it
            logTest("calibrator %d not magnitude in the range\n", el+1);
            Remove(*calibrator);
            el = el-1;            
        }
    }

    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have not the specified Spectral type
 */
mcsCOMPL_STAT
sclsvrCALIBRATOR_LIST::GetSpectralType(std::list<char *> spectTypeList)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetSpectralType()");;
    mcsLOGICAL isSameSpectralClass;
    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        // if the calibrator had a spectral type different to the wnated
        // spectral type
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        mcsSTRING32 spectralClass;
        // if it is not possible to get the spectral type
        if (calibrator->GetSpectralClass(spectralClass)==
            mcsFAILURE)
        {
            // Remove it
            Remove(*calibrator);
            el = el-1;
        }
        else
        {
            isSameSpectralClass=mcsFALSE;
            // compare the sectral class of te class with all the spectral
            // class in the list
            std::list<char *>::iterator spectTypeListIterator;
            spectTypeListIterator=spectTypeList.begin();
            while (spectTypeListIterator != spectTypeList.end())
            {
                if (strcmp((*spectTypeListIterator), spectralClass) == 0)
                {
                    isSameSpectralClass = mcsTRUE;
                }
                spectTypeListIterator++;
            }
            // if the spectral of the list is not one of the list
            if (isSameSpectralClass != mcsTRUE)
            {            
                // Remove it
                Remove(*calibrator);
                el = el-1;            
            }
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have not the specified luminosity class
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR_LIST::GetLuminosityClass(std::list<char *> luminosityList)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetLuminosityClass()");

    mcsLOGICAL isSameLuminosityClass;
    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        // if the calibrator had a spectral type different to the wanted
        // spectral type
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        mcsSTRING32 luminosityClass;
        // if it is not possible to get the luminosity class
        if (calibrator->GetLuminosityClass(luminosityClass)==
            mcsFAILURE)
        {
            // Remove it
            Remove(*calibrator);
            el = el-1;
        }
        else
        {
            isSameLuminosityClass=mcsFALSE;
            // compare the luminosity class of te class with all the spectral
            // class in the list
            std::list<char *>::iterator luminosityListIterator;
            luminosityListIterator=luminosityList.begin();
            while (luminosityListIterator != luminosityList.end())
            {
                if (strcmp((*luminosityListIterator), luminosityClass) == 0)
                {
                    isSameLuminosityClass = mcsTRUE;
                }
                luminosityListIterator++;
            }
            // if the spectral of the list is not one of the list
            if (isSameLuminosityClass != mcsTRUE)
            {            
                // Remove it
                Remove(*calibrator);
                el = el-1;            
            }
        }
    }

    
    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have not the specified expected accuracy
 */
mcsCOMPL_STAT
sclsvrCALIBRATOR_LIST::GetMaximalExpectedRelativeAccuracy(mcsFLOAT visMax)
{
    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    mcsFLOAT calibratorVis;
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        // if it is not possible to get the visibility, remove the star
        if (calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS,
                                         &calibratorVis) == mcsFAILURE)
        {
            // Remove it
            logTest("calibrator %d not visibility enough\n", el+1);
            Remove(*calibrator);
            el = el-1;            
        }
        // if it is possible to get the star
        else
        {
            // if the value of the calibrator visibility is lower than this
            // enter by the user, remove it
            if (calibratorVis < visMax)   
            {
                // Remove it
                logTest("calibrator %d not visibility enough\n", el+1);
                Remove(*calibrator);
                el = el-1;
            }
        }
    }
   
    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have Variability if they are not
 * authorized.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetVariability(mcsLOGICAL authorized)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetVariability()");
    // if varability are not authorized
    // Check in all star if they have a variability flag.
    // if a star have the flag N or G, remove it
    if (authorized == mcsFALSE)
    {
        sclsvrCALIBRATOR *calibrator;
        for (unsigned int el = 0; el < Size(); el++)
        {
            calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility, remove the star
            if (calibrator->GetPropertyValue(vobsSTAR_CODE_VARIAB) != NULL)
            {
                // check if the var flag property is = N or G
                if ((strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_VARIAB), "N") == 0)||
                    (strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_VARIAB), "G") == 0))
                {
                    // Remove it
                    logTest("calibrator %d had variability %s\n",
                            el+1, 
                            calibrator->GetPropertyValue(vobsSTAR_CODE_VARIAB));
                    Remove(*calibrator);
                    el = el-1;            
                }
            }
        }
    }
    return mcsSUCCESS;
}

/**
 * Method to remove calibrator which have Multiplicity
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetMultiplicity(mcsLOGICAL authorized)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetMultiplicity()");
    // if multiplicity are not authorized
    // Check in all star if they have a multiplicity flag.
    // if a star have the flag C, G, O, V or X remove it
    if (authorized == mcsFALSE)
    {
        sclsvrCALIBRATOR *calibrator;
        for (unsigned int el = 0; el < Size(); el++)
        {
            calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility, remove the star
            if (calibrator->GetPropertyValue(vobsSTAR_CODE_MULT_FLAG) != NULL)
            {
                // check if the mult flag property is = C, G, O, V, X
                if ((strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG),
                            "C") == 0)||
                    (strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG),
                            "G") == 0)||
                    (strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG),
                            "O") == 0)||
                    (strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG),
                            "V") == 0)||
                    (strcmp(calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG),
                            "X") == 0))
                {
                    // Remove it
                    logTest("calibrator %d had multiplicity %s\n",
                            el+1, 
                            calibrator->
                            GetPropertyValue(vobsSTAR_CODE_MULT_FLAG));
                    Remove(*calibrator);
                    el = el-1;            
                }
            }
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Method to delete a star from his number in the list
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Delete(unsigned int starNumber)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Delete(%d)", starNumber);

    // if the number is negative or higher than the list size, return erro
    if ((starNumber < 0) || (starNumber > Size()))
    {
        // todo err
        return mcsFAILURE;
    }
    sclsvrCALIBRATOR *calibrator;    
    // go to the star
    for (unsigned int el = 0; el <= starNumber; el++)
    {
        calibrator = (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
    }
    // Once the star is touch, remove it from the list
    Remove(*calibrator);
    return mcsSUCCESS;
}

/**
 * Method to load a star list from a file
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load()
{
    return mcsSUCCESS;
}

/*___oOo___*/

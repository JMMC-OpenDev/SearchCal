/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.26 2005-02-14 14:13:19 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.25  2005/02/10 08:20:02  gzins
 * Fixed bug in Copy method
 *
 * Revision 1.24  2005/02/08 20:48:11  gzins
 * Updated Copy(); added filterDiameterNok and filterVisibilityNok parameters
 * Removed obsolete CopyIn(), GetCoherentDiameter() and GetVisibilityOk() methods
 *
 * Revision 1.23  2005/02/08 07:24:07  gzins
 * Changed char* to const char* when applicable
 *
 * Revision 1.22  2005/02/08 04:39:32  gzins
 * Updated for new vobsREQUEST API and used new sclsvrREQUEST class
 *
 * Revision 1.21  2005/02/07 15:00:59  gzins
 * Reformated file header
 *
 * Revision 1.20  2005/02/07 14:44:51  gzins
 * Changed slcsvr.h to sclsvrCALIBRATOR_LIST.h
 *
 * Revision 1.19  2005/02/07 11:10:29  scetre
 * Removed obsolete class sclsvrSEARCH_CALIBRATOR trace and changed method name
 * in sclsvrCALIBRATOR_LIST
 *
 * Revision 1.18  2005/02/07 10:06:36  scetre
 * Changed filter method name in FilterBy...
 *
 * Revision 1.17  2005/02/07 09:50:45  scetre
 * Changed HadCoherentDiameter and VisibilityOk method in IsDiameterOk and
 * IsVisibilityOk
 *
 * Revision 1.16  2005/02/07 09:22:40  gzins
 * Added method to copy list from another sclsvrCALIBRATOR_LIST instance
 *
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
 ******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR_LIST class definition.
  */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.26 2005-02-14 14:13:19 scetre Exp $"; 
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
#include "sclsvrCALIBRATOR_LIST.h"

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

/**
 * Import list of calibrators from a list of stars.
 *
 * \param list list containing calibrators to be imported
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
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
 * Copy Method from a sclsvrCALIBRATOR_LIST
 * Import list of calibrators from another list.
 *
 * \param list list containing calibrators to be imported
 * \param copyDiameterNok if mcsFALSE do not copy calibrator with a no coherent
 * diameter
 * \param copyVisibilityNok if mcsFALSE do not copy calibrator with a visibility
 * less than the expected one.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Copy(sclsvrCALIBRATOR_LIST& list,
                                          mcsLOGICAL copyDiameterNok,
                                          mcsLOGICAL copyVisibilityNok)
{
     
    logExtDbg("vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)");
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        // Get next calibrator
        sclsvrCALIBRATOR *calibrator;
        calibrator = (sclsvrCALIBRATOR *)list.GetNextStar((mcsLOGICAL)(el==0));
        
        // Check if this calibrator has to be copy in or not
        mcsLOGICAL copyIt = mcsTRUE;
        if (((copyDiameterNok == mcsFALSE) && 
             (calibrator->IsDiameterOk() == mcsFALSE)) ||
            ((copyVisibilityNok == mcsFALSE) && 
             (calibrator->IsVisibilityOk() == mcsFALSE)))
        {
            copyIt = mcsFALSE;
        }

        // If yes, copy it
        if (copyIt == mcsTRUE)
        {
            AddAtTail(*calibrator);
        }
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
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(sclsvrREQUEST &request)
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
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Pack(miscoDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Pack()");
    
    vobsCDATA cdata;
    sclsvrCALIBRATOR calibrator;
    
    // In unpack method, extended logical is true
    if (cdata.Store(calibrator, *this, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    buffer->AppendString(cdata.GetBuffer());

    return mcsSUCCESS;
}

/**
 * Unpack dynamic buffer and create a list.
 *
 * \param buffer the dynamic buffer where is stord the list
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(miscoDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::UnPack()");
   
    // create a cdata object and put the content of the buffer in it
    vobsCDATA cdata;
    if (cdata.LoadBuffer(buffer->GetBuffer()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    sclsvrCALIBRATOR calibrator;
    
    if (cdata.Extract(calibrator, *this, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
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
sclsvrCALIBRATOR_LIST::FilterByDistanceSeparation(const char *scienceRa,
                                                  const char *scienceDec,
                                                  mcsFLOAT raRange,
                                                  mcsFLOAT decRange)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByDistanceSeparation()");
    
    // create a star correponding to the science object
    sclsvrCALIBRATOR scienceStar;
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,
                                     scienceRa, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN,
                                     scienceDec, "") == mcsFAILURE)
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
sclsvrCALIBRATOR_LIST::FilterByMagnitude(const char *band,
                                         mcsFLOAT magValue,
                                         mcsFLOAT magRange)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByMagnitude()"); 

    // Create the UCD corresponding to the band
    mcsSTRING256 magnitudeUcd;
    strcpy(magnitudeUcd, "PHOT_JHN_");
    strcat(magnitudeUcd, band);
    // create a star correponding to the science object
    sclsvrCALIBRATOR scienceStar;
    if (scienceStar.SetPropertyValue(magnitudeUcd,
                                     magValue, "") == mcsFAILURE)
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
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT
sclsvrCALIBRATOR_LIST::FilterBySpectralType(std::list<char *> spectTypeList)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterBySpectralType()");;
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
        if (calibrator->GetSpectralClass(spectralClass)== mcsFAILURE)
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
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR_LIST::FilterByLuminosityClass(std::list<char *> luminosityList)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByLuminosityClass()");

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
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT
sclsvrCALIBRATOR_LIST::FilterByVisibility(mcsFLOAT visMax)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByVisibility()");

    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    mcsFLOAT calibratorVis;
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        // if it is not possible to get the visibility, remove the star
        if (calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS2,
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
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::FilterByVariability(mcsLOGICAL authorized)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByVariability()");
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
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR_LIST::FilterByMultiplicity(mcsLOGICAL authorized)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterByMultiplicity()");
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
 *
 * \param starNumber the number of the star to removed
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
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
 * Save the elements (calibrators) of the list in a file.
 *
 * \param filename the file where to save
 * \param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only property is saved.
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          mcsLOGICAL extendedFormat)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Save()");

    // Store list into the CDATA
    vobsCDATA cData;
    sclsvrCALIBRATOR  calibrator;
    if (cData.Store(calibrator, *this, extendedFormat) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Save into file
    if (cData.SaveInFile(filename) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Load elements (calibrators) from a file.
 *
 * \param filename name of file containing star list
 * \param extendedFormat if true, each property is has been saved with its
 * attributes (origin and confidence index), otherwise only only property has
 * been saved.
 * \param origin used if origin is not given in file (see above). If NULL, the
 * name of file is used as origin.
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load(const char *filename,
                                          mcsLOGICAL extendedFormat,
                                          const char *origin)
{
    logExtDbg("vobsSTAR_LIST::Load()");

    // Load file
    vobsCDATA cData;
    if (cData.LoadFile(filename) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set origin (if needed)
    if (extendedFormat == mcsFALSE)
    {
        if (origin == NULL)
        {
            cData.SetCatalogName(filename);
        }
        else
        {
            cData.SetCatalogName(origin);
        }
    }
        
    // Extract list from the CDATA
    sclsvrCALIBRATOR calibrator;
    if (cData.Extract(calibrator, *this, extendedFormat) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/*___oOo___*/

/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.39 2005-03-07 15:16:08 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.38  2005/03/07 14:17:32  scetre
 * Changed 'JMMC calibrators Group' to 'JMMC calibrator Group' in save file
 *
 * Revision 1.37  2005/03/06 20:34:23  gzins
 * Improved Save/Load methods; added file header, used REQUEST/FORMAT tags and added automatic format detection when loading file
 *
 * Revision 1.36  2005/03/06 10:48:30  gzins
 * Merged FilterBySpectralType and FilterByLuminosityClass methods
 *
 * Revision 1.35  2005/03/04 16:28:52  scetre
 * Changed Call to Save method
 *
 * Revision 1.34  2005/03/04 15:51:21  scetre
 * Updated call to Save method
 *
 * Revision 1.33  2005/03/03 16:48:22  scetre
 * Added GetScienceObject method
 *
 * Revision 1.32  2005/02/22 16:24:00  gzins
 * Updated misco::GetNextLine API
 *
 * Revision 1.31  2005/02/17 15:31:26  gzins
 * Added request parameter to Save and Load methods
 *
 * Revision 1.30  2005/02/17 09:24:24  scetre
 * updated Delete method
 *
 * Revision 1.29  2005/02/16 17:34:23  gzins
 * Changed prototype for Unpack; used const char* instead of miscoDYN_BUF
 *
 * Revision 1.28  2005/02/16 16:56:30  gzins
 * Fixed wrong parameter name in documentation
 *
 * Revision 1.27  2005/02/15 15:54:00  gzins
 * Changed remaining SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.26  2005/02/14 14:13:19  scetre
 * Added Load and Save methods
 * change miscDYN_BUF to miscoDYN_BUF in pack and unpack methods
 *
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

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.39 2005-03-07 15:16:08 scetre Exp $"; 
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
#include "sclsvrVersion.h"
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
    
    vobsSTAR_PROPERTY_ID_LIST ucdList;
    // In unpack method, extended logical is true
    if (cdata.Store(calibrator, *this, ucdList, mcsTRUE) == mcsFAILURE)
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
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(const char *buffer)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::UnPack()");
   
    // create a cdata object and put the content of the buffer in it
    vobsCDATA cdata;
    if (cdata.LoadBuffer(buffer) == mcsFAILURE)
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
 * \param scienceRa Right ascension coordinates of the science star
 * \param scienceDec Declinaison coordinates of the science star
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
 * Filter calibrator list by spectral type.
 *
 * This method removes all the calibrators whose the spectral type does not
 * match the given temperature or luminosity classes. The temperature and
 * luminosity classes, used to filter list, are in two separated tables which
 * are ends by a NULL to mark the end of table.
 *
 * \param tempClassList list of accepted temperature classes. If NULL, no filter
 * is applied on temperature class.
 * \param lumClassList list of accepted luminosity classes. If NULL, no filter
 * is applied on luminosity class.
 * 
 * \warning Only temperature class coded on one letter is supported.
 * \ex
 * Filter all calibrators belonging to the temperature classes 'O' and 'G'.
 * No filter applied on luminosity class.
 * \code
 * char *temperatureClassList[3] = {"O", "G", NULL};
 * list.FilterBySpectralType(temperatureClassList, NULL);
 * \endcode
 * Filter all calibrators belonging to the luminosity classes 'I' and 'IV'.
 * No filter applied on temperature class.
 * \code
 * char *luminosityClassList[3] = {"I", "IV", NULL};
 * list.FilterBySpectralType(NULL, luminosityClassList);
 * \endcode
 * ls
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::FilterBySpectralType(char *tempClassList[],
                                                          char *lumClassList[])
{
    logExtDbg("sclsvrCALIBRATOR_LIST::FilterBySpectralType()");;

    // For each calibrator in the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator = (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));

        // If spectral type is unknown
        if (calibrator->IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
        {
            // Remove it
            Remove(*calibrator);
            el = el - 1;
        }
        // Else
        else
        {
            // Get the star spectral type 
            const char *spType;
            spType = calibrator-> GetPropertyValue(vobsSTAR_SPECT_TYPE_MK);
                
            // Look for temperature class(es). Stop when end of string reached
            // or first character of luminosity class found 
            mcsINT32 spIdx = 0; // Index on spectral type string
            mcsLOGICAL tempClassMatch = mcsFALSE;
            while ((spType[spIdx] != '\0') && 
                   (spType[spIdx] != 'I') && (spType[spIdx] != 'V'))
            {
                // If the list of temperature classes has been given
                if (tempClassList != NULL)
                {
                    // For ecah given temperature class
                    mcsINT32 classIdx = 0;
                    while (tempClassList[classIdx] != NULL)
                    {
                        // Check if 
                        if (spType[spIdx] == tempClassList[classIdx][0])
                        {
                            tempClassMatch = mcsTRUE;
                        }
                        classIdx++;
                    }
                }
                // Else
                else
                {
                    // Do not filter
                    tempClassMatch = mcsTRUE;
                }
                spIdx++;
            }

            mcsLOGICAL lumClassMatch = mcsFALSE;
            // If the list of luminosity classes has been given
            if (lumClassList != NULL)
            {
                // Look for luminosity class(es). Stop when end of string
                // recahed
                mcsSTRING8 lumClass;
                memset(lumClass, '\0', sizeof(mcsSTRING8)); 
                mcsLOGICAL endOfString = mcsFALSE;
                mcsINT32   lumIdx = 0;
                while (endOfString == mcsFALSE)
                {
                    // Get all consecutive characters constituing luminosity
                    // class
                    if ((spType[spIdx] == 'I') || (spType[spIdx] == 'V'))
                    {
                        lumClass[lumIdx] = spType[spIdx];
                        lumIdx++;
                    }
                    else
                    {
                        // If luminosity class has been found
                        if (lumIdx != 0)
                        {
                            // For ecah given temperature class
                            mcsINT32 classIdx = 0;
                            while (lumClassList[classIdx] != NULL)
                            {
                                // Check if 
                                if (strcmp(lumClass, 
                                           lumClassList[classIdx]) == 0)
                                {
                                    lumClassMatch = mcsTRUE;
                                }
                                classIdx++;
                            }

                            memset(lumClass, '\0', sizeof(mcsSTRING8)); 
                            lumIdx = 0;
                        }
                    }
                    if (spType[spIdx] == '\0')
                    {
                        endOfString = mcsTRUE;
                    }

                    spIdx++;
                }
            }
            // Else
            else
            {
                // Do not filter
                lumClassMatch = mcsTRUE;
            }

            // If the spectral of the list is not one of the list
            if ((tempClassMatch == mcsFALSE) || (lumClassMatch == mcsFALSE))
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
    mcsFLOAT calibratorVis, calibratorVisError;
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        // if it is not possible to get the visibility or the visibility error,
        // remove the star
        if ((calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS2,
                                         &calibratorVis) == mcsFAILURE)||
            (calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR,
                                          &calibratorVisError) == mcsFAILURE) )
        {
            // Remove it
            logTest("calibrator %d not visibility enough\n", el+1);
            Remove(*calibrator);
            el = el-1;            
        }
        // if it is possible to get the star
        else
        {
            // if the value of the calibrator visibility error / visibility
            // is lower than this enter by the user, remove it
            if (calibratorVisError/calibratorVis <= visMax)   
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
    if ((starNumber < 1) || (starNumber > Size()))
    {
        // todo err
        return mcsFAILURE;
    }
    sclsvrCALIBRATOR *calibrator;    
    // go to the star
    for (unsigned int el = 0; el <= starNumber-1; el++)
    {
        calibrator = (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
    }
    // Once the star is touch, remove it from the list
    if (Remove(*calibrator) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Save the elements (calibrators) of the list in a file.
 *
 * If given, the request is placed in the file, as a comment line.
 *
 * \param filename the file where to save
 * \param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only property is saved.
 * \param request request used to get this list 
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          vobsSTAR_PROPERTY_ID_LIST ucdList,
                                          sclsvrREQUEST &request,
                                          mcsLOGICAL extendedFormat)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Save()");

    vobsCDATA cData;

    // Add creation date and SW version
    mcsSTRING32 utcTime;
    miscGetUtcTimeStr(0, utcTime);
    mcsSTRING256 line;
    cData.AppendString("# JMMC - Calibrator group\n");
    cData.AppendString("#\n");
    cData.AppendString("# This file has been created by Search Calibrators Software\n");
    sprintf(line, "#\t\tVersion : %s\n", sclsvrVERSION);
    cData.AppendString(line);
    sprintf(line, "#\t\tDate    : %s\n", utcTime);
    cData.AppendString(line);
    cData.AppendString("#\n");
    
    // Add request into the file (if given)
    mcsSTRING1024 cmdParamLine;
    if (request.GetCmdParamLine(cmdParamLine) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (strlen(cmdParamLine) != 0)
    {
        cData.AppendString(sclsvrREQUEST_TAG);
        cData.AppendString(cmdParamLine);
        cData.AppendString("\n");
    }

    // Add format : STANDARD or EXTENDED
    cData.AppendString(sclsvrFORMAT_TAG);
    if (extendedFormat == mcsTRUE)
    {
        cData.AppendString("EXTENDED");
    }
    else
    {
        cData.AppendString("STANDARD");
    }
    cData.AppendString("\n");
    
    // Store list into the CDATA
    sclsvrCALIBRATOR  calibrator;
    if (cData.Store(calibrator, *this, ucdList, extendedFormat) == mcsFAILURE)
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
 * Save the elements (calibrators) of the list in a file.
 *
 * If given, the request is placed in the file, as a comment line.
 *
 * \param filename the file where to save
 * \param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only property is saved.
 * \param request request used to get this list 
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          sclsvrREQUEST &request,
                                          mcsLOGICAL extendedFormat)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::Save()");

    vobsSTAR_PROPERTY_ID_LIST ucdList;
    return Save(filename, ucdList, request, extendedFormat);
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
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load(const char *filename,
                                          sclsvrREQUEST &request)
{
    logExtDbg("vobsSTAR_LIST::Load()");

    // File format; by default standard format is assumed
    mcsLOGICAL extendedFormat = mcsFALSE;

    // Load file
    vobsCDATA cData;
    if (cData.LoadFile(filename) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Look for request and format in comment lines
    const char *from = NULL;
    mcsSTRING1024 cmdParamLine;
    do 
    {
        from = cData.GetNextCommentLine
            (from, cmdParamLine, sizeof(mcsSTRING1024));
        if (from != NULL)
        {
            if (strncmp(cmdParamLine, 
                        sclsvrREQUEST_TAG, strlen(sclsvrREQUEST_TAG)) == 0)
            {
                // Remove request tag
                miscTrimString(cmdParamLine, sclsvrREQUEST_TAG);

                // Parse the found request
                if (request.Parse(cmdParamLine) == mcsFAILURE)
                {
                    errAdd(sclsvrERR_REQUEST_LINE_FORMAT, filename,
                           cmdParamLine);
                    return mcsFAILURE;
                }
            }
            else if (strncmp(cmdParamLine, 
                             sclsvrFORMAT_TAG, strlen(sclsvrFORMAT_TAG)) == 0)
            {
                // Remove request tag
                miscTrimString(cmdParamLine, sclsvrFORMAT_TAG);

                // Parse the found request
                if (strcmp(cmdParamLine, "EXTENDED") == 0)
                {
                    extendedFormat = mcsTRUE;
                }
            }
        }
    } while (from != NULL);

    // Extract list from the CDATA
    sclsvrCALIBRATOR calibrator;
    if (cData.Extract(calibrator, *this, extendedFormat) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Get Science object if it is present in the list.
 *
 * This method complete the calibrator which should have been built before. It
 * should have as the minimum of properties, the value of his right ascension
 * and declinaison position. If one of this properties is not present in the
 * list, an error is returned.
 *
 * \param scienceObject the science object
 * 
 * \return mcsSUCCESS if the science star had been found and had been updated,
 * else return mcsFAILURE 
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR_LIST::GetScienceObject(sclsvrCALIBRATOR &scienceObject)
{
    logExtDbg("sclsvrCALIBRATOR_LIST::GetScienceObject()");
   
    // Check if coordinates of the science star are present in order to be able
    // to compare
    if ((scienceObject.IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsFALSE) ||
        (scienceObject.IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsFALSE))
    {
        return mcsFAILURE;
    }
    
    // Logical flag to know if the object had been found in the list
    // At beginning, the objet is not found
    mcsLOGICAL isScienceObjectFound = mcsFALSE;
   
    sclsvrCALIBRATOR *calibrator;
    // for each star of the list, check if the coordinates are the same as the
    // science object coordinates.
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
        // if the next star of the list is the same that the science object
        if (scienceObject.IsSame(*calibrator) == mcsTRUE)
        {
            // update value of the calibrator
            if (scienceObject.Update(*calibrator) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // changed flag as true
            isScienceObjectFound = mcsTRUE;            
        }
    }
   
    // if the science object had not been found
    if (isScienceObjectFound == mcsFALSE)
    {
        // return failure
        return mcsFAILURE;
    }

    // If it has been found, return success
    return mcsSUCCESS;
}


/*___oOo___*/

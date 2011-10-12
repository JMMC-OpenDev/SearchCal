/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclsvrCALIBRATOR_LIST class definition.
 */

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


/**
 * Class constructor
 */
sclsvrCALIBRATOR_LIST::sclsvrCALIBRATOR_LIST()
{
}

/**
 * Class destructor
 */
sclsvrCALIBRATOR_LIST::~sclsvrCALIBRATOR_LIST()
{
}


/**
 * Fill the list from a given list of stars.
 *
 * @param list list containing stars to be imported as calibrators
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Copy(vobsSTAR_LIST& list)
{
    logTrace("sclsvrCALIBRATOR_LIST::Copy()");

    const unsigned int nbStars = list.Size();
    
    // Put each star of the given vobsSTAR_LIST in the internal list
    for (unsigned int el = 0; el < nbStars; el++)
    {
        AddAtTail(*(list.GetNextStar((mcsLOGICAL)(el==0))));
    }

    return mcsSUCCESS;
}

/**
 * Fill the list from a given list of calibrators.
 *
 * @param list list containing calibrators to be imported
 * @param copyDiameterNok if mcsFALSE do not copy calibrator with a not coherent
 * diameter
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Copy(sclsvrCALIBRATOR_LIST& list,
                                          mcsLOGICAL copyDiameterNok)
{
    logTrace("vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)");

    sclsvrCALIBRATOR* calibrator = NULL;
    bool copyIt;
    
    const unsigned int nbStars = list.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        // Get next calibrator
        calibrator = (sclsvrCALIBRATOR*)list.GetNextStar((mcsLOGICAL)(el==0));
        
        copyIt = true;
        
        // Check wether this calibrator has to be copyied in or not
        if ((copyDiameterNok == mcsFALSE) &&  (calibrator->IsDiameterOk() == mcsFALSE))
        {
            copyIt = false;
        }

        // If yes, copy it
        if (copyIt)
        {
            if (AddAtTail(*calibrator) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Adds the given calibrator at the end of the list.
 *
 * @param calibrator calibrator to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(const sclsvrCALIBRATOR &calibrator)
{
    // Copy the given calibrator
    sclsvrCALIBRATOR* newCalibrator = new sclsvrCALIBRATOR(calibrator);

    // Add one pointer of the calibrator in the list
    _starList.push_back(newCalibrator);

    return mcsSUCCESS;
}

/**
 * Adds the given star as a calibrator at the end of the list.
 *
 * @param star star to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(const vobsSTAR &star)
{
    // Copy the given star as a calibrator
    sclsvrCALIBRATOR* newCalibrator = new sclsvrCALIBRATOR(star);

    // Add one pointer of the calibrator in the list
    _starList.push_back(newCalibrator);

    return mcsSUCCESS;
}

/**
 * Complete each calibrator of the list according to the user request.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(const sclsvrREQUEST &request)
{
    const unsigned int nbStars = Size();

    logTest("Complete: start [%d stars]", nbStars);

    sclsvrCALIBRATOR* calibrator;
    
    // For each calibrator of the list 
    for (unsigned int el = 0; el < nbStars; el++)
    {
        // Complete the calibrator
        calibrator = (sclsvrCALIBRATOR*)GetNextStar((mcsLOGICAL)(el==0));

        if (calibrator->Complete(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Sort calibrators according to their distance from the science object in
    // ascending order, i.e. the closest first.
    
    // TODO: non sense with catalogs: use another field ??
    
    Sort(sclsvrCALIBRATOR_DIST);

    logTest("Complete(): done [%d stars]", nbStars);

    return mcsSUCCESS;
}

/**
 * Serialize a calibrator list.
 *
 * @warning This method should be called after a call to request Pack method. If
 * it is called before, or if the request Pack method is not called, it is
 * necessary to write in the buffer before the call of this method to write an
 * empty line. 
 *
 * @param buffer the dynamic buffer in which the calibrator will be packed
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Pack(miscoDYN_BUF *buffer)
{
    logTrace("sclsvrCALIBRATOR_LIST::Pack()");
    
    vobsCDATA cdata;
    sclsvrCALIBRATOR calibrator;
    
    // In unpack method, extended logical is true
    vobsSTAR_PROPERTY_ID_LIST ucdList;
    if (cdata.Store(calibrator, *this, ucdList, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    buffer->AppendString(cdata.GetBuffer());

    return mcsSUCCESS;
}

/**
 * Deserialize a calibrator list from.
 *
 * @param buffer the dynamic buffer in which the serialized list is stored
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(const char *buffer)
{
    logTrace("sclsvrCALIBRATOR_LIST::UnPack()");
   
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
 * Save the calibrator list in a file.
 *
 * If given, the request is placed in the file as a comment line.
 *
 * @param filename the file in which the calibrator list should be saved
 * @param ucdList list of UCD
 * @param request request used to get this list 
 * @param extendedFormat if mcsTRUE, each property is saved with its attributes
 * (origin and confidence index), otherwise only the property is saved.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          vobsSTAR_PROPERTY_ID_LIST ucdList,
                                          const sclsvrREQUEST &request,
                                          mcsLOGICAL extendedFormat)
{
    logTrace("sclsvrCALIBRATOR_LIST::Save()");

    // Get creation date and SW version
    mcsSTRING32 utcTime;
    if (miscGetUtcTimeStr(0, utcTime) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mcsSTRING256 line;
    vobsCDATA cData;
    cData.AppendString("# JMMC - Calibrator group\n");
    cData.AppendString("#\n");
    cData.AppendString("# This file has been created by Search Calibrators Software\n");
    sprintf(line, "#\t\tVersion : %s\n", sclsvrVERSION);
    cData.AppendString(line);
    sprintf(line, "#\t\tDate    : %s\n", utcTime);
    cData.AppendString(line);
    cData.AppendString("#\n");
    
    // Add the request into the file (if given)
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
 * Save the calibrators of the list in a file.
 *
 * If given, the request is placed in the file, as a comment line.
 *
 * @param filename the file where to save
 * @param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only property is saved.
 * @param request request used to get this list 
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          const sclsvrREQUEST &request,
                                          mcsLOGICAL extendedFormat)
{
    logTrace("sclsvrCALIBRATOR_LIST::Save()");

    vobsSTAR_PROPERTY_ID_LIST ucdList;
    return Save(filename, ucdList, request, extendedFormat);
}

/**
 * Load a calibrator list and a request from a file.
 *
 * @param filename name of file containing the calibrator list
 * @param request request object loaded from file
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load(const char*     filename,
                                          sclsvrREQUEST  &request)
{
    logTrace("sclsvrCALIBRATOR_LIST::Load()");

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
        from = cData.GetNextCommentLine(from, cmdParamLine,
                                        sizeof(cmdParamLine));
        if (from != NULL)
        {
            if (strncmp(cmdParamLine, sclsvrREQUEST_TAG,
                        strlen(sclsvrREQUEST_TAG)) == 0)
            {
                // Remove request tag
                if (miscTrimString(cmdParamLine, sclsvrREQUEST_TAG) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Parse the found request
                if (request.Parse(cmdParamLine) == mcsFAILURE)
                {
                    errAdd(sclsvrERR_REQUEST_LINE_FORMAT, filename,
                           cmdParamLine);
                    return mcsFAILURE;
                }
            }
            else if (strncmp(cmdParamLine, sclsvrFORMAT_TAG, 
                             strlen(sclsvrFORMAT_TAG)) == 0)
            {
                // Remove request tag
                if (miscTrimString(cmdParamLine, sclsvrFORMAT_TAG) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }

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
 * @param scienceObject the science object
 * 
 * @return mcsSUCCESS if the science star had been found and had been updated,
 * mcsFAILURE otherwise.
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR_LIST::GetScienceObject(sclsvrCALIBRATOR &scienceObject)
{
    logTrace("sclsvrCALIBRATOR_LIST::GetScienceObject()");
   
    // Check if coordinates of the science star are present in order to be able
    // to compare
    if ((scienceObject.IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsFALSE) ||
        (scienceObject.IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsFALSE))
    {
        return mcsFAILURE;
    }

    const unsigned int nbStars = Size();
    
    // Logical flag to know if the object had been found in the list
    // At beginning, the objet is not found
    mcsLOGICAL isScienceObjectFound = mcsFALSE;
   
    sclsvrCALIBRATOR* calibrator;
    // For each star of the list, check if the coordinates are the same as the
    // given science object coordinates.
    for (unsigned int el = 0; el < nbStars; el++)
    {
        calibrator = (sclsvrCALIBRATOR*)GetNextStar((mcsLOGICAL)(el==0));

        // If the next star of the list is the same that the science object
        if (scienceObject.IsSame(calibrator) == mcsTRUE)
        {
            // Update value of the calibrator
            scienceObject.Update(*calibrator);

            // Changed flag as true
            isScienceObjectFound = mcsTRUE;            
            
            // fast return
            break;
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

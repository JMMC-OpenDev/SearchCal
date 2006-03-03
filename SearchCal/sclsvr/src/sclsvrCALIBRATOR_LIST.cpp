/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.52 2006-03-03 15:25:23 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.51  2005/11/14 14:48:13  lafrasse
 * Changed hard coded property id used to sort calibrator according their disctance to the science object to a definied constant
 *
 * Revision 1.50  2005/11/14 14:19:41  lafrasse
 * Added "distance to science object" computation and sorting
 *
 * Revision 1.49  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.48  2005/10/24 13:03:50  lafrasse
 * Refined code documentation
 *
 * Revision 1.47  2005/10/05 12:34:44  scetre
 * Update bad logTrace
 *
 * Revision 1.46  2005/06/20 14:43:25  scetre
 * Removed unused FilterBy... methods
 *
 * Revision 1.45  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.44  2005/03/30 12:50:58  scetre
 * Changed call to alx funtions.
 * Updated documentation
 *
 * Revision 1.43  2005/03/10 15:46:21  scetre
 * Simplified filter on visibility and variability
 *
 * Revision 1.42  2005/03/10 11:31:34  scetre
 * Removed all star with a flag of variability and multiplicity in filter by visibility and variability methods
 *
 * Revision 1.41  2005/03/09 07:30:39  scetre
 * Fixed bug in filter by visibility
 *
 * Revision 1.40  2005/03/07 16:06:06  gzins
 * Removed automatic sort on visibility
 *
 * Revision 1.39  2005/03/07 15:16:08  scetre
 * Changed Filter by visibility
 *
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
 * gzins     09-Dec-2004  Fixed cast problem with new mcsLOGICAL enumerate
 * scetre    15-Sep-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrCALIBRATOR_LIST class definition.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrCALIBRATOR_LIST.cpp,v 1.52 2006-03-03 15:25:23 scetre Exp $"; 


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

    // Put each star of the given vobsSTAR_LIST in the internal list
    for (unsigned int el = 0; el < list.Size(); el++)
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

    // For each calibrator of the given list
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        // Get next calibrator
        sclsvrCALIBRATOR *calibrator;
        calibrator = (sclsvrCALIBRATOR *)list.GetNextStar((mcsLOGICAL)(el==0));
        
        // Check wether this calibrator has to be copyied in or not
        mcsLOGICAL copyIt = mcsTRUE;
        if ((copyDiameterNok == mcsFALSE) && 
            (calibrator->IsDiameterOk() == mcsFALSE))
        {
            copyIt = mcsFALSE;
        }

        // If yes, copy it
        if (copyIt == mcsTRUE)
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
 * Extract a list inside another one.
 *
 * @param list the list to Extract in the current list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Extract(sclsvrCALIBRATOR_LIST &list)
{
    logTrace("sclsvrCALIBRATOR_LIST::Extract()");

    // For each element of the given list
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        // Get next calibrator
        sclsvrCALIBRATOR* calibratorToDelete;
        calibratorToDelete =
            (sclsvrCALIBRATOR *)list.GetNextStar((mcsLOGICAL)(el==0));
        
        // For each element of the internal list
        for (unsigned int elem = 0; elem < Size(); elem++)
        {
            // Get next calibrator
            sclsvrCALIBRATOR* calibrator;
            calibrator = 
                (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(elem==0));

            // if calibrator is in the list of deleting calibrator, remove it
            if (calibratorToDelete->IsSame(*calibrator))
            {
                Remove(*calibrator);
                break;
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
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(sclsvrCALIBRATOR &calibrator)
{
    logTrace("sclsvrCALIBRATOR_LIST::AddAtTail()");

    // Copy the given calibrator
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(calibrator);

    // Add the copy of the given calibrator in the list
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
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::AddAtTail(vobsSTAR &star)
{
    logTrace("sclsvrCALIBRATOR_LIST::AddAtTail()");
    
    // Copy the given star as a calibrator
    sclsvrCALIBRATOR *newCalibrator = new sclsvrCALIBRATOR(star);

    // Add the copy of the given star in the list
    _starList.push_back(newCalibrator);

    return mcsSUCCESS;
}

/**
 * Complete each calibrator of the list according to the user request.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR_LIST::Complete()");

    // For each calibrator of the list 
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

    // Sort calibrators according to their distance from the science object in
    // ascending order, i.e. the closest first.
    Sort(sclsvrCALIBRATOR_DIST);

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
 * Delete a calibrator according to its number in the list
 *
 * @param starNumber the number of the star to removed
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Delete(unsigned int starNumber)
{
    logTrace("sclsvrCALIBRATOR_LIST::Delete(%d)", starNumber);

    // If the number is negative or higher than the list size, return erro
    if ((starNumber < 1) || (starNumber > Size()))
    {
        // todo err
        return mcsFAILURE;
    }

    sclsvrCALIBRATOR *calibrator;    
    // Go to the star
    for (unsigned int el = 0; el <= starNumber-1; el++)
    {
        calibrator = (sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));
    }

    // Once the star is found, remove it from the list
    if (Remove(*calibrator) == mcsFAILURE)
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
                                          sclsvrREQUEST &request,
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
                                          sclsvrREQUEST &request,
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
    
    // Logical flag to know if the object had been found in the list
    // At beginning, the objet is not found
    mcsLOGICAL isScienceObjectFound = mcsFALSE;
   
    sclsvrCALIBRATOR *calibrator;
    // For each star of the list, check if the coordinates are the same as the
    // given science object coordinates.
    for (unsigned int el = 0; el < Size(); el++)
    {
        calibrator=(sclsvrCALIBRATOR *)GetNextStar((mcsLOGICAL)(el==0));

        // If the next star of the list is the same that the science object
        if (scienceObject.IsSame(*calibrator) == mcsTRUE)
        {
            // Update value of the calibrator
            if (scienceObject.Update(*calibrator) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Changed flag as true
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

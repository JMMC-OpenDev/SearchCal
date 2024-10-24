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
sclsvrCALIBRATOR_LIST::sclsvrCALIBRATOR_LIST(const char* name) : vobsSTAR_LIST(name)
{
}

/**
 * Class destructor
 */
sclsvrCALIBRATOR_LIST::~sclsvrCALIBRATOR_LIST()
{
}

/**
 * Convert stars to calibrators and clear the given star list
 * i.e. add all calibrators created from stars present
 * in the given list at the end of this list
 *
 * @param list the list to copy and clear
 */
void sclsvrCALIBRATOR_LIST::Move(vobsSTAR_LIST& list)
{
    // vobsSTAR_LIST does not own vobsSTAR instances anymore
    // to avoid double-free problems:
    list.SetFreeStarPointers(false);

    vobsSTAR* starPtr;

    const mcsUINT32 nbStars = list.Size();
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        // Get a vobsSTAR from the input vobsSTAR_LIST
        starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

        // create a new sclsvrCALIBRATOR from the given vobsSTAR
        AddAtTail(*starPtr);

        // Delete the vobsSTAR ASAP to avoid wasting memory
        delete(starPtr);
    }

    // Clear the input list
    list.Clear();
}

/**
 * Copy from a list
 * i.e. Add all elements present in the given list at the end of this list
 *
 * @param list the list to copy
 */
void sclsvrCALIBRATOR_LIST::Copy(const vobsSTAR_LIST& list)
{
    const mcsUINT32 nbStars = list.Size();
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        AddAtTail(*(list.GetNextStar((mcsLOGICAL) (el == 0))));
    }
}

/**
 * Adds the given calibrator at the end of the list.
 *
 * @param calibrator calibrator to be added to the list.
 */
void sclsvrCALIBRATOR_LIST::AddAtTail(const sclsvrCALIBRATOR &calibrator)
{
    // Copy the given calibrator
    sclsvrCALIBRATOR* newCalibrator = new sclsvrCALIBRATOR(calibrator);

    // Add one pointer of the calibrator in the list
    _starList.push_back(newCalibrator);
}

/**
 * Adds the given star as a calibrator at the end of the list.
 *
 * @param star star to be added to the list.
 */
void sclsvrCALIBRATOR_LIST::AddAtTail(const vobsSTAR &star)
{
    // Copy the given star as a calibrator
    sclsvrCALIBRATOR* newCalibrator = new sclsvrCALIBRATOR(star);

    // Add one pointer of the calibrator in the list
    _starList.push_back(newCalibrator);
}

/**
 * Complete each calibrator of the list according to the user request.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Complete(const sclsvrREQUEST &request)
{
    const mcsUINT32 nbStars = Size();

    logTest("Complete: start [%d stars]", nbStars);

    // Prepare information buffer:
    miscoDYN_BUF infoMsg;
    infoMsg.Reserve(1024);

    sclsvrCALIBRATOR* calibrator;

    // For each calibrator of the list
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        // Complete the calibrator
        calibrator = (sclsvrCALIBRATOR*) GetNextStar((mcsLOGICAL) (el == 0));

        FAIL(calibrator->Complete(request, infoMsg));

        // Check cancellation every 100 stars:
        if (el % 100 == 0)
        {
            FAIL_COND(vobsIsCancelled());
        }
    }

    if (IS_FALSE(request.IsJSDCMode()) && IS_TRUE(request.hasObjectRaDec()))
    {
        // Sort calibrators according to their distance from the science object in
        // ascending order, i.e. the closest first.
        Sort(sclsvrCALIBRATOR_DIST);
    }

    logTest("Complete: done [%d stars]", nbStars);

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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Pack(miscoDYN_BUF *buffer)
{
    vobsCDATA cData;
    sclsvrCALIBRATOR calibrator;

    // In unpack method, extended logical is true
    FAIL(cData.Store(calibrator, *this, mcsTRUE));

    buffer->AppendString(cData.GetBuffer());

    return mcsSUCCESS;
}

/**
 * De-serialize a calibrator list from.
 *
 * @param buffer the dynamic buffer in which the serialized list is stored
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::UnPack(const char *buffer)
{
    // create a cdata object and put the content of the buffer in it
    vobsCDATA cData;
    FAIL(cData.LoadBuffer(buffer));

    sclsvrCALIBRATOR calibrator;
    FAIL(cData.Extract(calibrator, *this, mcsTRUE));

    return mcsSUCCESS;
}

/**
 * Save the calibrator list in a file.
 *
 * If given, the request is placed in the file as a comment line.
 *
 * @param filename the file in which the calibrator list should be saved
 * @param request request used to get this list
 * @param extendedFormat if mcsTRUE, each property is saved with its attributes
 * (origin and confidence index), otherwise only the property is saved.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Save(const char *filename,
                                          const sclsvrREQUEST &request,
                                          mcsLOGICAL extendedFormat)
{
    // Get creation date and SW version
    mcsSTRING32 utcTime;
    FAIL(miscGetUtcTimeStr(0, &utcTime));

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
    mcsSTRING16384 cmdParamLine;
    FAIL(request.GetCmdParamLine(&cmdParamLine));

    if (strlen(cmdParamLine) != 0)
    {
        cData.AppendString(sclsvrREQUEST_TAG);
        cData.AppendString(cmdParamLine);
        cData.AppendString("\n");
    }

    // Add format : STANDARD or EXTENDED
    cData.AppendString(sclsvrFORMAT_TAG);
    if (IS_TRUE(extendedFormat))
    {
        cData.AppendString("EXTENDED");
    }
    else
    {
        cData.AppendString("STANDARD");
    }
    cData.AppendString("\n");

    // Store list into the CDATA
    sclsvrCALIBRATOR calibrator;
    FAIL(cData.Store(calibrator, *this, extendedFormat));

    // Save into file
    FAIL(cData.SaveInFile(filename));

    return mcsSUCCESS;
}

/**
 * Load a calibrator list and a request from a file.
 *
 * @param filename name of file containing the calibrator list
 * @param request request object loaded from file
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::Load(const char* filename,
                                          sclsvrREQUEST &request)
{
    // File format; by default standard format is assumed
    mcsLOGICAL extendedFormat = mcsFALSE;

    // Load file
    vobsCDATA cData;
    FAIL(cData.LoadFile(filename));

    // Look for request and format in comment lines
    const char *from = NULL;
    mcsSTRING1024 cmdParamLine;
    do
    {
        from = cData.GetNextCommentLine(from, cmdParamLine,
                                        sizeof (cmdParamLine));
        if (IS_NOT_NULL(from))
        {
            if (strncmp(cmdParamLine, sclsvrREQUEST_TAG, strlen(sclsvrREQUEST_TAG)) == 0)
            {
                // Remove request tag
                miscTrimString(cmdParamLine, sclsvrREQUEST_TAG);

                // Parse the found request
                FAIL_DO(request.Parse(cmdParamLine), errAdd(sclsvrERR_REQUEST_LINE_FORMAT, filename, cmdParamLine));
            }
            else if (strncmp(cmdParamLine, sclsvrFORMAT_TAG, strlen(sclsvrFORMAT_TAG)) == 0)
            {
                // Remove format tag
                miscTrimString(cmdParamLine, sclsvrFORMAT_TAG);

                // Parse the found format
                if (strcmp(cmdParamLine, "EXTENDED") == 0)
                {
                    extendedFormat = mcsTRUE;
                }
            }
        }
    }
    while (IS_NOT_NULL(from));

    // Extract list from the CDATA
    sclsvrCALIBRATOR calibrator;
    FAIL(cData.Extract(calibrator, *this, extendedFormat));

    return mcsSUCCESS;
}

/**
 * Return this list serialized in the Tab-Separated Value format
 * 
 * @param header header of the file
 * @param softwareVersion software version
 * @param request user request
 * @param buffer the output buffer
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetTSV(const char* header,
                                            const char* softwareVersion,
                                            const char* request,
                                            miscoDYN_BUF *buffer)
{
    // Get creation date and SW version
    mcsSTRING32 utcTime;
    FAIL(miscGetUtcTimeStr(0, &utcTime));

    mcsSTRING256 line;
    vobsCDATA cData;
    cData.AppendString("# JMMC - Calibrator group\n");
    cData.AppendString("#\n#  ");
    cData.AppendString(header);
    cData.AppendString("\n#  Request parameters: ");
    cData.AppendString(request);
    cData.AppendString("\n#  Server version    : ");
    cData.AppendString(softwareVersion);
    sprintf(line,      "\n#  Generated on (UTC): %s\n", utcTime);
    cData.AppendString(line);
    cData.AppendString("#\n");

    sclsvrCALIBRATOR calibrator;

    // TODO: skip ID line + use numeric formatter
    FAIL(cData.Store(calibrator, *this, mcsFALSE, mcsTRUE));

    buffer->AppendString(cData.GetBuffer());

    return mcsSUCCESS;
}

mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::SaveTSV(const char *filename,
                                             const char* header,
                                             const char* softwareVersion,
                                             const char* request)
{
    miscoDYN_BUF buffer;

    // Get the star list in the TSV format
    FAIL(GetTSV(header, softwareVersion, request, &buffer));

    logInfo("Saving TSV: %s", filename);

    // Try to save the generated VOTable in the specified file as ASCII
    return (buffer.SaveInASCIIFile(filename));
}

/**
 * Get Science object if it is present in the list.
 *
 * This method complete the calibrator which should have been built before. It
 * should have as the minimum of properties, the value of his right ascension
 * and declination position. If one of this properties is not present in the
 * list, an error is returned.
 *
 * @param scienceObject the science object
 *
 * @return mcsSUCCESS if the science star had been found and had been updated,
 * mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::GetScienceObject(sclsvrCALIBRATOR &scienceObject) const
{
    // Check if coordinates of the science star are present in order to be able
    // to compare
    FAIL_COND(IS_FALSE(scienceObject.IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN)) ||
              IS_FALSE(scienceObject.IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN)));

    const mcsUINT32 nbStars = Size();

    // Logical flag to know if the object had been found in the list
    // At beginning, the objet is not found
    mcsLOGICAL isScienceObjectFound = mcsFALSE;

    sclsvrCALIBRATOR* calibrator;
    // For each star of the list, check if the coordinates are the same as the
    // given science object coordinates.
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        calibrator = (sclsvrCALIBRATOR*) GetNextStar((mcsLOGICAL) (el == 0));

        // If the next star of the list is the same that the science object
        if (IS_TRUE(scienceObject.IsSame(calibrator)))
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
    FAIL_FALSE(isScienceObjectFound);

    // If it has been found, return success
    return mcsSUCCESS;
}

/**
 * Filter this list to keep only valid diameters
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR_LIST::FilterDiameterOk()
{
    logTest("FilterDiameterOk: start [%d stars]", Size());

    // Traverse and remove star pointers in the same loop (efficiency):
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); )
    {
        // check if diameter is set and ok:
        if (IS_FALSE(((sclsvrCALIBRATOR*) * iter)->IsDiameterOk()))
        {
            if (IsFreeStarPointers())
            {
                // Delete star
                delete(*iter);
            }

            // Clear star from list
            iter = _starList.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    logTest("FilterDiameterOk: done [%d stars]", Size());

    return mcsSUCCESS;
}


/*___oOo___*/

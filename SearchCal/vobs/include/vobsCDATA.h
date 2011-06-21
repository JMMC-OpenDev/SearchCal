#ifndef vobsCDATA_H
#define vobsCDATA_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCDATA class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>
#include <string.h>
#include <stdio.h>
/*
 * MCS header files
 */
#include "log.h"
#include "mcs.h"
#include "misc.h"
#include "misco.h"

/*
 * Locale header files
 */
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */
/**
 * vobsCDATA represent the CDATA part of the resulting xml files coming from
 * the CDS.
 *
 * It is build with several part :
 * \li a list of UCD name.
 * \li a list of parameter name.
 * \li a number of line to skip in order to get the information data in the
 * CDATA block
 * \li the body of the CDATA
 * \li the number of line stored in the CDATA
 **/
class vobsCDATA : public miscoDYN_BUF
{

public:
    // Constructor
    vobsCDATA();

    // Destructor
    virtual ~vobsCDATA();

    virtual mcsCOMPL_STAT SetCatalogName(const char *name);
    virtual const char   *GetCatalogName(void);

    virtual mcsCOMPL_STAT ParseParamsAndUCDsNamesLines(char *paramNameLine, char *ucdNameLine);
    virtual mcsCOMPL_STAT AddParamName(const char *paramName);
    virtual mcsCOMPL_STAT AddUcdName(const char *ucdName);

    virtual mcsUINT32     GetNbParams(void);
    virtual mcsCOMPL_STAT GetNextParamDesc(char **paramName,
                                           char **ucdName,
                                           mcsLOGICAL init);
    virtual mcsCOMPL_STAT SetNbLinesToSkip(mcsINT32 nbLines);
    virtual mcsUINT32     GetNbLinesToSkip(void);
    virtual mcsCOMPL_STAT AppendLines(char *buffer, mcsINT32 nbLinesToSkip);
    virtual mcsUINT32     GetNbLines(void);

    virtual mcsCOMPL_STAT LoadFile(const char *fileName); 
    virtual mcsCOMPL_STAT LoadBuffer(const char *buffer); 

    /**
     * Store a star list into the CDATA object
     *
     * It stores all the star properties into the internal buffer. The star
     * properties are separated by tabs with one line per star.
     *
     * \param object object contained in the list
     * \param objectList list of objects
     * \param ucdList list of UCD
     * \param extendedFormat if true, each property is stored with its
     * attributes (origin and confidence index), otherwise only only property is
     * stored.
     *
     * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
     * returned. 
     */
    template <class obj, class list>
    mcsCOMPL_STAT Store(obj                       &object,
                        list                      &objectList,
                        vobsSTAR_PROPERTY_ID_LIST  ucdList, 
                        mcsLOGICAL                 extendedFormat = mcsFALSE)
    {
        logPrint("vobs", logTRACE, __FILE_LINE__, "vobsCDATA::Store()");

        vobsSTAR_PROPERTY_ID_LIST propertyIDList;

        // If the list is empty, then store all star properties
        if (ucdList.size() != 0)
        {
            propertyIDList = ucdList;
        }
        else
        {
            obj star;
            for (mcsINT32 propertyIndex = 0; propertyIndex < star.NbProperties(); propertyIndex++)
            {
                vobsSTAR_PROPERTY *property;
                property = star.GetNextProperty((mcsLOGICAL)(propertyIndex == 0));
                propertyIDList.push_back((char *)property->GetId());
            }
        }
        
        vobsSTAR_PROPERTY_ID_LIST::iterator propertyIDIterateur;
        obj star;
        // Write each property Id corresponding with the ucd into the buffer
        propertyIDIterateur = propertyIDList.begin();
        while (propertyIDIterateur != propertyIDList.end())
        {
            vobsSTAR_PROPERTY *property;
            property = star.GetProperty((char *)(*propertyIDIterateur).data());
            AppendString(property->GetId());
            AppendString("\t");
            if (extendedFormat == mcsTRUE)
            {
                AppendString("\t\t");
            }
            AddUcdName(property->GetId());
            propertyIDIterateur++;
        }
        AppendString("\n");

        // Write each property name corresponding with the ucd into the buffer
        propertyIDIterateur = propertyIDList.begin();
        while (propertyIDIterateur != propertyIDList.end())
        {
            vobsSTAR_PROPERTY *property;
            property = star.GetProperty((char *)(*propertyIDIterateur).data());
            AppendString(property->GetName());
            AppendString("\t");
            if (extendedFormat == mcsTRUE)
            {
                AppendString("\t\t");
            }
            AddParamName(property->GetName());
            propertyIDIterateur++;
        }
        AppendString("\n");

        // For each object of the list    
        obj *starPtr;
        mcsUINT32 starIdx;
        for (starIdx = 0; starIdx < objectList.Size(); starIdx++)
        {
            // Get each object of the list
            starPtr = (obj *)objectList.GetNextStar((mcsLOGICAL)
                                                    (starIdx==0));
            // For each property of the object
            propertyIDIterateur = propertyIDList.begin();
            while (propertyIDIterateur != propertyIDList.end())
            {
                // Get each property
                vobsSTAR_PROPERTY *property;
                property =
                    starPtr->GetProperty((char *)(*propertyIDIterateur).data());  
                
                // Each star property is placed in buffer in form :
                // 'value \t origin \t confidenceIndex'
                AppendString(property->GetValue());
                AppendString("\t");
                if (extendedFormat == mcsTRUE)
                {
                    AppendString(property->GetOrigin());
                    AppendString("\t");
                    mcsSTRING16 confidenceIndex;
                    sprintf(confidenceIndex, "%d", property->GetConfidenceIndex());
                    AppendString(confidenceIndex);
                    AppendString("\t");
                }
                propertyIDIterateur++;
            }

            // If it's not the last star of the list
            if (starIdx != (objectList.Size() - 1))
            {
                // Go to the next line
                AppendString("\n");
            }
        }

        // Removed the '\0' character at the end of the buffer
        _dynBuf.storedBytes -= 1;

        return mcsSUCCESS;
    }


    /**
     * Parse the CDATA section.
     *
     * This method parses the CDATA section defined by the parameter
     * description. The CDATA section contains a list of stars which are stored
     * in a CSV table where each line corresponds to a star; i.e. all star
     * properties are placed on one line, and separated by the '\\t' character.
     * The list of star properties to be extracted is given by \em ucd.ucdName
     * list.
     *
     * The first lines of the CDATA containing the description of the parameters
     * has to be skipped. The number of lines to be skipped is given by \em
     * _nbLinesToSkip 
     *
     * The found stars are put in the \em starList parameter. 
     * 
     * \param object type of object contained in the list (polymophism).
     * \param objectList list where extracted stars should be put.
     * \param extendedFormat if true, each property is stored with its attributes
     * (origin and confidence index), otherwise only the value is stored (e.g
     * when loading the local catalog).
     * 
     * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    template <class obj, class list>
    mcsCOMPL_STAT Extract(obj &object, list &objectList,
                          mcsLOGICAL extendedFormat = mcsFALSE)
    {
        logPrint("vobs", logTRACE, __FILE_LINE__, "vobsCDATA::Extract()");

        // For each line in the internal buffer, get the value for each defined
        // UCD (values are separated by '\t' characters), store them in object,
        // then add this new object to the given list.
        const char*   from = NULL;
        mcsSTRING2048 line;
        mcsUINT32     maxLineLength = sizeof(line);
        mcsINT32      nbOfLine = 0;
        do
        {
            // Get next line of the internal buffer
            from = GetNextLine(from, line, maxLineLength);
            nbOfLine++;

            logPrint("vobs", logDEBUG, __FILE_LINE__, "Next line = %s\n", line);

            if ((nbOfLine > _nbLinesToSkip) &&  (from != NULL) && 
                (miscIsSpaceStr(line) != mcsTRUE))
            {
                mcsSTRING256 lineSubStrings[1024];
                mcsUINT32    nbOfSubStrings;
                char        *paramName;
                char        *ucdName;
                const char  *ucdValue;
                const char  *propertyID;
                const char  *origin;
                vobsCONFIDENCE_INDEX confidenceIndex;
                obj          object;

                // Split line on '\t' character, and store each token
                if (miscSplitString(line, '\t', lineSubStrings, 
                                    1024, &nbOfSubStrings) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // Remove each token trailing and leading blanks
                for (mcsUINT32 i = 0; i < nbOfSubStrings; i++)
                {
                    miscTrimString(lineSubStrings[i], " ");
                }

                // Determine the number of attributes per property
                mcsUINT32 nbOfAttributesPerProperty = 1;
                // If extended format then nb attributes per properties is
                // 3 (value, origin, confidence index) else 1 (value only)
                if (extendedFormat == mcsTRUE)
                {
                    nbOfAttributesPerProperty = 3;
                }
                    
                // Number of UCDs per line
                mcsUINT32 nbOfUCDSPerLine = GetNbParams();

                // Temporary variables to parse in case of II/225
                mcsSTRING256 wavelength = "";
                strcpy(wavelength, "");
                mcsSTRING256 flux = "";
                strcpy(flux, "");

                for (mcsUINT32 propertyIndex = 0; propertyIndex < nbOfUCDSPerLine; propertyIndex++)
                {
                    // Get the parameter name and UCD
                    if (GetNextParamDesc(&paramName, &ucdName,(mcsLOGICAL)(propertyIndex == 0)) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    logPrint("vobs", logDEBUG, __FILE_LINE__,
                             "\tExtracting '%s' parameter (UCD = '%s') :", paramName, ucdName);

                    // Get the UCD value
                    mcsUINT32 realIndex = propertyIndex * nbOfAttributesPerProperty;
                    if (realIndex < nbOfSubStrings)
                    {
                        // Value is the first token
                        ucdValue = lineSubStrings[realIndex];

                        if (extendedFormat == mcsTRUE)
                        {
                            // Origin is the second token
                            origin = lineSubStrings[realIndex + 1];

                            // Confidence is the third token
                            int confidenceValue;
                            sscanf(lineSubStrings[realIndex + 2], "%d", &confidenceValue);

                            confidenceIndex = (vobsCONFIDENCE_INDEX) confidenceValue;
                        }
                        else // In local catalog case
                        {
                            // Load the properties with the global catalog name as origin
                            origin = GetCatalogName();
                            confidenceIndex = vobsCONFIDENCE_HIGH;
                        }
                        const char* confidence = (confidenceIndex == vobsCONFIDENCE_LOW ? "LOW" : (confidenceIndex == vobsCONFIDENCE_MEDIUM ? "MEDIUM" : "HIGH"));
                        logPrint("vobs", logDEBUG, __FILE_LINE__,
                                 "\t\tValue = '%s'; Origin = '%s'; Confidence = '%s'.", ucdValue, origin, confidence);
                    }
                    else 
                    {
                        // End of line reached : stop UCD scan and skip to next line
                        logPrint("vobs", logDEBUG, __FILE_LINE__,
                                 "\t\tNO VALUE FOUND.");
                        break;
                    }

                    // If UCD is not a known property ID
                    if (object.IsProperty(ucdName) == mcsFALSE)
                    {
                        // Check if UCD and parameter association correspond to
                        // a known property
                        propertyID = GetPropertyId(paramName, ucdName);
                        logPrint("vobs", logDEBUG, __FILE_LINE__,
                                 "\t\tUCD '%s' is NOT a known property ID, using '%s' property ID instead.", ucdName, propertyID);
                    }
                    else
                    {
                        // Property ID is the UCD
                        propertyID = ucdName;
                        logPrint("vobs", logDEBUG, __FILE_LINE__,
                                 "\t\tUCD '%s' is a known property ID.", ucdName, propertyID);
                    }

                    // If it is a known property
                    if (propertyID != NULL)
                    {
                        // Specific treatement of the flux
                        // If wavelength is found, save it
                        if (strcmp(propertyID, vobsSTAR_INST_WAVELENGTH_VALUE) == 0)
                        {
                            strcpy(wavelength, ucdValue); 
                        }
                        // If flux is found, save it
                        else if (strcmp(propertyID, vobsSTAR_PHOT_FLUX_IR_MISC) == 0)
                        {
                            strcpy(flux, ucdValue);
                        }
                        else
                        {
                            // Check if extracted value is empty
                            if (miscIsSpaceStr(ucdValue) == mcsTRUE)
                            {
                                // Use blanking value
                                ucdValue = vobsSTAR_PROP_NOT_SET;
                            }

                            // Set object property with extracted values
                            if (object.SetPropertyValue(propertyID, ucdValue, origin, confidenceIndex) == mcsFAILURE)
                            {
                                return mcsFAILURE;
                            }
                        }
                    }

                    // If wavelength and flux have been found, find the 
                    // corresponding magnitude band
                    if ((strcmp(wavelength, "") != 0) && (strcmp(flux, "") != 0))
                    {
                        // Get the wavelength value 
                        mcsDOUBLE lambdaValue = -1.0;
                        if (sscanf(wavelength, "%lf" , &lambdaValue) == 1)
                        {
                            const char* magnitudeBand = NULL;

                            // Determnine to corresponding magnitude
                            if ((lambdaValue >= (mcsDOUBLE)1.24) ||
                                (lambdaValue <= (mcsDOUBLE)1.26))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_J;
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)1.64) ||
                                     (lambdaValue <= (mcsDOUBLE)1.66))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_H;
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)2.19) ||
                                     (lambdaValue <= (mcsDOUBLE)2.21))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_K;
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)3.49) ||
                                     (lambdaValue <= (mcsDOUBLE)3.51))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_L;
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)4.99) ||
                                     (lambdaValue <= (mcsDOUBLE)5.01))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_M;
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)9.99) ||
                                     (lambdaValue <= (mcsDOUBLE)10.01))
                            {
                                magnitudeBand = vobsSTAR_PHOT_JHN_N;
                            }

                            // If the given flux correspond to an expected
                            // magnitude
                            if (magnitudeBand != NULL)
                            {
                                logPrint("vobs", logDEBUG, __FILE_LINE__,
                                         "\t\tFlux = '%s' and wavelength = '%s' --> magnitude band = '%s'",
                                         flux, wavelength, magnitudeBand);

                                // Set object property with extracted values
                                object.SetPropertyValue(magnitudeBand, flux, origin); 
                            }
                        }
                    }
                }

                if (logGetStdoutLogLevel() >= logDEBUG)
                {
                    object.Display(mcsTRUE);
                }

                // Store the object in the list
                if (objectList.AddAtTail(object) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
        } while (from != NULL);

        return mcsSUCCESS;
    }

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCDATA(const vobsCDATA&);
    vobsCDATA& operator=(const vobsCDATA&);
    
    mcsCOMPL_STAT LoadParamsAndUCDsNamesLines(void);
    const char *GetPropertyId(const char *paramName, const char *ucdName);
    
    std::vector<char*> _paramName; // Name of parameters
    std::vector<char*> _ucdName; // Name of corresponding UCD
    std::vector<char *>::iterator _paramNameIterator;
    std::vector<char *>::iterator _ucdNameIterator;

    int _nbLinesToSkip;          // Number of lines to be skipped in CDATA
                                 // section
    int _nbLines;                // Number of lines stored in buffer

    string _catalogName;         // Catalog name from where CDATA comming from 
};

#endif /*!vobsCDATA_H*/

/*___oOo___*/

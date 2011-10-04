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
#include "vobsCATALOG.h"
#include "vobsSTAR_LIST.h"

/* Define module id for logs */
#ifndef MODULE_ID
#define MODULE_ID_HACK
#define MODULE_ID "vobs"
#endif

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

    virtual mcsCOMPL_STAT SetCatalogName(const char* name);
    virtual const char*   GetCatalogName(void);

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
    template <class Star, class list>
    mcsCOMPL_STAT Store(Star                       &object,
                        list                       &objectList,
                        vobsSTAR_PROPERTY_ID_LIST  ucdList, 
                        mcsLOGICAL                 extendedFormat = mcsFALSE)
    {
        logTrace("vobsCDATA::Store()");

        vobsSTAR_PROPERTY_ID_LIST propertyIDList;
        vobsSTAR_PROPERTY* property;

        // If the list is empty, then store all star properties
        if (ucdList.size() != 0)
        {
            propertyIDList = ucdList;
        }
        else
        {
            Star star;
            for (mcsINT32 propertyIndex = 0, len = star.NbProperties(); propertyIndex < len; propertyIndex++)
            {
                property = star.GetNextProperty((mcsLOGICAL)(propertyIndex == 0));
                
                propertyIDList.push_back(property->GetId());
            }
        }
        
        vobsSTAR_PROPERTY_ID_LIST::iterator propertyIDIterator;
        Star star;
        // Write each property Id corresponding with the ucd into the buffer
        propertyIDIterator = propertyIDList.begin();
        while (propertyIDIterator != propertyIDList.end())
        {
            property = star.GetProperty(*propertyIDIterator);
            
            AppendString(property->GetId());
            AppendString("\t");
            
            if (extendedFormat == mcsTRUE)
            {
                AppendString("\t\t");
            }
            AddUcdName(property->GetId());
            propertyIDIterator++;
        }
        AppendString("\n");

        // Write each property name corresponding with the ucd into the buffer
        propertyIDIterator = propertyIDList.begin();
        while (propertyIDIterator != propertyIDList.end())
        {
            property = star.GetProperty(*propertyIDIterator);
            
            AppendString(property->GetName());
            AppendString("\t");
            
            if (extendedFormat == mcsTRUE)
            {
                AppendString("\t\t");
            }
            AddParamName(property->GetName());
            propertyIDIterator++;
        }
        AppendString("\n");

        mcsUINT32 nbStars = objectList.Size();
        
        // For each object of the list
        Star *starPtr;
        for (mcsUINT32 starIdx = 0; starIdx < nbStars; starIdx++)
        {
            // Get each object of the list
            starPtr = (Star*)objectList.GetNextStar((mcsLOGICAL) (starIdx==0));
            
            // For each property of the object
            propertyIDIterator = propertyIDList.begin();
            while (propertyIDIterator != propertyIDList.end())
            {
                // Get each property
                property = starPtr->GetProperty(*propertyIDIterator);  
                
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
                propertyIDIterator++;
            }

            // If it's not the last star of the list
            if (starIdx != (nbStars - 1))
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
    template <class Star, class list>
    mcsCOMPL_STAT Extract(Star& object, list &objectList,
                          mcsLOGICAL extendedFormat = mcsFALSE)
    {
        logTrace("vobsCDATA::Extract()");
         
        const bool isLogTest  = (logIsStdoutLogLevel(logTEST) == mcsTRUE);
        const bool isLogDebug = (logIsStdoutLogLevel(logDEBUG) == mcsTRUE);
        const bool isLogTrace = (logIsStdoutLogLevel(logTRACE) == mcsTRUE);
        
        // Number of UCDs per line
        mcsUINT32 nbOfUCDSPerLine = GetNbParams();

        // Determine the number of attributes per property
        mcsUINT32 nbOfAttributesPerProperty = 1;
        // If extended format then nb attributes per properties is
        // 3 (value, origin, confidence index) else 1 (value only)
        if (extendedFormat == mcsTRUE)
        {
            nbOfAttributesPerProperty = 3;
        }
        
        // Find matching Param/UCD in star properties:
        vobsSTAR_PROPERTY* property;
        // special case of catalog II/225 (photometric catalog JP11)
        bool               isWaveLength;
        bool               isFlux;
        // global flag indicating special case (wavelength or flux)
        bool               isWaveLengthOrFlux = false;
        
        // star properties:
        vobsSTAR_PROPERTY* properties[nbOfUCDSPerLine];
        bool               propIsWaveLength[nbOfUCDSPerLine];
        bool               propIsFlux[nbOfUCDSPerLine];

        if (isLogTest)
        {
            logTest("Extract: Property / Parameter mapping for catalog '%s':", GetCatalogName());
        }
        
        for (mcsUINT32 el = 0; el < nbOfUCDSPerLine; el++)
        {
            char* paramName;
            char* ucdName;
            
            // Get the parameter name and UCD
            if (GetNextParamDesc(&paramName, &ucdName,(mcsLOGICAL)(el == 0)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (isLogDebug)
            {
                logDebug("Extracting parameter '%s' (UCD = '%s') :", paramName, ucdName);
            }
            
            const char* propertyID = NULL;

            // If UCD is not a known property ID
            if (object.IsProperty(ucdName) == mcsFALSE)
            {
                // Check if UCD and parameter association correspond to
                // a known property
                propertyID = GetPropertyId(paramName, ucdName);
                if (isLogDebug)
                {
                    logDebug("\tUCD '%s' is NOT a known property ID, using '%s' property ID instead.", ucdName, propertyID);
                }
            }
            else
            {
                // Property ID is the UCD
                propertyID = ucdName;
                if (isLogDebug)
                {
                    logDebug("\tUCD '%s' is a known property ID.", ucdName, propertyID);
                }
            }
        
            // reset first:
            property     = NULL;
            isWaveLength = false;
            isFlux       = false;
            
            if (propertyID != NULL)
            {
                property = object.GetProperty(propertyID);

                isWaveLength = (strcmp(propertyID, vobsSTAR_INST_WAVELENGTH_VALUE) == 0);
                isFlux       = (strcmp(propertyID, vobsSTAR_PHOT_FLUX_IR_MISC) == 0);
                
                if (isWaveLength || isFlux)
                {
                    isWaveLengthOrFlux = true;
                }
            } 
            
            if (property == NULL)
            {
                logWarning("\tNo property found for parameter '%s' (UCD = '%s') in catalog '%s'", 
                                paramName, ucdName, GetCatalogName());
            }
            else 
            {
                if (isLogTest)
                {
                    logTest("Extract: Property '%s' [%s] found for parameter '%s' (UCD = '%s')", 
                                property->GetName(), property->GetId(), ucdName, propertyID);
                }
            }

            // memorize star property because star is one single instance so 
            // vobsSTAR_PROPERTY* is constant during the main loop:
            properties[el] = property;
            
            // memorize wavelength/flux flags:
            propIsWaveLength[el] = isWaveLength;
            propIsFlux[el]       = isFlux;
        }

        // Get flux properties in the johnson order (J,H,K,L,M,N)
        vobsSTAR_PROPERTY* fluxProperties[6];
        
        if (isWaveLengthOrFlux)
        {
            // get flux properties for special case of catalog II/225 (photometric catalog JP11)
            fluxProperties[0] = object.GetProperty(vobsSTAR_PHOT_JHN_J);
            fluxProperties[1] = object.GetProperty(vobsSTAR_PHOT_JHN_H);
            fluxProperties[2] = object.GetProperty(vobsSTAR_PHOT_JHN_K);
            fluxProperties[3] = object.GetProperty(vobsSTAR_PHOT_JHN_L);
            fluxProperties[4] = object.GetProperty(vobsSTAR_PHOT_JHN_M);
            fluxProperties[5] = object.GetProperty(vobsSTAR_PHOT_JHN_N);
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                fluxProperties[i] = NULL;
            }
        }

        const char*   from = NULL;
        mcsSTRING2048 line;
        mcsUINT32     maxLineLength = sizeof(line);
        mcsINT32      nbOfLine = 0;
        mcsSTRING256  lineSubStrings[1024];
        mcsUINT32     nbOfSubStrings;
        const char*   ucdValue;
        const char*   origin;
        int           confidenceValue;
        vobsCONFIDENCE_INDEX confidenceIndex;
        mcsSTRING256  wavelength;
        mcsSTRING256  flux;
        mcsUINT32     i, el, realIndex;
        mcsDOUBLE     lambdaValue;
        
        // For each line in the internal buffer, get the value for each defined
        // UCD (values are separated by '\t' characters), store them in object,
        // then add this new object to the given list.
        do
        {
            // Get next line of the internal buffer
            from = GetNextLine(from, line, maxLineLength);
            nbOfLine++;

            if (isLogDebug)
            {
                logDebug("Extract: Next line = '%s'", line);
            }

            if ((nbOfLine > _nbLinesToSkip) &&  (from != NULL) && (miscIsSpaceStr(line) == mcsFALSE))
            {
                // Split line on '\t' character, and store each token
                if (miscSplitString(line, '\t', lineSubStrings, 1024, &nbOfSubStrings) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // Remove each token trailing and leading blanks
                for (i = 0; i < nbOfSubStrings; i++)
                {
                    miscTrimString(lineSubStrings[i], " ");
                }

                if (isWaveLengthOrFlux)
                {
                    // Temporary variables to parse in special case of catalog II/225 (photometric catalog JP11)
                    wavelength[0] = '\0';
                    flux[0]       = '\0';
                }

                // Clear completely star object:
                object.ClearValues();

                for (el = 0; el < nbOfUCDSPerLine; el++)
                {
                    // Get related property:
                    property = properties[el];
                    
                    if (property == NULL)
                    {
                        // skip this value:
                        continue;
                    }
                    
                    if (isLogDebug)
                    {
                        logDebug("Extract: property '%s' :", property->GetId());
                    }

                    // Get the UCD value
                    realIndex = el * nbOfAttributesPerProperty;
                    if (realIndex < nbOfSubStrings)
                    {
                        // Value is the first token
                        ucdValue = lineSubStrings[realIndex];

                        if (extendedFormat == mcsTRUE)
                        {
                            // Origin is the second token
                            origin = GetKnownOrigin(lineSubStrings[realIndex + 1]);

                            // Confidence is the third token
                            confidenceValue = 0;
                            sscanf(lineSubStrings[realIndex + 2], "%d", &confidenceValue);

                            confidenceIndex = (vobsCONFIDENCE_INDEX) confidenceValue;
                        }
                        else // In local catalog case
                        {
                            // Load the properties with the global catalog name as origin
                            origin = GetCatalogName();
                            confidenceIndex = vobsCONFIDENCE_HIGH;
                        }
                        if (isLogDebug)
                        {
                            logDebug("\tValue = '%s'; Origin = '%s'; Confidence = '%s'.", ucdValue, origin, vobsGetConfidenceIndex(confidenceIndex));
                        }
                    }
                    else 
                    {
                        // End of line reached : stop UCD scan and skip to next line
                        if (isLogDebug)
                        {
                            logDebug("\tNO VALUE FOUND.");
                        }
                        break;
                    }

                    if (!isWaveLengthOrFlux)
                    {
                        // Check if extracted value is empty
                        if (miscIsSpaceStr(ucdValue) == mcsFALSE)
                        {
                            // Only set property if the extracted value is not empty
                            if (object.SetPropertyValue(property, ucdValue, origin, confidenceIndex) == mcsFAILURE)
                            {
                                return mcsFAILURE;
                            }
                        }
                        
                        // go to next parameter:
                        continue;
                    }

                    // special case of catalog II/225 (photometric catalog JP11)

                    isWaveLength = propIsWaveLength[el];
                    isFlux       = propIsFlux[el];
                    
                    // Specific treatement of the flux
                    // If wavelength is found, save it
                    if (isWaveLength)
                    {
                        strcpy(wavelength, ucdValue); 
                    }
                    // If flux is found, save it
                    else if (isFlux)
                    {
                        strcpy(flux, ucdValue);
                    }
                    else
                    {
                        // Check if extracted value is empty
                        if (miscIsSpaceStr(ucdValue) == mcsFALSE)
                        {
                            // Only set property if the extracted value is not empty
                            if (object.SetPropertyValue(property, ucdValue, origin, confidenceIndex) == mcsFAILURE)
                            {
                                return mcsFAILURE;
                            }
                        }
                        
                        // go to next parameter:
                        continue;
                    }

                    // If wavelength and flux have been found, find the 
                    // corresponding magnitude band
                    if ((wavelength[0] != '\0') && (flux[0] != '\0'))
                    {
                        // Get the wavelength value 
                        lambdaValue = -1.0;
                        if (sscanf(wavelength, "%lf" , &lambdaValue) == 1)
                        {
                            property = NULL;
                            
                            // Determine to corresponding magnitude
                            if ((lambdaValue >= (mcsDOUBLE)1.24) && (lambdaValue <= (mcsDOUBLE)1.26))
                            {
                                property = fluxProperties[0];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)1.64) && (lambdaValue <= (mcsDOUBLE)1.66))
                            {
                                property = fluxProperties[1];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)2.19) && (lambdaValue <= (mcsDOUBLE)2.21))
                            {
                                property = fluxProperties[2];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)3.49) && (lambdaValue <= (mcsDOUBLE)3.51))
                            {
                                property = fluxProperties[3];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)4.99) && (lambdaValue <= (mcsDOUBLE)5.01))
                            {
                                property = fluxProperties[4];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE)9.99) && (lambdaValue <= (mcsDOUBLE)10.01))
                            {
                                property = fluxProperties[5];
                            }

                            // If the given flux correspond to an expected magnitude
                            if (property != NULL)
                            {
                                if (isLogDebug)
                                {
                                    logDebug("\tFlux = '%s' and wavelength = '%s' --> magnitude band = '%s'",
                                                flux, wavelength, property->GetId());
                                }
                                
                                // Set object property with extracted values
                                object.SetPropertyValue(property, flux, origin); 
                            }
                        }
                        
                        // reset wavelength and flux:
                        wavelength[0] = '\0';
                        flux[0]       = '\0';
                    }
                    
                } // line parsing

                if (isLogTrace)
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
    const char* GetPropertyId(const char* paramName, const char* ucdName);
    
    std::vector<char*> _paramName; // Name of parameters
    std::vector<char*> _ucdName; // Name of corresponding UCD
    std::vector<char *>::iterator _paramNameIterator;
    std::vector<char *>::iterator _ucdNameIterator;

    int _nbLinesToSkip;          // Number of lines to be skipped in CDATA
                                 // section
    int _nbLines;                // Number of lines stored in buffer

    const char* _catalogName;    // Catalog name from where CDATA comming from 
    
    /**
     * Return one known origin for the given origin
     * @param origin origin value to look up
     * @return constant origin value or vobsSTAR_PROP_NOT_SET
     */
    inline static const char* GetKnownOrigin(char* origin) __attribute__((always_inline))
    {
        if (strcmp(origin, vobsCATALOG_AKARI_ID) == 0)
        {
            return vobsCATALOG_AKARI_ID;
        }
        if (strcmp(origin, vobsCATALOG_ASCC_ID) == 0)
        {
            return vobsCATALOG_ASCC_ID;
        }
        if (strcmp(origin, vobsCATALOG_ASCC_LOCAL_ID) == 0)
        {
            return vobsCATALOG_ASCC_LOCAL_ID;
        }
        if (strcmp(origin, vobsCATALOG_BSC_ID) == 0)
        {
            return vobsCATALOG_BSC_ID;
        }
        if (strcmp(origin, vobsCATALOG_CHARM2_ID) == 0)
        {
            return vobsCATALOG_CHARM2_ID;
        }
        if (strcmp(origin, vobsCATALOG_CIO_ID) == 0)
        {
            return vobsCATALOG_CIO_ID;
        }
        if (strcmp(origin, vobsCATALOG_DENIS_ID) == 0)
        {
            return vobsCATALOG_DENIS_ID;
        }
        if (strcmp(origin, vobsCATALOG_DENIS_JK_ID) == 0)
        {
            return vobsCATALOG_DENIS_JK_ID;
        }
        if (strcmp(origin, vobsCATALOG_HIC_ID) == 0)
        {
            return vobsCATALOG_HIC_ID;
        }
        if (strcmp(origin, vobsCATALOG_LBSI_ID) == 0)
        {
            return vobsCATALOG_LBSI_ID;
        }
        if (strcmp(origin, vobsCATALOG_MASS_ID) == 0)
        {
            return vobsCATALOG_MASS_ID;
        }
        if (strcmp(origin, vobsCATALOG_MERAND_ID) == 0)
        {
            return vobsCATALOG_MERAND_ID;
        }
        if (strcmp(origin, vobsCATALOG_MIDI_ID) == 0)
        {
            return vobsCATALOG_MIDI_ID;
        }
        if (strcmp(origin, vobsCATALOG_PHOTO_ID) == 0)
        {
            return vobsCATALOG_PHOTO_ID;
        }
        if (strcmp(origin, vobsCATALOG_SBSC_ID) == 0)
        {
            return vobsCATALOG_SBSC_ID;
        }
        if (strcmp(origin, vobsCATALOG_SB9_ID) == 0)
        {
            return vobsCATALOG_SB9_ID;
        }
        if (strcmp(origin, vobsCATALOG_UNSO_ID) == 0)
        {
            return vobsCATALOG_UNSO_ID;
        }
        if (strcmp(origin, vobsCATALOG_WDS_ID) == 0)
        {
            return vobsCATALOG_WDS_ID;
        }
        
        return vobsSTAR_PROP_NOT_SET;
    }
    
};

#ifdef MODULE_ID_HACK
#undef MODULE_ID
#endif


#endif /*!vobsCDATA_H*/

/*___oOo___*/

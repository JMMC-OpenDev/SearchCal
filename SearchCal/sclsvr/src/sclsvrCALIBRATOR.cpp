/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.20 2005-02-04 15:27:40 gzins Exp $"
 *
 * History
 * -------
 * scetre    14-Sep-2004  Created
 *
 *
 *******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR class definition.
 */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.20 2005-02-04 15:27:40 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>

using namespace std;
#include "gdome.h"

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "alx.h"

/*
 * Local Headers 
 */
#include "sclsvrCALIBRATOR.h"
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"


/**
 * Class constructor.
 *
 * Build a calibrator object.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR()
{
    // Add all star properties 
    AddProperties();
}

/*
 * Copy Constructor
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(vobsSTAR &star)
{
    (*this).vobsSTAR::operator=(star); 

    // Add all star properties 
    AddProperties(); 
}



/*
 * Class destructor
 */
sclsvrCALIBRATOR::~sclsvrCALIBRATOR()
{
}

/*
 * Public methods
 */

/**
 * Pack the calibrator in a dynamic buffer
 *
 * \param buffer buffer to complete
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Pack(miscDYN_BUF *buffer)
{
    logExtDbg("sclsvrCALIBRATOR::Pack()");
    // Copy properties of the star in th buffer
    for (_propertyOrderIterator = _propertyOrder.begin();
         _propertyOrderIterator != _propertyOrder.end();
         _propertyOrderIterator++)
    {
        miscDynBufAppendString(buffer,
                               (char *)_propertyList[(*_propertyOrderIterator).second].GetValue());
        miscDynBufAppendString(buffer, "\t");        
    }
    
    return mcsSUCCESS;
}

/**
 * Unpack the calibrator in a dynamic buffer
 *
 * \param calibratorString the calibrator to unpack
 *
 * \return
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::UnPack(char *calibratorString)
{ 
    logExtDbg("sclsvrCALIBRATOR::UnPack()");
    char *parsingString=NULL;

    // Initialise the buffer which will contain the calibrator
    miscDYN_BUF localBuffer;
    miscDynBufInit(&localBuffer);

    // Copy the calibrator in the local buffer
    miscDynBufAppendString(&localBuffer, calibratorString);

    // Parse the buffer
    parsingString = (char *) strtok(miscDynBufGetBuffer(&localBuffer), "\t");
    
    for (_propertyOrderIterator = _propertyOrder.begin();
         _propertyOrderIterator != _propertyOrder.end();
         _propertyOrderIterator++)
    {
        _propertyList[(*_propertyOrderIterator).second].SetValue(parsingString);

        parsingString = (char *) strtok(NULL, "\t");

    }
    
    miscDynBufDestroy(&localBuffer);

    return mcsSUCCESS;
}


/**
 * Say if the calibrator had a coherent diameter
 */
mcsLOGICAL sclsvrCALIBRATOR::HadCoherentDiameter()
{
    // If diameter has not been computed
    if (IsPropertySet(sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG) == mcsFALSE)
    {
        // Return false
        return mcsFALSE;
    }
    // Else
    else
    {
        // Get the flag, and test it
        const char *flag;
        flag = GetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG);
        if (strcmp(flag, "OK") == 0)
        {
            return mcsTRUE;
        }
        else
        {
            return mcsFALSE;
        }
    }
    // End if
}

/**
 * Say if the visibility is ok
 */
mcsLOGICAL sclsvrCALIBRATOR::VisibilityOk()
{
    // If visibility has not been computed
    if (IsPropertySet(sclsvrCALIBRATOR_VIS_FLAG) == mcsFALSE)
    {
        // Return false
        return mcsFALSE;
    }
    // Else
    else
    {
        // Get the flag, and test it
        const char *flag;
        flag = GetPropertyValue(sclsvrCALIBRATOR_VIS_FLAG);
        if (strcmp(flag, "OK") == 0)
        {
            return mcsTRUE;
        }
        else
        {
            return mcsFALSE;
        }
    }
    // End if
}

/**
 * Complete the property of the calibrator
 *
 * Method to complete calibrator properties by using several methods
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR::Complete()");
    
    // Get the observed band
    mcsSTRING16 band;
    if (request.GetConstraint(OBSERVED_BAND_ID, band) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Galactic coordinates
    if (ComputeGalacticCoordinates() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // If it is not the scenario for N band
    if (strcmp(band, "N") != 0)
    {
        // If paralax of the star if known
        if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
        {
            // Compute Interstellar extinction
            if (ComputeInterstellarAbsorption() == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute Missing Magnitude
            if (ComputeMissingMagnitude() == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute Angular Diameter
            if (ComputeAngularDiameter() != mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    // Compute visibility and visibility error
    if (ComputeVisibility(request) != mcsFAILURE)
    {
        // If visibility has been computed
        mcsFLOAT computedVis;
        if (IsPropertySet(sclsvrCALIBRATOR_VIS) == mcsTRUE)
        {
            // Get computed visibility
            GetPropertyValue(sclsvrCALIBRATOR_VIS, &computedVis);

            // Get requested visibility
            mcsFLOAT requestedVis;
            if (request.GetConstraint(STAR_EXPECTED_VIS_ID, &requestedVis)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // Check if the computed visibility is greater than the
            // requested one, and the flag accordingly
            if (computedVis < requestedVis)
            {
                SetPropertyValue(sclsvrCALIBRATOR_VIS_FLAG, "NOK");
            }
            else
            {
                SetPropertyValue(sclsvrCALIBRATOR_VIS_FLAG, "OK");
            }
        }
    }

    // compute multiplicity
    if (ComputeMultiplicity() == mcsFAILURE)
    {
        return mcsFAILURE;
    } // end multiplicity

    return mcsSUCCESS;
}


/*
 * Private methods
 */

/**
 * Compute Missing magnitude
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeMissingMagnitude()");
    alxCONFIDENCE_INDEX confidenceIndex;

    mcsFLOAT mgB, mgV, mgR, mgI, mgJ, mgH, mgK, mgL, mgM;
    mcsSTRING32 spType;
    //Get the value of the Spectral Type
    strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));
    // Get the value of the magnitude in the different band
    // if B or V is not present, return mcsFAILURE
    if (IsPropertySet(sclsvrCALIBRATOR_BO) == mcsTRUE)
    {
        if (GetPropertyValue(sclsvrCALIBRATOR_BO, &mgB) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
    }
    if (IsPropertySet(sclsvrCALIBRATOR_VO) == mcsTRUE)
    {
        if (GetPropertyValue(sclsvrCALIBRATOR_VO, &mgV) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // For the other Magnitude, don't return mcsFAILURE
    if (IsPropertySet(sclsvrCALIBRATOR_RO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_RO, &mgR);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_IO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_IO, &mgI);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_JO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_JO, &mgJ);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_HO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_HO, &mgH);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_KO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_KO, &mgK);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_LO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_LO, &mgL);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_MO) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_MO, &mgM);
    }
    
    // run alx function to compute magnitude
    if (alxComputeMagnitudesForBrightStar(spType, mgB, mgV, &mgR, &mgI, &mgJ,
                                          &mgH, &mgK, &mgL, &mgM,
                                          &confidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
  
    // If property is ever set, do nothing, else write the computed value
    if (IsPropertySet(sclsvrCALIBRATOR_RO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_RO, mgR, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_IO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_IO, mgI, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_JO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_JO, mgJ, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_HO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_HO, mgH, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_KO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_KO, mgK, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_LO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_LO, mgL, mcsTRUE, confidenceIndex);
    }
    if (IsPropertySet(sclsvrCALIBRATOR_MO) != mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_MO, mgM, mcsTRUE, confidenceIndex);
    }
    
    return mcsSUCCESS;
}

/**
 * Compute Galactic Coordonate
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeGalacticCoordinates()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeGalacticCoordinates()");
    mcsFLOAT gLat, gLon;
    mcsFLOAT ra; 
    mcsFLOAT dec;

    // Get right ascension position in degree
    if (GetRa(ra)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Get declinaison in degree
    if (GetDec(dec)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Run alxComputeGalacticCoordinates() from alx library
    if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set the galactic lattitude (if not yet set)
    SetPropertyValue(vobsSTAR_POS_GAL_LAT, gLat, mcsTRUE);

    // Set the galactic longitude (if not yet set)
    SetPropertyValue(vobsSTAR_POS_GAL_LON, gLon, mcsTRUE);

    return mcsSUCCESS;
}
/**
 * Compute Interstellar Absorption
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeInterstellarAbsorption()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeInterstellarAbsorption()");

    mcsFLOAT mgB, mgV, mgR, mgI, mgJ, mgH, mgK, mgL, mgM;
    mcsFLOAT paralax;
    mcsFLOAT gLat, gLon;


    mcsFLOAT starProperty[9];
    mcsLOGICAL isPropertySet[9];
    mcsSTRING64 starPropertyId[9] = 
    {
        vobsSTAR_PHOT_JHN_B,
        vobsSTAR_PHOT_JHN_V,
        vobsSTAR_PHOT_JHN_R,
        vobsSTAR_PHOT_JHN_I,
        vobsSTAR_PHOT_JHN_J,
        vobsSTAR_PHOT_JHN_H,
        vobsSTAR_PHOT_JHN_K,
        vobsSTAR_PHOT_JHN_L,
        vobsSTAR_PHOT_JHN_M
    };

    // for each property
    for (int i=0; i<9; i++)
    { 
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            GetPropertyValue(starPropertyId[i], &starProperty[i]);
            isPropertySet[i] = mcsTRUE;
        }
        else
        {
            starProperty[i] = 0;
            isPropertySet[i] = mcsFALSE;
        }
    }
    mgB=starProperty[0];
    mgV=starProperty[1];
    mgR=starProperty[2];
    mgI=starProperty[3];
    mgJ=starProperty[4];
    mgH=starProperty[5];
    mgK=starProperty[6];
    mgL=starProperty[7];
    mgM=starProperty[8];

    // Get the value of the paralax, the galactic coordinates if it exists.
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &paralax);
    }
    else 
    {
        logWarning("can't compute interstellar absorption because paralax not set");
        return mcsFAILURE;
    }
    if (IsPropertySet(vobsSTAR_POS_GAL_LAT) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_GAL_LAT, &gLat);
    }
    else
    {
        logWarning("can't compute interstellar absorption because galactic Lattitude not set");
        return mcsFAILURE;
    }
    if (IsPropertySet(vobsSTAR_POS_GAL_LON) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_GAL_LON, &gLon);
    }
    else 
    {
        logWarning("can't compute interstellar absorption because galactic Longitude not set");
        return mcsFAILURE;
    }

    // Run alx function to compute corrected magnitude
    if (alxComputeRealMagnitudes(paralax, gLat, gLon, &mgM, &mgL, &mgK, &mgH,
                                 &mgJ, &mgI, &mgR, &mgV, &mgB) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Print in the calibrators properties the magnitude if they had been
    // computed, else do nothing
    if (isPropertySet[8] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_MO, mgM, mcsTRUE);
    }
    if (isPropertySet[7] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_LO, mgL, mcsTRUE);
    }
    if (isPropertySet[6] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_KO, mgK, mcsTRUE);
    }
    if (isPropertySet[5] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_HO, mgH, mcsTRUE);
    }
    if (isPropertySet[4] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_JO, mgJ, mcsTRUE);
    }
    if (isPropertySet[3] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_IO, mgI, mcsTRUE);
    }
    if (isPropertySet[2] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_RO, mgR, mcsTRUE);
    }
    if (isPropertySet[1] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_VO, mgV, mcsTRUE);
    }
    if (isPropertySet[0] == mcsTRUE)
    {
        SetPropertyValue(sclsvrCALIBRATOR_BO, mgB, mcsTRUE);
    }

    return mcsSUCCESS;
}
/**
 * Compute Angular diameter
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeAngularDiameter()");
    mcsFLOAT diam, diamError;
    mcsFLOAT mgB, mgV, mgR, mgK;
    mcsFLOAT starProperty[4];
    alxCONFIDENCE_INDEX confidenceIndex;
    
    char *starPropertyId[4] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_KO
    };

    // for each property needed to compute angular diameter, check if they are
    // affected
    for (int i=0; i<4; i++)
    { 
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            GetPropertyValue(starPropertyId[i], &starProperty[i]);
        }
        else
        {
            errAdd(sclsvrERR_PROPERTY_NOT_SET, "Corrected magnitude");
            return mcsFAILURE;
        }
    }
    mgB=starProperty[0];
    mgV=starProperty[1];
    mgR=starProperty[2];
    mgK=starProperty[3];
     
    if (alxComputeAngularDiameter(mgB, mgV, mgR, mgK,
                                  &diam,
                                  &diamError,
                                  &confidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set compute value of the angular diameter
    SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM, diam,
                     mcsTRUE, confidenceIndex);
    SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM_ERROR, diamError,
                     mcsTRUE, confidenceIndex);
   
    // Set flag according to the confidence index 
    if (confidenceIndex == alxCONFIDENCE_LOW)
    {
         SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG, "NOK");
    }
    else
    {
         SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG, "OK");
    }
   
    return mcsSUCCESS;
}
/**
 * Compute Visibility
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR::ComputeVisibility()");
    mcsFLOAT diam, diamError;
    mcsFLOAT baseMax, wavelength;
    mcsFLOAT vis, vis2, visErr, vis2Err;

    // Get object diameter. First look at the diameter coming from catalog, and
    // if not found used the computed diameter.
    if (IsPropertySet(vobsSTAR_EXTENSION_DIAM) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_EXTENSION_DIAM, &diam);
        GetPropertyValue(vobsSTAR_EXTENSION_DIAM_ERROR, &diamError);
    }
    else
    {
        if (IsPropertySet(sclsvrCALIBRATOR_ANGULAR_DIAM) == mcsTRUE)
        {
            GetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM, &diam);
            GetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAM_ERROR, &diamError);
        }
        else
        {
            // If diameter is not available, do not compute visibility
            logWarning("Coul not compute visibility, no diameter available");
            return mcsSUCCESS;
        }
    }

    // Get value in request of the wavelength
    request.GetConstraint(STAR_WLEN_ID, &wavelength);
    // get value in request of the base max
    request.GetConstraint(BASEMAX_ID, &baseMax);
    
    if (alxComputeVisibility(diam, diamError, baseMax, wavelength,
                             &vis, &vis2, &visErr, &vis2Err)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Affect visibility property
    SetPropertyValue(sclsvrCALIBRATOR_VIS, vis2);
    SetPropertyValue(sclsvrCALIBRATOR_VIS_ERROR, vis2Err);
    
    return mcsSUCCESS;
}

/**
 * Compute Multiplicity
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMultiplicity()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeMultiplicity()");

    //SetPropertyValue(sclsvrCALIBRATOR_MULTIPLICITY, "1.00");
    
    return mcsSUCCESS;
}

/**
 * Add all star properties 
 *
 * \return mcsSUCCESS 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::AddProperties(void)
{
    //logExtDbg("vobsSTAR::AddProperties()");
    AddProperty(sclsvrCALIBRATOR_ANGULAR_DIAM, "diam", vobsFLOAT_PROPERTY, 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_ANGULAR_DIAM_ERROR, "diamErr", 
                vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG, "diamFlag", 
                vobsSTRING_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_VIS_FLAG, "visFlag", vobsSTRING_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_MO, "Mo", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_LO, "Lo", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_KO, "Ko", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_HO, "Ho", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_JO, "Jo", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_IO, "Io", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_RO, "Ro", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_VO, "Vo", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_BO, "Bo", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS, "vis", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS_ERROR, "visErr", 
                vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS_FLAG, "visFlag", vobsSTRING_PROPERTY);

    return mcsSUCCESS;
}

/*___oOo___*/

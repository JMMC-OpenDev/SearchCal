/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.16 2005-01-07 14:13:12 scetre Exp $"
 *
 * who       when         what
 * --------  -----------  -------------------------------------------------------
 * scetre    14-Sep-2004  Created
 *
 *
 *******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR class definition.
 */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.16 2005-01-07 14:13:12 scetre Exp $"; 
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
    _propertyList = star._propertyList;
    _propertyOrder = star._propertyOrder;
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
 * \return always SUCCESS
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
    
    // Write in the buffer diam OK if the diameter is coherent, else diamNOK
    if (_coherentDiameter == mcsTRUE)
    {
        miscDynBufAppendString(buffer,"diamOK\t");
    }
    else 
    {
        miscDynBufAppendString(buffer,"diamNOK\t");        
    }

    // Write in the buffer visOK if visibility is ok, else visNOK
    if (_correctVisibility == mcsTRUE)
    {
        miscDynBufAppendString(buffer,"visOK\t");
    }
    else 
    {
        miscDynBufAppendString(buffer,"visNOK\t");        
    }

    return SUCCESS;
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
    // initialized the buffer which will contain the calibrator
    miscDYN_BUF localBuffer;
    miscDynBufInit(&localBuffer);
    // Copy the calibrator in the local buffer
    miscDynBufAppendString(&localBuffer, calibratorString);
    // parse the buffer
    parsingString = (char *) strtok (miscDynBufGetBufferPointer(&localBuffer), "\t");
    
    for (_propertyOrderIterator = _propertyOrder.begin();
         _propertyOrderIterator != _propertyOrder.end();
         _propertyOrderIterator++)
    {
        _propertyList[(*_propertyOrderIterator).second].SetValue(parsingString);

        parsingString = (char *) strtok (NULL, "\t");

    }
    
    if (strcmp(parsingString, "diamOK") == 0)
    {
        _coherentDiameter = mcsTRUE;
    }
    else
    {
        _coherentDiameter = mcsFALSE;
    }
    parsingString = (char *) strtok (NULL, "\t");
    if (strcmp(parsingString, "visOK") == 0)
    {
        _correctVisibility = mcsTRUE;
    }
    else
    {
        _correctVisibility = mcsFALSE;
    }
    miscDynBufDestroy(&localBuffer);

    return SUCCESS;
}


/**
 * Say if the calibrator had a coherent diameter
 */
mcsLOGICAL sclsvrCALIBRATOR::HadCoherentDiameter()
{
    return _coherentDiameter;
}

/**
 * Say if the visibility is ok
 */
mcsLOGICAL sclsvrCALIBRATOR::VisibilityOk()
{
    return _correctVisibility;
}

/**
 * Complete the property of the calibrator
 *
 * Method to complete calibrator properties by using several methods
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR::Complete()");
    
    _coherentDiameter = mcsFALSE;
    _correctVisibility = mcsFALSE;

    // Compute Galactic coordinates
    if (ComputeGalacticCoordinates() == FAILURE)
    {
        return FAILURE;
    }
    // Compute Interstellar extinction
    if (ComputeInterstellarAbsorption() == FAILURE)
    {
        return FAILURE;
    }
    // Compute Missing Magnitude
    if (ComputeMissingMagnitude() == FAILURE)
    {
        return FAILURE;
    }
    // Compute Angular Diameter
    if (ComputeAngularDiameter() != FAILURE)
    {
        if (_coherentDiameter == mcsTRUE)
        {
            // Compute visibility and visibility error
            if (ComputeVisibility(request) != FAILURE)
            {
                // Get computed visibility
                mcsFLOAT computedVisibility;
                if (IsPropertySet(sclsvrCALIBRATOR_VISIBILITY) == 
                    mcsTRUE)
                {
                    GetPropertyValue(sclsvrCALIBRATOR_VISIBILITY,
                                     &computedVisibility);
                }
                else
                {
                    return FAILURE;
                }
                // Get wanted visibility
                mcsFLOAT requestedVisibility;
                if (request.GetConstraint(STAR_EXPECTED_VIS_ID,
                                          &requestedVisibility)
                    == FAILURE)
                {
                    return FAILURE;
                }
                // check if the compute visibility is inferior to 
                // the wanted visibility
                if (computedVisibility >= requestedVisibility)
                {
                    _correctVisibility = mcsTRUE;    
                }
            }
        }
    }

    // compute multiplicity
    if (ComputeMultiplicity() == FAILURE)
    {
        return FAILURE;
    } // end multiplicity


    return SUCCESS;
}


/*
 * Private methods
 */

/**
 * Compute Missing magnitude
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    mcsFLOAT mgB, mgV, mgR, mgI, mgJ, mgH, mgK, mgL, mgM;
    mcsSTRING32 spType;
    //Get the value of the Spectral Type
    strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));
    // Get the value of the magnitude in the different band
    // if B or V is not present, return FAILURE
    if (IsPropertySet(sclsvrCALIBRATOR_BO) == mcsTRUE)
    {
        if (GetPropertyValue(sclsvrCALIBRATOR_BO, &mgB) == FAILURE)
        {       
            return FAILURE;
        }
    }
    if (IsPropertySet(sclsvrCALIBRATOR_VO) == mcsTRUE)
    {
        if (GetPropertyValue(sclsvrCALIBRATOR_VO, &mgV) == FAILURE)
        {
            return FAILURE;
        }
    }
    // For the other Magnitude, don't return FAILURE
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
    if (alxComputeMissingMagnitude(spType,
                                   mgB,
                                   mgV,
                                   &mgR,
                                   &mgI,
                                   &mgJ,
                                   &mgH,
                                   &mgK,
                                   &mgL,
                                   &mgM) == FAILURE)
    {
        return FAILURE;
    }

    // Print in the calibrators properties the magnitude
    SetPropertyValue(sclsvrCALIBRATOR_MO, mgM);
    SetPropertyValue(sclsvrCALIBRATOR_LO, mgL);
    SetPropertyValue(sclsvrCALIBRATOR_KO, mgK);
    SetPropertyValue(sclsvrCALIBRATOR_HO, mgH);
    SetPropertyValue(sclsvrCALIBRATOR_JO, mgJ);
    SetPropertyValue(sclsvrCALIBRATOR_IO, mgI);
    SetPropertyValue(sclsvrCALIBRATOR_RO, mgR);
    SetPropertyValue(sclsvrCALIBRATOR_VO, mgV);
    SetPropertyValue(sclsvrCALIBRATOR_BO, mgB);
    
    return SUCCESS;
}

/**
 * Compute Galactic Coordonate
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeGalacticCoordinates()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeGalacticCoordinates()");
    mcsFLOAT gLat, gLon;
    mcsFLOAT ra; 
    mcsFLOAT dec;

    // Get right ascension position in degree
    if (GetRa(ra)==FAILURE)
    {
        return FAILURE;
    }
    
    // Get declinaison in degree
    if (GetDec(dec)==FAILURE)
    {
        return FAILURE;
    }
    
    // Run alxComputeGalacticCoordinates() from alx library
    if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon)
        == FAILURE)
    {
        return FAILURE;
    }

    // If the galactic lattitude is not yet affected
    // Write the new value in the compute value table
    SetPropertyValue(vobsSTAR_POS_GAL_LAT, gLat);
    // If the galactic longitude is not yet affected
    // Write the new value in the compute value table
    SetPropertyValue(vobsSTAR_POS_GAL_LON, gLon);

    return SUCCESS;
}
/**
 * Compute Interstellar Absorption
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeInterstellarAbsorption()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeInterstellarAbsorption()");

    mcsFLOAT noMagnitudeDefined=99.99;
    
    mcsFLOAT mgB, mgV, mgR, mgI, mgJ, mgH, mgK, mgL, mgM;
    mcsFLOAT paralax;
    mcsFLOAT gLat, gLon;
    mcsFLOAT e_b_v=99.99;


    mcsFLOAT starProperty[9];
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
        }
        else 
        {
            starProperty[i]=noMagnitudeDefined;
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

    // Get the value of the paralax, the galactic coordinates, and the
    // interstellar extinction if it exists.
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &paralax);
    }
    else 
    {
        logWarning("can't compute interstellar absorption because paralax not set");
        return FAILURE;
    }
    if (IsPropertySet(vobsSTAR_POS_GAL_LAT) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_GAL_LAT, &gLat);
    }
    else
    {
        logWarning("can't compute interstellar absorption because galactic Lattitude not set");
        return FAILURE;
    }
    if (IsPropertySet(vobsSTAR_POS_GAL_LON) == mcsTRUE)
    {
        GetPropertyValue(vobsSTAR_POS_GAL_LON, &gLon);
    }
    else 
    {
        logWarning("can't compute interstellar absorption because galactic Longitude not set");
        return FAILURE;
    }

    // Run alx function to compute corrected magnitude
    if (alxComputeRealMagnitude(paralax,
                                gLat,
                                gLon,
                                e_b_v,
                                &mgM,
                                &mgL,
                                &mgK,
                                &mgH,
                                &mgJ,
                                &mgI,
                                &mgR,
                                &mgV,
                                &mgB) == FAILURE)
    {
        return FAILURE;
    }

    // Print in the calibrators properties the magnitude if they had been
    // computed, else do nothing
    if (mgM != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_MO, mgM);
    }
    if (mgL != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_LO, mgL);
    }
    if (mgK != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_KO, mgK);
    }
    if (mgH != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_HO, mgH);
    }
    if (mgJ != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_JO, mgJ);
    }
    if (mgI != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_IO, mgI);
    }
    if (mgR != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_RO, mgR);
    }
    if (mgV != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_VO, mgV);
    }
    if (mgB != noMagnitudeDefined)
    {
        SetPropertyValue(sclsvrCALIBRATOR_BO, mgB);
    }

    return SUCCESS;
}
/**
 * Compute Angular diameter
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeAngularDiameter()");
    mcsFLOAT angularDiameter, angularDiameterError;
    mcsFLOAT mgB, mgV, mgR, mgK;
    mcsFLOAT starProperty[4];
    char *starPropertyId[4] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_KO
    };

    // for each property
    for (int i=0; i<4; i++)
    { 
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            GetPropertyValue(starPropertyId[i], &starProperty[i]);
        }
        else
        {
            errAdd(sclsvrERR_PROPERTY_NOT_SET, "Corrected magnitude");
            return FAILURE;
        }
    }
    mgB=starProperty[0];
    mgV=starProperty[1];
    mgR=starProperty[2];
    mgK=starProperty[3];
     
    if (alxComputeAngularDiameter(&angularDiameter,
                                  &angularDiameterError,
                                  mgB,
                                  mgV,
                                  mgR,
                                  mgK,
                                  &_coherentDiameter) == FAILURE)
    {
        return FAILURE;
    }
    // Set compute value of the angular diameter
    SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER, angularDiameter);
    SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR,
                     angularDiameterError);

    return SUCCESS;
}
/**
 * Compute Visibility
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(vobsREQUEST request)
{
    logExtDbg("sclsvrCALIBRATOR::ComputeVisibility()");
    mcsFLOAT angularDiameter, angularDiameterError;
    mcsFLOAT baseMax, wavelength;
    mcsFLOAT visibility, visibility2, visibilityError, visibilityError2;

    // Get value of the angular diameter and the error on it
    if (IsPropertySet(sclsvrCALIBRATOR_ANGULAR_DIAMETER) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER,
                    &angularDiameter);
    }
    else
    {
        logWarning("can't compute visibility because angular diameter not set");
        return FAILURE;
    }

    if (IsPropertySet(sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR) == mcsTRUE)
    {
        GetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR,
                    &angularDiameterError);
    }
    else
    {
        logWarning("can't compute visibility because angular diameter error not set");
        return FAILURE;
    }
        
    // Get value in request of the wavelength
    request.GetConstraint(STAR_WLEN_ID, &wavelength);
    // get value in request of the base max
    request.GetConstraint(BASEMAX_ID, &baseMax);
    
    if (alxComputeVisibility(angularDiameter,
                             angularDiameterError,
                             baseMax,
                             wavelength,
                             &visibility,
                             &visibility2,
                             &visibilityError,
                             &visibilityError2)==FAILURE)
    {
        return FAILURE;
    }
    // Affect visibility property
    SetPropertyValue(sclsvrCALIBRATOR_VISIBILITY, visibility2);
    SetPropertyValue(sclsvrCALIBRATOR_VISIBILITY_ERROR, visibilityError2);
    
    return SUCCESS;
}
/**
 * Compute Multiplicity
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMultiplicity()
{
    logExtDbg("sclsvrCALIBRATOR::ComputeMultiplicity()");

    SetPropertyValue(sclsvrCALIBRATOR_MULTIPLICITY, "1.00");
    
    return SUCCESS;
}

/**
 * Add all star properties 
 *
 * \return SUCCESS 
 */
 
mcsCOMPL_STAT sclsvrCALIBRATOR::AddProperties(void)
{
    //logExtDbg("vobsSTAR::AddProperties()");
    
    AddProperty(sclsvrCALIBRATOR_ANGULAR_DIAMETER, "angDiam", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[31]=sclsvrCALIBRATOR_ANGULAR_DIAMETER;
    AddProperty(sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR, "angDiamErr", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[32]=sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR;
    AddProperty(sclsvrCALIBRATOR_MO, "Mo", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[33]=sclsvrCALIBRATOR_MO;
    AddProperty(sclsvrCALIBRATOR_LO, "Lo", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[34]=sclsvrCALIBRATOR_LO;
    AddProperty(sclsvrCALIBRATOR_KO, "Ko", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[35]=sclsvrCALIBRATOR_KO;
    AddProperty(sclsvrCALIBRATOR_HO, "Ho", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[36]=sclsvrCALIBRATOR_HO;
    AddProperty(sclsvrCALIBRATOR_JO, "Jo", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[37]=sclsvrCALIBRATOR_JO;
    AddProperty(sclsvrCALIBRATOR_IO, "Io", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[38]=sclsvrCALIBRATOR_IO;
    AddProperty(sclsvrCALIBRATOR_RO, "Ro", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[39]=sclsvrCALIBRATOR_RO;
    AddProperty(sclsvrCALIBRATOR_VO, "Vo", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[40]=sclsvrCALIBRATOR_VO;
    AddProperty(sclsvrCALIBRATOR_BO, "Bo", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[41]=sclsvrCALIBRATOR_BO;
    AddProperty(sclsvrCALIBRATOR_MULTIPLICITY, "multiplicity", vobsFLOAT_PROPERTY);
    _propertyOrder[42]=sclsvrCALIBRATOR_MULTIPLICITY;
    AddProperty(sclsvrCALIBRATOR_VISIBILITY, "visibility", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[43]=sclsvrCALIBRATOR_VISIBILITY;
    AddProperty(sclsvrCALIBRATOR_VISIBILITY_ERROR, "visibilityErr", vobsFLOAT_PROPERTY, "%.3f");
    _propertyOrder[44]=sclsvrCALIBRATOR_VISIBILITY_ERROR;

    return SUCCESS;
}


/*___oOo___*/

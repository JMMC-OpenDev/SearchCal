/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.C,v 1.2 2004-11-25 14:54:46 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR.C,v 1.2 2004-11-25 14:54:46 scetre Exp $"; 
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

/* Local variables */
// Array containing the property name with the corresponding format used to
// set it. */
static char *propertyNameList[][2] =
{
    {"ANGULAR_DIAMETER",        "%.2f"},
    {"ANGULAR_DIAMETER_ERROR",  "%.2f"},
    {"MO",                      "%.2f"},
    {"LO",                      "%.2f"},
    {"KO",                      "%.2f"},
    {"HO",                      "%.2f"},
    {"JO",                      "%.2f"},
    {"IO",                      "%.2f"},
    {"RO",                      "%.2f"},
    {"VO",                      "%.2f"},
    {"BO",                      "%.2f"},
    {"MULTIPLICITY",            "%.2f"},
    {"VISIBILITY",              "%.2f"},
    {"VISIBILITY_ERROR",        "%.2f"},
    { NULL,                     NULL}
};

/**
 * Class constructor.
 *
 * Build a calibrator object.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR()
{
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        strcpy(_properties[ucdId], vobsSTAR_PROP_NOT_SET);
    }
    for (int propertyId=0; propertyId<sclsvrNB_CALIBRATOR_PROPERTIES; propertyId++)
    {
        strcpy(_compProperties[propertyId], sclsvrCALIBRATOR_PROP_NOT_SET);
    }
}

/**
 * Build a calibrator object from another one (copy constructor).
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const sclsvrCALIBRATOR &star)
{
    logExtDbg("sclsvrCALIBRATOR::sclsvrCALIBRATOR(const sclsvrCALIBRATOR &star)");
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        star.GetProperty(ucdId, (char *)_properties[ucdId]);
    }
    for (int propertyIdx=0; propertyIdx<sclsvrNB_CALIBRATOR_PROPERTIES; propertyIdx++)
    {
        int propertyId;
        propertyId = propertyIdx + sclsvrPROP_ID_OFFSET;
        star.GetProperty(propertyId, (char *)_compProperties[propertyIdx]);
    }
}

/**
 * Build a calibrator object from a vobsSTAR instance.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const vobsSTAR &star)
{
    logExtDbg("sclsvrCALIBRATOR::sclsvrCALIBRATOR(const vobsSTAR &star)");
    // Copy properties of the star
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        star.GetProperty((vobsUCD_ID)ucdId, (char *)_properties[ucdId]);
    }
    for (int propertyIdx=0; propertyIdx<sclsvrNB_CALIBRATOR_PROPERTIES; propertyIdx++)
    {
        strcpy(_compProperties[propertyIdx], sclsvrCALIBRATOR_PROP_NOT_SET);
    }
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
 * Set a calibrator property.
 *
 * Set value corresponding to the UCD or the property name . By default, the
 * value is writen only if it is not already set, except if the overwrite flag
 * is
 * true.
 * 
 * \param name UCD or PROPERTY. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::SetProperty(char *name, char *value, 
                                            mcsLOGICAL overwrite)
{
    logExtDbg("sclsvrCALIBRATOR::SetProperty()");

    // If given name is a UCD 
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(name);
    if (ucdId != UNKNOWN_UCD_ID)
    {
        // Set star property
        return (vobsSTAR::SetProperty(name, value, overwrite));
    }
    // Else
    {
        // Get Id corresponding to the specified PROPERTY
        sclsvrPROPERTY_ID propId;
        propId = Property2Id(name);
        // If property id is unknown
        if (propId == UNKNOWN_PROP_ID)
        {
            // Return error
            errAdd(sclsvrERR_INVALID_PROPERTY_NAME, name);
            return FAILURE;
        }
        // Else
        else 
        {
            // Affect value
            return (SetProperty(propId, value, overwrite));
        }
        // End if
    }
    // End if
}

/**
 * Set a calibrator property.
 *
 * Set value property corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::SetProperty(vobsUCD_ID ucdId, char *value,
                                            mcsLOGICAL overwrite)
{
    return vobsSTAR::SetProperty(ucdId, value, overwrite);
}

/**
 * Set a calibrator property.
 *
 * Set value property corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::SetProperty(vobsUCD_ID ucdId, mcsFLOAT value,
                                            mcsLOGICAL overwrite)
{
    return vobsSTAR::SetProperty(ucdId, value, overwrite);
}

/**
 * Set a calibrator property.
 *
 * Set value property corresponding to the PROPERTY
 * 
 * \param id property or UCD id. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_ID
 * 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::SetProperty(sclsvrPROPERTY_ID id,
                                            char *value,
                                            mcsLOGICAL overwrite)
{
    logExtDbg("sclsvrCALIBRATOR::SetProperty()");

    // If it is not a valid property id, return error
    if ((id == UNKNOWN_PROP_ID) ||
        (id < UNKNOWN_PROP_ID) ||
        (id > VISIBILITY_ERROR_ID))
    {
        errAdd(sclsvrERR_INVALID_PROPERTY_ID, id);
        return FAILURE;
    }
    // Affect property value
    if ((IsPropertySet(id) == mcsFALSE) || (overwrite == mcsTRUE))
    {
        int propertyIdx; 
        propertyIdx = id - sclsvrPROP_ID_OFFSET;
        strcpy(_compProperties[propertyIdx], value);
    }


    return SUCCESS;    
}
/**
 * Set a calibrator property.
 *
 * Set value property corresponding to the PROPERTY
 * 
 * \param id property or UCD id. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::SetProperty(sclsvrPROPERTY_ID id,
                                            mcsFLOAT value,
                                            mcsLOGICAL overwrite)
{
    logExtDbg("sclsvrCALIBRATOR::SetProperty()");

    // If it is not a valid property id, return error
    if ((id == UNKNOWN_PROP_ID) ||
        (id < UNKNOWN_PROP_ID) ||
        (id > VISIBILITY_ERROR_ID))
    {
        errAdd(sclsvrERR_INVALID_PROPERTY_ID, id);
        return FAILURE;
    }
    // Affect property value
    if ((IsPropertySet(id) == mcsFALSE) || (overwrite == mcsTRUE))
    {
        int propertyIdx; 
        propertyIdx = id - sclsvrPROP_ID_OFFSET;
        sprintf(_compProperties[propertyIdx], propertyNameList[propertyIdx][1],
                value);
    }

    return SUCCESS;    
}

/**
 * Check whether the property is set or not.  
 * 
 * \param ucd UCD name. 
 * 
 * \warning
 * If the given UCD name is unknown, this method returns false (i.e.  mcsFALSE)
 *
 * \return
 * True value (i.e. mcsTRUE) if the the property has been set, false (i.e.
 * mcsFALSE) otherwise.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsPropertySet(char *name) const
{
    //logExtDbg("vobsSTAR::IsPropertySet()");

    // If given name is a UCD 
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(name);
    if (ucdId != UNKNOWN_UCD_ID)
    {
        // Call vobsSTAR method
        return (vobsSTAR::IsPropertySet(name));
    }
    // Else
    else
    {
        // Check if property is set 
        sclsvrPROPERTY_ID propId;
        propId = Property2Id(name);
        return (IsPropertySet(propId));
    }
    // End if
}

mcsLOGICAL sclsvrCALIBRATOR::IsUcdId(int id) const
{
    logExtDbg("sclsvrCALIBRATOR::IsUcdId(%d)", id);

    if ((id <= UNKNOWN_UCD_ID) || (id > PHOT_COLOR_EXCESS_ID))
    {
        return mcsFALSE;
    }
    else
    {
        return mcsTRUE;
    }
}


/**
 * Check whether the property is set or not.  
 * 
 * \param ucd UCD id. 
 * 
 * \warning
 * If the given UCD id is invalid, this method returns false (i.e.  mcsFALSE)
 *
 * \return
 * True value (i.e. mcsTRUE) if the the property has been set, false (i.e.
 * mcsFALSE) otherwise.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsPropertySet(int id) const
{
    // If id is an UCD id
    if (IsUcdId(id) == mcsTRUE)
    {
        // Set star property
        return (vobsSTAR::IsPropertySet((vobsUCD_ID)id));
    }
    // Else
    else
    {
        // If it is not a valid property id, return false
        if ((id == UNKNOWN_PROP_ID) ||
            (id < UNKNOWN_PROP_ID) ||
            (id > VISIBILITY_ERROR_ID))
        {
            return mcsFALSE;
        }

        // Check if property string value is set to vobsSTAR_PROP_NOT_SET
        int propertyIdx; 
        propertyIdx = id - sclsvrPROP_ID_OFFSET;
        if (strcmp(_compProperties[propertyIdx], vobsSTAR_PROP_NOT_SET) == 0)
        {
            return mcsFALSE;
        }
        else
        {
            return mcsTRUE;
        }
    }
}

/**
 * Get a calibrator property, as a string.
 *
 * Get value property, as a string, corresponding to the PROPERTY
 * 
 * \param property PROPERTY name. 
 * \param value property value to get, as a string.
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_NAME
 *
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::GetProperty(char *name, char *value) const
{
    logExtDbg("sclsvrCALIBRATOR::GetProperty()");

    // If given name is a UCD 
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(name);
    if (ucdId != UNKNOWN_UCD_ID)
    {
        // Get star property
        return (vobsSTAR::GetProperty(name, value));
    }
    // Else
    {
        // Get Id corresponding to the specified PROPERTY
        sclsvrPROPERTY_ID propId;
        propId = Property2Id(name);
        // If property id is unknown
        if (propId == UNKNOWN_PROP_ID)
        {
            // Return error
            errAdd(sclsvrERR_INVALID_PROPERTY_NAME, name);
            return FAILURE;
        }
        // Else
        else 
        {
            // Get calibrator property
            return (GetProperty(propId, value));
        }
        // End if
    }
    // End if
}

/**
 * Get a star property, as a string.
 *
 * Get value property, as a string, corresponding to the PROPERTY
 * 
 * \param propertyId PROPERTY id. 
 * \param value property value to get, as a string. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::GetProperty(int id,
                                            char *value) const
{
    //logExtDbg("sclsvrCALIBRATOR::GetProperty()");

    // If id is an UCD id
    if (IsUcdId(id) == mcsTRUE)
    {
        // Get star property
        return (vobsSTAR::GetProperty((vobsUCD_ID)id, value));
    }
    // Else
    else
    {
        // If it is not a valid property id, return error
        if ((id == UNKNOWN_PROP_ID) ||
            (id < UNKNOWN_PROP_ID) ||
            (id > VISIBILITY_ERROR_ID))
        {
            errAdd(sclsvrERR_INVALID_PROPERTY_ID, id);
            return FAILURE;
        }
        // Retrieve property value
        int propertyIdx; 
        propertyIdx = id - sclsvrPROP_ID_OFFSET;
        strcpy(value, _compProperties[propertyIdx]);
    }

    return SUCCESS;
}

/**
 * Get a calibrator property, as a float.
 *
 * Get value property corresponding, as a float, to the PROPERTY
 * 
 * \param property PROPERTY name. 
 * \param value property value to get, as a float. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::GetProperty(char *name, float *value) const
{
    logExtDbg("sclsvrCALIBRATOR::GetProperty()");

    // If given name is a UCD 
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(name);
    if (ucdId != UNKNOWN_UCD_ID)
    {
        // Get star property
        return (vobsSTAR::GetProperty(name, value));
    }
    // Else
    {
        // Get Id corresponding to the specified PROPERTY
        sclsvrPROPERTY_ID propId;
        propId = Property2Id(name);
        // If property id is unknown
        if (propId == UNKNOWN_PROP_ID)
        {
            // Return error
            errAdd(sclsvrERR_INVALID_PROPERTY_NAME, name);
            return FAILURE;
        }
        // Else
        else 
        {
            // Get calibrator property
            return (GetProperty(propId, value));
        }
        // End if
    }
    // End if
}

/**
 * Get a calibrator property, as a float.
 *
 * Get value property, as a float, corresponding to the PROPERTY
 * 
 * \param propertyId PROPERTY id. 
 * \param value property value to get, as a float. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_PROPERTY_ID
 * \li sclsvrERR_INVALID_PROPERTY_FORMAT
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::GetProperty(int id,
                                            float *value) const
{
    logExtDbg("sclsvrCALIBRATOR::GetProperty(%d)", id);

    // If id is an UCD id
    if (IsUcdId(id) == mcsTRUE)
    {
        // Get star property
        return (vobsSTAR::GetProperty((vobsUCD_ID)id, value));
    }
    // Else
    else
    {
        // If it is not a valid property id, return error
        if ((id == UNKNOWN_PROP_ID) ||
            (id < UNKNOWN_PROP_ID) ||
            (id > VISIBILITY_ERROR_ID))
        {
            errAdd(sclsvrERR_INVALID_PROPERTY_ID, id);
            return FAILURE;
        }
        // Convert property string value to float value
        int propertyIdx; 
        propertyIdx = id - sclsvrPROP_ID_OFFSET;
        if (sscanf(_compProperties[propertyIdx], "%f", value) != 1)
        {
            errAdd(sclsvrERR_INVALID_PROPERTY_FORMAT, 
                   _compProperties[propertyIdx], "float");
            return FAILURE;
        }
        
    }
    // End if

    return SUCCESS;
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

    if (ComputeGalacticCoordinates() == FAILURE)
    {
        return FAILURE;
    }
    if (ComputeInterstellarAbsorption() == FAILURE)
    {
        return FAILURE;
    }
    if (ComputeMissingMagnitude() == FAILURE)
    {
        return FAILURE;
    }
    if (ComputeAngularDiameter() == FAILURE)
    {
        return FAILURE;
    }
    if (ComputeVisibility(request) == FAILURE)
    {
        return FAILURE;
    }
    if (ComputeMultiplicity() == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Display all calibrator properties on the console.
 * 
 */
void sclsvrCALIBRATOR::Display(void)
{
    logExtDbg("sclsvrCALIBRATOR::Display()");
    cout << "-----------------------------------------" << endl;
    for (int i=0; i<vobsNB_STAR_PROPERTIES; i++)
    {
        printf("%12s", _properties[i]);
    }
    printf("\n");
    for (int j=0; j<sclsvrNB_CALIBRATOR_PROPERTIES; j++)
    {
        printf("%12s", _compProperties[j]);
    }
    printf("\n");    

}

/*
 * Protected methods
 */
/**
 * Convert a given PROPERTY to an id.
 *
 * \param property PROPERTY name.
 *
 * \return an PROPERTY id.
 *
 */
sclsvrPROPERTY_ID sclsvrCALIBRATOR::Property2Id(char *property) const
{
    logExtDbg("sclsvrCALIBRATOR::Property2Id()");

    for (unsigned int i=0; i<sclsvrNB_CALIBRATOR_PROPERTIES; i++)
    {
        if (strcmp(propertyNameList[i][0], property) == 0)
        {
            return (sclsvrPROPERTY_ID)(sclsvrPROP_ID_OFFSET + i);
        }
    }

    return UNKNOWN_PROP_ID;
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
    strcpy(spType, _properties[SPECT_TYPE_MK_ID]);
    // Get the value of the magnitude in the different band
    if (GetProperty(PHOT_JHN_B_ID, &mgB) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_V_ID, &mgV) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_R_ID, &mgR) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_I_ID, &mgI) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_J_ID, &mgJ) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_H_ID, &mgH) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_K_ID, &mgK) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_L_ID, &mgL) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_M_ID, &mgM) == FAILURE)
    {
        return FAILURE;
    }

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

    logTest("B = %0.3f",mgB);
    logTest("V = %0.3f",mgV);
    logTest("R = %0.3f",mgR);
    logTest("I = %0.3f",mgI);
    logTest("J = %0.3f",mgJ);
    logTest("H = %0.3f",mgH);
    logTest("K = %0.3f",mgK);
    logTest("L = %0.3f",mgL);
    logTest("M = %0.3f",mgM);

    // Check if the property is not already affected.
    // If not, affect
    if (strcmp(_properties[PHOT_JHN_R_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    { 
        vobsSTAR::SetProperty(PHOT_JHN_R_ID, mgR);
    }
    if (strcmp(_properties[PHOT_JHN_I_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_I_ID, mgI);        
    }
    if (strcmp(_properties[PHOT_JHN_J_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_J_ID, mgJ);
    }
    if (strcmp(_properties[PHOT_JHN_H_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_H_ID, mgH);
    }
    if (strcmp(_properties[PHOT_JHN_K_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_K_ID, mgK);
    }
    if (strcmp(_properties[PHOT_JHN_L_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_L_ID, mgL);
    }
    if (strcmp(_properties[PHOT_JHN_M_ID],sclsvrCALIBRATOR_PROP_NOT_SET)==0)
    {
        vobsSTAR::SetProperty(PHOT_JHN_M_ID, mgM);
    }
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
    if (strcmp(_properties[POS_GAL_LAT_ID],sclsvrCALIBRATOR_PROP_NOT_SET)
        ==0)
    {
        // Write the new value in the compute value table
        vobsSTAR::SetProperty(POS_GAL_LAT_ID, gLat);
    }
    // If the galactic longitude is not yet affected
    if (strcmp(_properties[POS_GAL_LON_ID],sclsvrCALIBRATOR_PROP_NOT_SET)
        ==0)
    {
        // Write the new value in the compute value table
        vobsSTAR::SetProperty(POS_GAL_LON_ID, gLon);
    }

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

    mcsFLOAT mgB, mgV, mgR, mgI, mgJ, mgH, mgK, mgL, mgM;
    mcsFLOAT paralax;
    mcsFLOAT gLat, gLon;
    mcsFLOAT e_b_v=99.99;


    // Get the value of the magnitude in the different band
    if (GetProperty(PHOT_JHN_B_ID, &mgB) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_V_ID, &mgV) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_R_ID, &mgR) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_I_ID, &mgI) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_J_ID, &mgJ) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_H_ID, &mgH) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_K_ID, &mgK) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_L_ID, &mgL) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_M_ID, &mgM) == FAILURE)
    {
        return FAILURE;
    }
    // Get the value of the paralax, the galactic coordinates, and the
    // interstellar extinction if it exists.
    if (GetProperty(POS_PARLX_TRIG_ID, &paralax) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(POS_GAL_LAT_ID, &gLat) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(POS_GAL_LON_ID, &gLon) == FAILURE)
    {
        return FAILURE;
    }

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

    // Print in the calibrators properties the magnitude
    SetProperty(MO_ID, mgM);
    SetProperty(LO_ID, mgL);
    SetProperty(KO_ID, mgK);
    SetProperty(HO_ID, mgH);
    SetProperty(JO_ID, mgJ);
    SetProperty(IO_ID, mgI);
    SetProperty(RO_ID, mgR);
    SetProperty(VO_ID, mgV);
    SetProperty(BO_ID, mgB);

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

    if (GetProperty(PHOT_JHN_B_ID, &mgB) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_V_ID, &mgV) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_R_ID, &mgR) == FAILURE)
    {
        return FAILURE;
    }
    if (GetProperty(PHOT_JHN_K_ID, &mgK) == FAILURE)
    {
        return FAILURE;
    }

    if (alxComputeAngularDiameter(&angularDiameter,
                                  &angularDiameterError,
                                  mgB,
                                  mgV,
                                  mgR,
                                  mgK) == FAILURE)
    {
        return FAILURE;
    }
    // Set compute value of the angular diameter
    SetProperty(ANGULAR_DIAMETER_ID, angularDiameter);
    SetProperty(ANGULAR_DIAMETER_ERROR_ID, angularDiameterError);

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
    GetProperty(ANGULAR_DIAMETER_ID, &angularDiameter) ;
    GetProperty(ANGULAR_DIAMETER_ERROR_ID, &angularDiameterError);
    
    // Get calue of base max and wavelength
    GetProperty(INST_WAVELENGTH_VALUE_ID, &wavelength);
    
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
    SetProperty(VISIBILITY_ID, visibility2);
    SetProperty(VISIBILITY_ERROR_ID, visibilityError2);
    
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

    SetProperty(MULTIPLICITY_ID, "1.0");
    
    return SUCCESS;
}


/*___oOo___*/

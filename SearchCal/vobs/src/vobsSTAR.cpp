/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.cpp,v 1.2 2004-12-06 13:27:41 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR class definition.
 */


static char *rcsId="@(#) $Id: vobsSTAR.cpp,v 1.2 2004-12-06 13:27:41 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <math.h>
#include <iostream>
using namespace std;  
/**
 * Export standard iostream objects (cin, cout,...).
 */

#define MODULE_ID "vobs"

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/* Local variables */
static char *ucdNameList[] =
{
   "DATA_LINK",
   "ID_MAIN",
   "POS_EQ_RA_MAIN",
   "POS_EQ_DEC_MAIN",
   "POS_EQ_PMDEC",
   "POS_EQ_PMRA",
   "POS_PARLX_TRIG",
   "SPECT_TYPE_MK",
   "CODE_VARIAB",
   "CODE_MULT_FLAG",
   "POS_GAL_LAT",
   "POS_GAL_LON",
   "VELOC_HC",
   "EXTENSION_DIAM",
   "OBS_METHOD",
   "INST_WAVELENGTH_VALUE",
   "PHOT_FLUX_IR_MISC",
   "UNITS",
   "PHOT_JHN_U",
   "PHOT_JHN_B",
   "PHOT_JHN_V",
   "PHOT_JHN_R",
   "PHOT_JHN_I",
   "PHOT_JHN_J",
   "PHOT_JHN_H",
   "PHOT_JHN_K",
   "PHOT_JHN_L",
   "PHOT_JHN_M",
   "PHOT_IR_N_10_4",
   "VELOC_ROTAT",
   "PHOT_COLOR_EXCESS",
   NULL
};

/*
 * Class constructor
 */

/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        strcpy(_properties[ucdId], vobsSTAR_PROP_NOT_SET);
    }
}
/**
 * Build a star object from another one (copy constructor).
 */
vobsSTAR::vobsSTAR(const vobsSTAR &star)
{
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        star.GetProperty((vobsUCD_ID)ucdId, (char *)_properties[ucdId]);
    }
}

/*
 * Class destructor
 */

/** Delete a star object. */
vobsSTAR::~vobsSTAR()
{
}

/*
 * Public methods
 */

/**
 * Set a star property.
 *
 * Set value property corresponding to the UCD
 * 
 * \param ucd UCD name. 
 * \param value property value to set
 * \param overwrite booleen to know if it is an overwrite property 
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_NAME
 */
mcsCOMPL_STAT vobsSTAR::SetProperty(char *ucd, char *value, 
                                    mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR::SetProperty()");

    // Get Id corresponding to the specified UCD
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(ucd);
    if (ucdId == UNKNOWN_UCD_ID)
    {
        // Because catalogs contain lots of star properties which are not
        // needed, error is ignored to avoid useless overhead.
        return SUCCESS;
        //errAdd(vobsERR_INVALID_UCD_NAME, ucd);
        //return FAILURE;
    }
    
    // Affect property value
    if ((IsPropertySet(ucdId) == mcsFALSE) || (overwrite==mcsTRUE))
    {
        strcpy(_properties[ucdId], value);
    }

    return SUCCESS;    
}

/**
 * Set a star property.
 *
 * Set value property corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to set
 * \param overwrite booleen to know if it is an overwrite property 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_ID
 *
 */
mcsCOMPL_STAT vobsSTAR::SetProperty(vobsUCD_ID ucdId, char *value, 
                                    mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR::SetProperty()");

    if (ucdId == UNKNOWN_UCD_ID)
    {
        errAdd(vobsERR_INVALID_UCD_ID, ucdId);
        return FAILURE;
    }

    // Affect property value
    if ((IsPropertySet(ucdId) == mcsFALSE) || (overwrite==mcsTRUE))
    {
        strcpy(_properties[ucdId], value);
    }

    return SUCCESS;    
}
/**
 * Set a star property.
 *
 * Set value property corresponding to the ucd
 * 
 * \param id UCD identificator. 
 * \param value property value to set
 * \param overwrite indicate whether a value already set has to be overwritten  
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li sclsvrERR_INVALID_UCD_ID
 */
mcsCOMPL_STAT vobsSTAR::SetProperty(vobsUCD_ID id,
                                    mcsFLOAT value,
                                    mcsLOGICAL overwrite)
{
    logExtDbg("sclsvrCALIBRATOR::SetProperty()");

    // If it is not a valid property id, return error
    if ((id <= UNKNOWN_UCD_ID) ||
        (id >= vobsNB_STAR_PROPERTIES))
    {
        errAdd(vobsERR_INVALID_UCD_ID, id);
        return FAILURE;
    }
    // Affect property value
    if ((IsPropertySet(id) == mcsFALSE) || (overwrite == mcsTRUE))
    { 
        sprintf(_properties[id], "%f", value);
    }

    return SUCCESS;    
}

/**
 * Get a star property, as a string.
 *
 * Get value property, as a string, corresponding to the UCD
 * 
 * \param ucd UCD name. 
 * \param value property value to get, as a string.
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_NAME
 *
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(char *ucd, char *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Get Id corresponding to the specified UCD
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(ucd);
    if (ucdId == UNKNOWN_UCD_ID)
    {
        errAdd(vobsERR_INVALID_UCD_NAME, ucd);
        return FAILURE;
    }

    // Copy property value
    strcpy(value, _properties[ucdId]);

    return SUCCESS;
}

/**
 * Get a star property, as an integer.
 *
 * Get value property, as an integer, corresponding to the UCD
 * 
 * \param ucd UCD name. 
 * \param value property value to get, as an integer. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(char *ucd, int *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Get Id corresponding to the specified UCD
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(ucd);

    // Retrieve and return property value
    return (GetProperty(ucdId, value));
}

/**
 * Get a star property, as an integer.
 *
 * Get value property, as an integer, corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to get, as an integer. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_ID
 * \li vobsERR_INVALID_UCD_FORMAT
 *
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(vobsUCD_ID ucdId, int *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Check UCD id
    if ((id <= UNKNOWN_UCD_ID) ||
        (id >= vobsNB_STAR_PROPERTIES))
    {
        errAdd(vobsERR_INVALID_UCD_ID, ucdId);
        return FAILURE;
    }

    // Convert property string value to integer value
    if (sscanf(_properties[ucdId], "%d", value) != 1)
    {
        errAdd(vobsERR_INVALID_UCD_FORMAT, _properties[ucdId], "integer");
        return FAILURE;
    }
        
    return SUCCESS;
}

/**
 * Get a star property, as a float.
 *
 * Get value property corresponding, as a float, to the UCD
 * 
 * \param ucd UCD name. 
 * \param value property value to get, as a float. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(char *ucd, float *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Get Id corresponding to the specified UCD
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(ucd);

    // Retrieve property value
    return (GetProperty(ucdId, value));
}

/**
 * Get a star property, as a float.
 *
 * Get value property, as a float, corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to get, as a float. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_ID
 * \li vobsERR_INVALID_UCD_FORMAT
 *
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(vobsUCD_ID ucdId, float *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");
    
    // Check UCD id
    if ((id <= UNKNOWN_UCD_ID) ||
        (id >= vobsNB_STAR_PROPERTIES))
    {
        errAdd(vobsERR_INVALID_UCD_ID, ucdId);
        return FAILURE;
    }
    
    // Convert property string value to float value
    if (sscanf(_properties[ucdId], "%f", value) != 1)
    {
        errAdd(vobsERR_INVALID_UCD_FORMAT, _properties[ucdId], "float");
        return FAILURE;
    }
        
    return SUCCESS;
}
/**
 * Get a star property, as a string.
 *
 * Get value property, as a string, corresponding to the UCD
 * 
 * \param ucdId UCD id. 
 * \param value property value to get, as a string. 
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_UCD_ID
 */
mcsCOMPL_STAT vobsSTAR::GetProperty(vobsUCD_ID ucdId, char *value) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Check UCD id
    if ((id <= UNKNOWN_UCD_ID) ||
        (id >= vobsNB_STAR_PROPERTIES))
    {
        errAdd(vobsERR_INVALID_UCD_ID, ucdId);
        return FAILURE;
    }
   
    // Affect string value form the property
    strcpy(value, _properties[ucdId]);
            
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
mcsLOGICAL vobsSTAR::IsPropertySet(char *ucd) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Get Id corresponding to the specified UCD
    vobsUCD_ID ucdId;
    ucdId = Ucd2Id(ucd);

    // Retrieve and return property value
    return (IsPropertySet(ucdId));
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
mcsLOGICAL vobsSTAR::IsPropertySet(vobsUCD_ID ucdId) const
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Check UCD id
    if ((id <= UNKNOWN_UCD_ID) ||
        (id >= vobsNB_STAR_PROPERTIES))
    {
        return mcsFALSE;
    }

    // Check if property string value is set to vobsSTAR_PROP_NOT_SET
    if (strcmp(_properties[ucdId], vobsSTAR_PROP_NOT_SET) == 0)
    {
        return mcsFALSE;
    }
    else
    {
        return mcsTRUE;
    }
}

/**
 * Get right ascension (ra) in arcseconds.
 *
 * \param ra right ascension "HH MM SS.TTT".
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetRa(float &ra)
{
    logExtDbg("vobsSTAR::GetRa()");
    
    char *raHms;
    float hh,hm,hs; 

    // Chech RA has been set
    raHms = _properties[POS_EQ_RA_MAIN_ID];
    if (strcmp(raHms, vobsSTAR_PROP_NOT_SET)==0)
    {
        errAdd(vobsERR_RA_NOT_SET);
        return FAILURE;
    }

    // Get hours, min and sec 
    if (sscanf(raHms, "%f %f %f", &hh, &hm, &hs) != 3)
    {
        errAdd(vobsERR_INVALID_RA_FORMAT, raHms);
        return FAILURE;
    }

    // Convert to degrees
    ra  = (hh + hm/60.0 + hs/3600.0)*15.0;

    // Set angle range [-180 - 180]
    if (ra > 180)
    {
        ra=-(360-ra);
    }
    
    //logTest("RA (deg) : %.5f", ra); 

    return SUCCESS;
}


/**
 * Get declinaison (dec) in arcseconds.
 *
 * \param dec declinaison.
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetDec(float &dec)
{
    logExtDbg("vobsSTAR::GetDec()");
    
    char *decDms;
    float dd,dm,ds; 

    // Chech DEC has been set
    decDms = _properties[POS_EQ_DEC_MAIN_ID];
    if (strcmp(decDms, vobsSTAR_PROP_NOT_SET)==0)
    {
        errAdd(vobsERR_DEC_NOT_SET);
        return FAILURE;
    }

    // Get hours, min and sec 
    if (sscanf(decDms, "%f %f %f", &dd, &dm, &ds) != 3)
    {
        errAdd(vobsERR_INVALID_DEC_FORMAT, decDms);
        return FAILURE;
    }

    // Convert to degrees
    dec = dd + ((dm + ds / 60) / 60);
    if (dec>180)
    {
        dec=-(360-dec);
    }
   
    //logTest("DEC (deg) : %.5f", dec); 
    return SUCCESS;
}


/**
 * Test if this star is the same than another one.
 *
 * \param star the other star.
 *
 * \return TRUE on successful completion. Otherwise FALSE is returned.
 *
 */
mcsLOGICAL vobsSTAR::IsSame(vobsSTAR &star)
{
    logExtDbg("vobsSTAR::IsSame()");
    
    mcsSTRING32 valueRA;
    mcsSTRING32 valueDEC;
    
    star.GetProperty(POS_EQ_RA_MAIN_ID, valueRA);
    star.GetProperty(POS_EQ_DEC_MAIN_ID, valueDEC);
    
    if ((strcmp(_properties[POS_EQ_RA_MAIN_ID], valueRA) !=0)
        || (strcmp(_properties[POS_EQ_DEC_MAIN_ID], valueDEC) !=0))
    {
        return mcsFALSE;
    }
    return mcsTRUE;
}

/**
 * Test if this star is the same than another one with the coordinate and an
 * interval.
 *
 * \param star the other star.
 * \param intervalRa interval of rihght ascension
 * \param intervalDec interval of declinaison
 * 
 * \return TRUE on successful completion. Otherwise FALSE is returned.
 *
 */
mcsLOGICAL vobsSTAR::IsSameCoordinate(vobsSTAR &star,
                                      float intervalRa,
                                      float intervalDec)
{
    logExtDbg("vobsSTAR::IsSameCoordonate()");
    
    mcsFLOAT valueRA1, valueRA2;
    mcsFLOAT valueDEC1, valueDEC2;
   

    GetRa(valueRA1);
    GetDec(valueDEC1);
    
    star.GetRa(valueRA2);
    star.GetDec(valueDEC2);
    
    if ((fabs(valueRA1-valueRA2) > intervalRa) ||
        (fabs(valueDEC1-valueDEC2) > intervalDec) )
    {
        return mcsFALSE;
    }
    return mcsTRUE;
}

/**
 * Test if this star is the same than another one with hipparcos number.
 *
 * \param star the other star.
 *
 * \return TRUE on successful completion. Otherwise FALSE is returned.
 *
 */
mcsLOGICAL vobsSTAR::IsSameHip(vobsSTAR &star)
{
    logExtDbg("vobsSTAR::IsSameHip()");
    
    mcsSTRING32 valueHIP;
    
    star.GetProperty(ID_MAIN_ID, valueHIP);
    
    if (strcmp(_properties[ID_MAIN_ID], valueHIP) !=0)
    {
        return mcsFALSE;
    }
    return mcsTRUE;
}

/**
 * Update a star from another one.
 *
 * \param star the other star.
 *
 * \return TRUE on successful completion. Otherwise FALSE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::Update (vobsSTAR &star)
{
    logExtDbg("vobsSTAR::Update()");
   
    mcsSTRING32 value; 
    
    // Compare each property with the propety of the other star
    for (int i=0; i<vobsNB_STAR_PROPERTIES; i++)
    {
        star.GetProperty((vobsUCD_ID)i, value);
        // Replace the property by the propery of the other star
        // if the first property has no value
        if ((strcmp(_properties[i], vobsSTAR_PROP_NOT_SET) == 0) &&
            (strcmp(value, vobsSTAR_PROP_NOT_SET) != 0))    
        {
           SetProperty((vobsUCD_ID)i, value);
        }
    }
    
    return SUCCESS;
}


/**
 * Display all star properties on the console.
 * 
 */
void vobsSTAR::Display(void)
{
    logExtDbg("vobsSTAR::Display()");
    cout << "-----------------------------------------" << endl;
    /*for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
        printf("%20s",ucdNameList[ucdId]);
    }
    printf("\n");*/
    for (int ucdId=0; ucdId<vobsNB_STAR_PROPERTIES; ucdId++)
    {
         printf("%12s", _properties[ucdId]);
    }
    printf("\n");

}

/*
 * Protected methods
 */

/**
 * Convert a given UCD to an id.
 *
 * \param ucd UCD name.
 *
 * \return an UCD id.
 *
 */
vobsUCD_ID vobsSTAR::Ucd2Id(char *ucd) const
{
    logExtDbg("vobsSTAR::Ucd2Id()");
    
    for (unsigned int i=0; i<vobsNB_STAR_PROPERTIES; i++)
    {
        if (strcmp(ucdNameList[i], ucd) == 0)
        {
            return (vobsUCD_ID)i;
        }
    }

    return UNKNOWN_UCD_ID;
}
/*___oOo___*/

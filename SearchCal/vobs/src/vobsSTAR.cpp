/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.cpp,v 1.27 2005-02-10 08:07:38 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.26  2005/02/08 20:43:11  gzins
* Changed _isComputed by _origin
*
* Revision 1.25  2005/02/08 07:22:27  gzins
* Changed value parameter type: char* to const char*
* Accepted RA and DEC with either ':' or ' ' as separator
*
* Revision 1.24  2005/02/07 09:14:00  gzins
* Fixed wrong type for isComputed parameter; mcsFLOAT instead of mcsLOGICAL
*
* Revision 1.23  2005/02/04 14:31:30  scetre
* add getLuminosityClass method
*
* Revision 1.22  2005/02/04 09:50:12  gzins
* Fixed major bug in RA and DEC convertion (sign was not correctly handled)
* Merged IRAS and MSX IR flux properties
*
* Revision 1.21  2005/02/04 08:09:28  gzins
* Added star properties for MIDI catalog
*
* Revision 1.20  2005/02/04 07:44:43  gzins
* Limited number of logged messages for test
*
* Revision 1.19  2005/01/27 13:44:41  scetre
* update documentation in IsSame method
*
* Revision 1.18  2005/01/26 14:11:42  scetre
* change assignement operator and pass list as private member of the class vobsSTAR
*
* Revision 1.17  2005/01/26 08:50:25  scetre
* upated documentation
*
* Revision 1.16  2005/01/26 08:17:54  scetre
* Change History, add documentation, add checking method on property.
* Change problem (vobsSTAR_POS_EQ_RA_MAIN -> ...DEC_MAIN). Update IsSame Method
*
* scetre    22-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR class definition.
 */


static char *rcsId="@(#) $Id: vobsSTAR.cpp,v 1.27 2005-02-10 08:07:38 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/*
 * System Headers
 */
#include <math.h>
#include <iostream>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers
 */
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    // Add all star properties
    AddProperties();
    _propertyListIterator=_propertyList.begin();
    _propertyOrderIterator=_propertyOrder.begin();
}

/**
 * Build a star object from another one (copy constructor).
 */
vobsSTAR::vobsSTAR(vobsSTAR &star)
{
    *this = star;
}

/*
 * Assignement operator
 */
vobsSTAR&vobsSTAR::operator=(const vobsSTAR&star)
{
    logExtDbg("vobsSTAR::operator=()");
   
    // clear the 2 map used in this class
    _propertyList.erase(_propertyList.begin(), _propertyList.end());
    _propertyOrder.erase(_propertyOrder.begin(), _propertyOrder.end());
    
    _propertyList = star._propertyList;
    _propertyOrder = star._propertyOrder;
    return *this;
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
 * Set a star property value.
 *
 * Set value corresponding to the given property id
 *
 * \param id property id.
 * \param value property value to set
 * \param isComputed booleen to know if it is a computed property
 * \param confidenceIndex confidence index
 * \param overwrite booleen to know if it is an overwrite property
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char *id, const char *value,
                                         const char *origin,
                                         mcsINT32 confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR::SetPropertyValue()");

    // Look for property
    map<string, vobsSTAR_PROPERTY> ::iterator propertyIter;
    propertyIter = _propertyList.find(id);
    // If found
    if (propertyIter != _propertyList.end())
    {
        // Set property
        propertyIter->second.SetValue(value, origin,
                                      confidenceIndex,
                                      overwrite);
        return mcsSUCCESS;
    }
    // Else
    else
    {
        // Return error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return mcsFAILURE;
    }
    // End if
}

/**
 * Set a star property.
 *
 * Set value corresponding to the given property id
 *
 * \param id property id.
 * \param value property value to set
 * \param isComputed booleen to know if it is a computed property
 * \param confidenceIndex confidence index
 * \param overwrite booleen to know if it is an overwrite property
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char *id, mcsFLOAT value,
                                         const char *origin,
                                         mcsINT32 confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR::SetPropertyValue()");

    // Look for property
    map<string, vobsSTAR_PROPERTY> ::iterator propertyIter;
    propertyIter = _propertyList.find(id);
    // If found
    if (propertyIter != _propertyList.end())
    {
        // Set property
        propertyIter->second.SetValue(value, origin, confidenceIndex, 
                                      overwrite);
        return mcsSUCCESS;
    }
    // Else
    else
    {
        // Return error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return mcsFAILURE;
    }
    // End if
}

/**
 * Get a star property.
 *
 * Set property value corresponding to the UCD
 *
 * \param id property id.
 *
 * \return pointer to the found star property object on successful completion.
 * Otherwise NULL is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_INVALID_PROPERTY_ID
 */
vobsSTAR_PROPERTY *vobsSTAR::GetProperty(char *id)
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Look for property
    map<string, vobsSTAR_PROPERTY> ::iterator propertyIter;
    propertyIter = _propertyList.find(id);
    // If found
    if (propertyIter != _propertyList.end())
    {
        // Return property
        return &propertyIter->second;
    }
    // Else
    else
    {
        // Return error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return NULL;
    }
    // End if
}

/**
 * Return the next property in the list
 *
 * This method returns the pointer to the next element of the list. If \em
 * init is mcsTRUE, it returns the first element of the list.
 *
 * This method can be used to move forward in the list, as shown below:
 * \code
 *     for (unsigned int el = 0; el < star.NbProperties(); el++)
 *     {
 *         printf("%s",star.GetNextProperty((mcsLOGICAL)(el==0))->GetName());
 *     }
 * \endcode
 *
 * \return pointer to the next element of the list or NULL if the end of the
 * list is reached.
 */
vobsSTAR_PROPERTY *vobsSTAR::GetNextProperty(mcsLOGICAL init)
{
    logExtDbg("vobsSTAR::GetNextProperty()");

    // if the logical value of the parameter, init is mcsTRUE, the wanted value
    // is the first
    if (init == mcsTRUE)
    {
        _propertyOrderIterator=_propertyOrder.begin();
    }
    // else increase the iterator. It is checking if the iterator stay in the
    // list and don't go further than the end of the list. If it is the case,
    // the pointer which is gave is NULL
    else
    {
        _propertyOrderIterator++;
        if ( _propertyOrderIterator == _propertyOrder.end())
        {
            return NULL;
        }
    }

    // Once the iterator is put on the correct property, the value of the
    // property is returned
    _propertyListIterator = _propertyList.find(_propertyOrderIterator->second);

    return (&(_propertyListIterator->second));
}


/**
 * Get a star property.
 *
 * Get value as string corresponding to the given property id.
 *
 * \param id property id.
 *
 * \return pointer to the found star property value on successful completion.
 * Otherwise NULL is returned.
 */
const char *vobsSTAR::GetPropertyValue(char *id)
{
    logExtDbg("vobsSTAR::GetPropertyValue()");

    // Look for property
    vobsSTAR_PROPERTY *property;
    property = GetProperty(id);
    // If found
    if (property != NULL)
    {
        // Return property
        return (property->GetValue());
    }
    // Else
    else
    {
        // Return error
        return NULL;
    }
    // End if
}

/**
 * Get a star property.
 *
 * Get value as string corresponding to the given property id.
 *
 * \param id property id.
 * \param value pointer to store value.
 *
 * \return pointer to the found star property value on successful completion.
 * Otherwise NULL is returned.
 */
mcsCOMPL_STAT vobsSTAR::GetPropertyValue(char *id, mcsFLOAT *value)
{
    logExtDbg("vobsSTAR::GetProperty()");

    // Look for property
    vobsSTAR_PROPERTY *property;
    property = GetProperty(id);
    // If found
    if (property != NULL)
    {
        // Return property
        return (property->GetValue(value));
    }
    // Else
    else
    {
        // Return error
        return mcsFAILURE;
    }
    // End if
}

/**
 * Check whether the property is set or not.
 *
 * \param id property id.
 *
 * \warning
 * If the given property id is unknown, this method returns false (i.e.
 * mcsFALSE)
 *
 * \return
 * True value (i.e. mcsTRUE) if the the property has been set, false (i.e.
 * mcsFALSE) otherwise.
 */
mcsLOGICAL vobsSTAR::IsPropertySet(char *id)
{
    logExtDbg("vobsSTAR::IsPropertySet()");

    // Look for property
    vobsSTAR_PROPERTY *property;
    property = GetProperty(id);
    // If found
    if (property != NULL)
    {
        // Return property
        return (property->IsSet());
    }
    // Else
    else
    {
        // Return error
        return mcsFALSE;
    }
    // End if
}

/**
 * Check whether a property is a property.
 *
 * \param id property id.
 *
 *
 * \return
 * True value (i.e. mcsTRUE) if the the property is known, false (i.e.
 * mcsFALSE) otherwise.
 */
mcsLOGICAL vobsSTAR::IsProperty(char *id)
{
    logExtDbg("vobsSTAR::IsPropertySet()");

    // Look for property
    // If found
    if (GetProperty(id) != NULL)
    {
        // Return mcsTRUE
        return mcsTRUE;
    }
    // Else
    else
    {
        // Return error
        return mcsFALSE;
    }
    // End if
}


/**
 * Get right ascension (ra) in arcseconds.
 *
 * \param ra pointer right ascension.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetRa(float &ra)
{
    logExtDbg("vobsSTAR::GetRa()");

    mcsSTRING64 raHms;
    mcsFLOAT    hh, hm, hs;

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_RA_NOT_SET);
        return mcsFAILURE;
    }

    // RA can be given as HH:MM:SS.TT or HH MM SS.TT. 
    // Replace ':' by ' '
    strcpy(raHms, GetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN));
    miscReplaceChrByChr(raHms, ':', ' ');
    if (sscanf(raHms, "%f %f %f", &hh, &hm, &hs) != 3)
    {
        errAdd(vobsERR_INVALID_RA_FORMAT, raHms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsFLOAT sign;
    sign = (hh >= 0) ? 1.0 : -1.0;

    // Convert to degrees
    ra  = (hh + sign*hm/60.0 + sign*hs/3600.0) * 15.0;

    // Set angle range [-180 - 180]
    if (ra > 180)
    {
        ra = -1.0 * (360 - ra);
    }

    return mcsSUCCESS;
}

/**
 * Get declinaison (dec) in arcseconds.
 *
 * \param dec declinaison.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsSTAR::GetDec(float &dec)
{
    logExtDbg("vobsSTAR::GetDec()");

    mcsSTRING64 decDms;
    float dd,dm,ds;

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_DEC_NOT_SET);
        return mcsFAILURE;
    }

    // DEC can be given as DD:MM:SS.TT or DD MM SS.TT. 
    // Replace ':' by ' '
    strcpy(decDms, GetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN));
    miscReplaceChrByChr(decDms, ':', ' ');
    if (sscanf(decDms, "%f %f %f", &dd, &dm, &ds) != 3)
    {
        errAdd(vobsERR_INVALID_DEC_FORMAT, decDms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsFLOAT sign;
    sign = (dd >= 0) ? 1.0 : -1.0;
    
    // Convert to degrees
    dec  = dd + sign*dm/60.0 + sign*ds/3600.0;

    return mcsSUCCESS;
}

/**
 *
 */
mcsCOMPL_STAT vobsSTAR::GetSpectralClass(char *spectralClass)
{
    logExtDbg("vobsSTAR::GetSpectralClass()");
    // Get the spectral type of the star
    mcsSTRING64 spectralType;
    strcpy(spectralType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));
    
    char        code;          /* Code of the spectral type */
    mcsFLOAT    quantity;      /* Quantity of the spectral subtype */
    mcsSTRING32 lightClass;    /* Luminosity class*/
    mcsINT32 nbItems;
    nbItems = sscanf(spectralType, "%c%f%s",
                     &code, 
                     &quantity, 
                     lightClass);

    // if the spectral class of the spectral type doesn't exist, return error
    if ((nbItems < 1) || (nbItems > 3))
    {
        // todo err
        return mcsFAILURE;
    }
    // if it exist (mbItems > 1)
    strcpy(spectralClass, &code);
    return mcsSUCCESS;
}

/**
 *
 */
mcsCOMPL_STAT vobsSTAR::GetLuminosityClass(char *LuminosityClass)
{
    logExtDbg("vobsSTAR::GetLuminosityClass()");
    // Get the luminosity class of the star
    mcsSTRING64 spectralType;
    strcpy(spectralType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));
    
    char        code;          /* Code of the spectral type */
    mcsFLOAT    quantity;      /* Quantity of the spectral subtype */
    mcsSTRING32 lightClass;    /* Luminosity class*/

    mcsINT32 nbItems;
    nbItems = sscanf(spectralType, "%c%f%s",
                     &code, 
                     &quantity, 
                     lightClass);

    // if the luminosity class of the spectral type doesn't exist, return error
    if (nbItems != 3)
    {
        // todo err
        return mcsFAILURE;
    }
    // if it exist (mbItems = 3)
    strcpy(LuminosityClass, lightClass);
    
    return mcsSUCCESS;
}


/**
 * Test if this star is the same than another one.
 *
 * Check that the star coordinates are the same.
 *
 * \param star the other star.
 * \param criteriaList the list of comparaison criteria
 *
 * \return TRUE if star positions are the same. Otherwise FALSE is returned.
 */
mcsLOGICAL vobsSTAR::IsSame(vobsSTAR &star,
                            vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    logExtDbg("vobsSTAR::IsSame(0x%x)", criteriaList);

    // Check if the criteria list is empty
    if (criteriaList == NULL)
    {
        mcsFLOAT ra1, ra2, dec1, dec2;
        // Get right ascension of the star. If not set return FALSE
        if (GetRa(ra1) == mcsFAILURE)
        {
            errResetStack();
            return mcsFALSE;
        }
        if (star.GetRa(ra2) == mcsFAILURE)
        {
            errResetStack();
            return mcsFALSE;
        }
        // Get declinaison of the star. If not set return FALSE
        if (GetDec(dec1) == mcsFAILURE)
        {
            errResetStack();
            return mcsFALSE;
        }
        if (star.GetDec(dec2) == mcsFAILURE)
        {
            errResetStack();
            return mcsFALSE;
        }
        // Compare coordinates
        if ((ra1 == ra2) && (dec1==dec2))
        {
            return mcsTRUE;
        }
        else
        {
            return mcsFALSE;
        }
    }
    // if criteria list is not empty
    else
    {
        mcsSTRING64 propertyId;
        mcsFLOAT range;
        // Get the size of the criteria list
        int listSize=criteriaList->Size();
        mcsFLOAT val1, val2;    
        // Get each criteria of the list and check if the comparaison with all
        // this criteria gave a equality
        for (int el = 0; el < listSize; el++)
        {
            if (criteriaList->GetNextCriteria
                (propertyId, &range, (mcsLOGICAL)(el==0)) == mcsFAILURE)
            {
                return mcsFALSE;
            }
            logDebug("%s delta is in +/- %.3f?", propertyId, range);
            if (strcmp(propertyId, vobsSTAR_POS_EQ_RA_MAIN) == 0)
            {
                // Get right ascension of the stars. If not set return FALSE
                if (GetRa(val1) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
                if (star.GetRa(val2) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
            }
            else if(strcmp(propertyId, vobsSTAR_POS_EQ_DEC_MAIN) == 0)
            {
                // Get declinaison of the stars. If not set return FALSE
                if (GetDec(val1) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
                if (star.GetDec(val2) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
            }
            else
            {
                // Get value of the property id
                if (GetPropertyValue(propertyId, &val1) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
                if (star.GetPropertyValue(propertyId, &val2) == mcsFAILURE)
                {
                    errResetStack();
                    return mcsFALSE;
                }
            }
            logDebug("%s delta = %.3f", propertyId, fabs(val1-val2));
            if (fabs(val1-val2) > range)
            {
                return mcsFALSE;
            }
        }
        return mcsTRUE;
    }
}

/**
 * Update a star from another one.
 *
 * \param star the other star.
 *
 * \return always SUCCESS.
 *
 */
mcsCOMPL_STAT vobsSTAR::Update (vobsSTAR &star)
{
    logExtDbg("vobsSTAR::Update()");

    // For each star property
    map<string, vobsSTAR_PROPERTY > ::iterator propertyIter;
    for (propertyIter  = _propertyList.begin();
         propertyIter != _propertyList.end() ; propertyIter++)
    {
        if (IsPropertySet((char *)(*propertyIter).first.c_str()) == mcsFALSE)
        {
            // Copy property form the given star
            _propertyList[(*propertyIter).first] =
                star._propertyList[(*propertyIter).first];
        }
    }

    return mcsSUCCESS;
}

/**
 * method to get the number of properties
 *
 * \return the number of properties of the star
 */
mcsINT32 vobsSTAR::NbProperties()
{
    return _propertyList.size();
}

/**
 * Display all star properties on the console.
 *
 */
void vobsSTAR::Display(void)
{
    logExtDbg("vobsSTAR::Display()");
    map<string, vobsSTAR_PROPERTY > ::iterator propertyIter;

    for (propertyIter  = _propertyList.begin();
         propertyIter != _propertyList.end();
         propertyIter++)
    {
         printf("%12s", (*propertyIter).second.GetValue());
    }
    printf("\n");

}

/*
 * Protected methods
 */

/**
 * Add a star property
 *
 * \param id     property identifier
 * \param name   property name
 * \param type   property type
 * \param format format used to set property
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible error is :
 * \li vobsERR_DUPLICATED_PROPERTY
 */
mcsCOMPL_STAT vobsSTAR::AddProperty(char *id, char *name,
                                    vobsPROPERTY_TYPE type, char *format)
{
    //logExtDbg("vobsSTAR::AddProperty()");

    // Check for duplicated property
    if (_propertyList.find(id) != _propertyList.end())
    {
        errAdd(vobsERR_DUPLICATED_PROPERTY, id);
        return mcsFAILURE;
    }

    vobsSTAR_PROPERTY property(id, name, type, format);
    _propertyList[id] = property;
    _propertyOrder[_propertyList.size()-1] = id;

    return mcsSUCCESS;
}

/**
 * Add all star properties and fix an order
 *
 * \return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR::AddProperties(void)
{
    //logExtDbg("vobsSTAR::AddProperties()");
    AddProperty(vobsSTAR_ID_HD, "hd", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_ID_HIP, "hip", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_ID_DM, "dm", vobsSTRING_PROPERTY);    
    AddProperty(vobsSTAR_POS_EQ_RA_MAIN, "ra", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_EQ_DEC_MAIN, "dec", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_EQ_PMDEC, "pmdec", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_EQ_PMRA, "pmra", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY);
    AddProperty(vobsSTAR_SPECT_TYPE_MK, "tsp", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_CODE_VARIAB, "varflag", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_CODE_MULT_FLAG, "multflag", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_GAL_LAT, "glat", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_POS_GAL_LON, "glon", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_VELOC_HC, "radvel", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_EXTENSION_DIAM, "diam", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_EXTENSION_DIAM_ERROR, "diamErr", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_OBS_METHOD, "meth", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_INST_WAVELENGTH_VALUE, "wlen", vobsFLOAT_PROPERTY);
    AddProperty(vobsSTAR_PHOT_FLUX_IR_MISC, "photflux", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_UNITS, "units", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_PHOT_JHN_U, "U", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_R, "R", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_I, "I", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_J, "J", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_VELOC_ROTAT, "velocrotat", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_PHOT_COLOR_EXCESS, "color", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx",  vobsFLOAT_PROPERTY, 
                "%.3f");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_12, "F12",  vobsFLOAT_PROPERTY, 
                "%.3f");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12",  
                vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_REF_STAR, "Calib", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC, "Teff", vobsFLOAT_PROPERTY, 
                "%.3f");
    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC_ERROR, "e_Teff", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_PHOT_EXTINCTION_TOTAL, "A_V", vobsFLOAT_PROPERTY, 
                "%.3f");
    AddProperty(vobsSTAR_CHI2_QUALITY, "Chi2", vobsFLOAT_PROPERTY, "%.3f");
    AddProperty(vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC, "SpTyp_Teff", 
                vobsFLOAT_PROPERTY, "%.3f");


    return mcsSUCCESS;
}


/*___oOo___*/

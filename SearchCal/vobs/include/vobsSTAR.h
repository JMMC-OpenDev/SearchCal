#ifndef vobsSTAR_H
#define vobsSTAR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.h,v 1.21 2005-01-26 14:11:42 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.20  2005/01/26 08:10:32  scetre
* change history
*
* scetre    22-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System headers
 */
#include <map>

/*
 * Laocal headers
 */
#include "vobsSTAR_PROPERTY.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
/*
 * Constants definition
 */

#define vobsSTAR_PROP_NOT_SET  "-"  /**< Default value of the empty
                                          properties */


/*
 * Type definition
 */


/*
 * Definition of the star properties
 */
#define vobsSTAR_ID_MAIN                 "ID_MAIN"
#define vobsSTAR_ID_ALTERNATIVE          "ID_ALTERNATIVE"
#define vobsSTAR_POS_EQ_RA_MAIN          "POS_EQ_RA_MAIN"
#define vobsSTAR_POS_EQ_DEC_MAIN         "POS_EQ_DEC_MAIN"
#define vobsSTAR_POS_EQ_PMDEC            "POS_EQ_PMDEC"
#define vobsSTAR_POS_EQ_PMRA             "POS_EQ_PMRA"
#define vobsSTAR_POS_PARLX_TRIG          "POS_PARLX_TRIG"
#define vobsSTAR_SPECT_TYPE_MK           "SPECT_TYPE_MK"
#define vobsSTAR_CODE_VARIAB             "CODE_VARIAB"
#define vobsSTAR_CODE_MULT_FLAG          "CODE_MULT_FLAG"
#define vobsSTAR_POS_GAL_LAT             "POS_GAL_LAT"
#define vobsSTAR_POS_GAL_LON             "POS_GAL_LON"
#define vobsSTAR_VELOC_HC                "VELOC_HC"
#define vobsSTAR_EXTENSION_DIAM          "EXTENSION_DIAM"
#define vobsSTAR_OBS_METHOD              "OBS_METHOD"
#define vobsSTAR_INST_WAVELENGTH_VALUE   "INST_WAVELENGTH_VALUE"
#define vobsSTAR_PHOT_FLUX_IR_MISC       "PHOT_FLUX_IR_MISC"
#define vobsSTAR_UNITS                   "UNITS"
#define vobsSTAR_PHOT_JHN_U              "PHOT_JHN_U"
#define vobsSTAR_PHOT_JHN_B              "PHOT_JHN_B"
#define vobsSTAR_PHOT_JHN_V              "PHOT_JHN_V"
#define vobsSTAR_PHOT_JHN_R              "PHOT_JHN_R"
#define vobsSTAR_PHOT_JHN_I              "PHOT_JHN_I"
#define vobsSTAR_PHOT_JHN_J              "PHOT_JHN_J"
#define vobsSTAR_PHOT_JHN_H              "PHOT_JHN_H"
#define vobsSTAR_PHOT_JHN_K              "PHOT_JHN_K"
#define vobsSTAR_PHOT_JHN_L              "PHOT_JHN_L"
#define vobsSTAR_PHOT_JHN_M              "PHOT_JHN_M"
#define vobsSTAR_PHOT_IR_N_10_4          "PHOT_IR_N_10_4"
#define vobsSTAR_VELOC_ROTAT             "VELOC_ROTAT"
#define vobsSTAR_PHOT_COLOR_EXCESS       "PHOT_COLOR_EXCESS"


/*
 * Class declaration
 */

/**
 * vobsSTAR is a class which caracterises a star.
 *
 * vobsSTAR methods allow to
 * \li modify
 * \li read
 * \li update
 * \li compare
 * stars properties.
 *
 * \sa vobsSTAR_LIST
 *
 * \todo Finish to implement Update and Display methods.
 *
 */
class vobsSTAR
{
public:
    // Constructors
    vobsSTAR();
    vobsSTAR(vobsSTAR &star);

    // assignement operator =
    vobsSTAR& operator=(const vobsSTAR&);
    
    // Destructor
    virtual ~vobsSTAR();

    // Method to set the star properties
    virtual mcsCOMPL_STAT SetPropertyValue(char *id, char *value,
                                           mcsFLOAT isComputed=mcsFALSE,
                                           mcsINT32 confidenceIndex=vobsCONFIDENCE_HIGH,                                           
                                           mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetPropertyValue(char *propertyId, mcsFLOAT value,
                                           mcsFLOAT isComputed=mcsFALSE,
                                           mcsINT32 confidenceIndex=vobsCONFIDENCE_HIGH,                                           
                                           mcsLOGICAL overwrite=mcsFALSE);

    // Methods to retreive the star property
    virtual vobsSTAR_PROPERTY *GetProperty(char *id) ;
    virtual vobsSTAR_PROPERTY *GetNextProperty(mcsLOGICAL init = mcsFALSE);
    virtual const char *GetPropertyValue(char *id);
    virtual mcsCOMPL_STAT GetPropertyValue(char *id, mcsFLOAT *value);

    // Is property set?
    virtual mcsLOGICAL    IsPropertySet(char *propertyId);

    // Is a name a property?
    virtual mcsLOGICAL    IsProperty(char *propertyId);

    // Method to get RA and DEC (in arcsecond)
    virtual mcsCOMPL_STAT GetRa(float &ra);
    virtual mcsCOMPL_STAT GetDec(float &dec);

    // Method to know if another star is the same than this one
    virtual mcsLOGICAL IsSame (vobsSTAR &star,
                               vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL);

    // Method to update star properties from the given star
    virtual mcsCOMPL_STAT Update (vobsSTAR &star);

    // Method to get the number of properties
    virtual mcsINT32 NbProperties();
    
    // Method to print out all star properties
    virtual void Display(void);
    
protected:
    // Method to add a property. Should be only called by constructors.
    mcsCOMPL_STAT AddProperty(char *id, char *name,
                              vobsPROPERTY_TYPE type, char *format=NULL);
     map<string, vobsSTAR_PROPERTY> _propertyList;
    map<string, vobsSTAR_PROPERTY>::iterator _propertyListIterator;
    map<int, string> _propertyOrder;
    map<int, string>::iterator _propertyOrderIterator;

private:
    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);

    
};


#endif /*!vobsSTAR_H*/
/*___oOo___*/

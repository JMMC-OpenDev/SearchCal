#ifndef vobsSTAR_H
#define vobsSTAR_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * System headers
 */
#include "string.h"
#include <map>


/*
 * Local headers
 */
#include "vobsSTAR_PROPERTY.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

/*
 * Constants definition
 */
#define vobsSTAR_PROP_NOT_SET  "-"  /**< Default value of the empty
                                          properties */

/*
 * Definition of the star properties
 */
#define vobsSTAR_ID_HD                          "ID_HD"
#define vobsSTAR_ID_HIP                         "ID_HIP"
#define vobsSTAR_ID_DM                          "ID_DM"
#define vobsSTAR_ID_TYC1                        "ID_TYC1"
#define vobsSTAR_ID_TYC2                        "ID_TYC2"
#define vobsSTAR_ID_TYC3                        "ID_TYC3"
#define vobsSTAR_ID_CATALOG                     "ID_CATALOG"
#define vobsSTAR_ID_2MASS                       "ID_2MASS"
#define vobsSTAR_ID_DENIS                       "ID_DENIS"
#define vobsSTAR_ID_SB9                         "ID_SB9"
#define vobsSTAR_ID_WDS                         "ID_WDS"
#define vobsSTAR_POS_EQ_RA_MAIN                 "POS_EQ_RA_MAIN"
#define vobsSTAR_POS_EQ_RA_OTHER                "POS_EQ_RA_OTHER"
#define vobsSTAR_POS_EQ_DEC_MAIN                "POS_EQ_DEC_MAIN"
#define vobsSTAR_POS_EQ_DEC_OTHER               "POS_EQ_DEC_OTHER"
#define vobsSTAR_POS_EQ_PMDEC                   "POS_EQ_PMDEC"
#define vobsSTAR_POS_EQ_PMRA                    "POS_EQ_PMRA"
#define vobsSTAR_POS_PARLX_TRIG                 "POS_PARLX_TRIG"
#define vobsSTAR_POS_PARLX_TRIG_ERROR           "POS_PARLX_TRIG_ERROR"
#define vobsSTAR_SPECT_TYPE_MK                  "SPECT_TYPE_MK"
#define vobsSTAR_CODE_VARIAB_V1                 "CODE_VARIAB_V1"
#define vobsSTAR_CODE_VARIAB_V2                 "CODE_VARIAB_V2"
#define vobsSTAR_CODE_VARIAB_V3                 "VAR_CLASS"
#define vobsSTAR_CODE_BIN_FLAG                  "CODE_BIN_FLAG"
#define vobsSTAR_CODE_MULT_FLAG                 "CODE_MULT_FLAG"
#define vobsSTAR_ORBIT_SEPARATION_SEP1          "ORBIT_SEPARATION_SEP1"
#define vobsSTAR_ORBIT_SEPARATION_SEP2          "ORBIT_SEPARATION_SEP2"
#define vobsSTAR_CODE_MISC_I                    "CODE_MISC_I"
#define vobsSTAR_CODE_MISC_J                    "CODE_MISC_J"
#define vobsSTAR_CODE_MISC_K                    "CODE_MISC_K"
#define vobsSTAR_CODE_QUALITY                   "CODE_QUALITY"
#define vobsSTAR_POS_GAL_LAT                    "POS_GAL_LAT"
#define vobsSTAR_POS_GAL_LON                    "POS_GAL_LON"
#define vobsSTAR_VELOC_HC                       "VELOC_HC"
#define vobsSTAR_LD_DIAM                        "LD_DIAM"
#define vobsSTAR_LD_DIAM_ERROR                  "LD_DIAM_ERROR"
#define vobsSTAR_UD_DIAM                        "UD_DIAM"
#define vobsSTAR_UD_DIAM_ERROR                  "UD_DIAM_ERROR"
#define vobsSTAR_UDDK_DIAM                      "UDDK_DIAM"
#define vobsSTAR_UDDK_DIAM_ERROR                "UDDK_DIAM_ERROR"
#define vobsSTAR_DIAM12                         "DIAM12"
#define vobsSTAR_DIAM12_ERROR                   "DIAM12_ERROR"
#define vobsSTAR_OBS_METHOD                     "REDUCT_METHOD"
#define vobsSTAR_INST_WAVELENGTH_VALUE          "INST_WAVELENGTH_VALUE"
#define vobsSTAR_SPECT_WAVELENGTH_MISC          "SPECT_WAVELENGTH_MISC"
#define vobsSTAR_INST_FILTER_CODE               "INST_FILTER_CODE"
#define vobsSTAR_PHOT_FLUX_IR_MISC              "PHOT_FLUX_IR_MISC"
#define vobsSTAR_UNITS                          "UNITS"
#define vobsSTAR_PHOT_JHN_U                     "PHOT_JHN_U"
#define vobsSTAR_PHOT_JHN_B                     "PHOT_JHN_B"
#define vobsSTAR_PHOT_PHG_B                     "PHOT_PHG_B"
#define vobsSTAR_PHOT_JHN_V                     "PHOT_JHN_V"
#define vobsSTAR_PHOT_PHG_V                     "PHOT_PHG_V"
#define vobsSTAR_PHOT_JHN_R                     "PHOT_JHN_R"
#define vobsSTAR_PHOT_PHG_R                     "PHOT_PHG_R"
#define vobsSTAR_PHOT_JHN_I                     "PHOT_JHN_I"
#define vobsSTAR_PHOT_COUS_I                    "PHOT_COUS_I"
#define vobsSTAR_PHOT_PHG_I                     "PHOT_PHG_I"
#define vobsSTAR_PHOT_JHN_J                     "PHOT_JHN_J"
#define vobsSTAR_PHOT_JHN_H                     "PHOT_JHN_H"
#define vobsSTAR_PHOT_JHN_K                     "PHOT_JHN_K"
#define vobsSTAR_PHOT_JHN_L                     "PHOT_JHN_L"
#define vobsSTAR_PHOT_JHN_M                     "PHOT_JHN_M"
#define vobsSTAR_PHOT_JHN_N                     "PHOT_JHN_N"
#define vobsSTAR_VELOC_ROTAT                    "VELOC_ROTAT"
#define vobsSTAR_PHOT_COLOR_EXCESS              "PHOT_COLOR_EXCESS"
#define vobsSTAR_IR_FLUX_ORIGIN                 "IR_FLUX_ORIGIN"
#define vobsSTAR_POS_PARLX_TRIG_ERROR           "POS_PARLX_TRIG_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_09                "PHOT_FLUX_IR_09"
#define vobsSTAR_PHOT_FLUX_IR_09_ERROR          "PHOT_FLUX_IR_09_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_12                "PHOT_FLUX_IR_12"
#define vobsSTAR_PHOT_FLUX_IR_12_ERROR          "PHOT_FLUX_IR_12_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_18                "PHOT_FLUX_IR_18"
#define vobsSTAR_PHOT_FLUX_IR_18_ERROR          "PHOT_FLUX_IR_18_ERROR"
#define vobsSTAR_REF_STAR                       "REF_STAR"
#define vobsSTAR_PHYS_TEMP_EFFEC                "PHYS_TEMP_EFFEC"
#define vobsSTAR_PHYS_TEMP_EFFEC_ERROR          "PHYS_TEMP_EFFEC_ERROR"
#define vobsSTAR_PHOT_EXTINCTION_TOTAL          "PHOT_EXTINCTION_TOTAL"
#define vobsSTAR_CHI2_QUALITY                   "CHI2_QUALITY"
#define vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC         "SP_TYP_PHYS_TEMP_EFFEC"

/*
 * const char* comparator used by map<const char*, ...>
 */
struct cmp_str {
    bool operator()(const char* s1, const char* s2) const {
        return (s1 == s2) ? false : strcmp(s1, s2) < 0;
    }
};

/* property map type using char* keys and custom comparator functor */
typedef std::map<const char*, vobsSTAR_PROPERTY, cmp_str> PropertyMap;

/*
 * Class declaration
 */
/**
 * Store all the propreties caracterising a star.
 *
 * It allows among oher things the following actions on star properies:
 * @li read;
 * @li update;
 * @li compare.
 *
 * @sa vobsSTAR_PROPERTY
 *
 * @todo Finish to implement Update and Display methods.
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

    // Set the star property values
    virtual mcsCOMPL_STAT SetPropertyValue
                   (const char*           id,
                    const char*           value,
                    const char*           origin,
                    vobsCONFIDENCE_INDEX  confidenceIndex = vobsCONFIDENCE_HIGH, 
                    mcsLOGICAL            overwrite       = mcsFALSE);

    virtual mcsCOMPL_STAT SetPropertyValue
                   (const char*           propertyId, 
                    mcsDOUBLE             value,
                    const char*           origin,
                    vobsCONFIDENCE_INDEX  confidenceIndex = vobsCONFIDENCE_HIGH, 
                    mcsLOGICAL            overwrite       = mcsFALSE);
    
    virtual mcsCOMPL_STAT ClearPropertyValue (const char* id);

    // Get the star properties
    virtual vobsSTAR_PROPERTY* GetProperty      (const char*      id);
    virtual vobsSTAR_PROPERTY* GetNextProperty  (mcsLOGICAL init = mcsFALSE);
    virtual const char*        GetPropertyValue (const vobsSTAR_PROPERTY* property);
    virtual const char*        GetPropertyValue (const char*      id);
    virtual mcsCOMPL_STAT      GetPropertyValue (const vobsSTAR_PROPERTY* property,
                                                 mcsDOUBLE*  value);
    virtual mcsCOMPL_STAT      GetPropertyValue (const char*      id,
                                                 mcsDOUBLE*  value);
    virtual vobsPROPERTY_TYPE  GetPropertyType  (const vobsSTAR_PROPERTY* property) ;
    virtual vobsPROPERTY_TYPE  GetPropertyType  (const char*      id) ;
    virtual vobsCONFIDENCE_INDEX GetPropertyConfIndex(const char* id); 
    // Is a property set?
    virtual mcsLOGICAL         IsPropertySet    (const vobsSTAR_PROPERTY* property);
    virtual mcsLOGICAL         IsPropertySet    (const char*      propertyId);

    // Is a name a property?
    virtual mcsLOGICAL         IsProperty       (const char*      propertyId);

    // Return the star RA and DEC coordinates (in arcsecond)
    virtual mcsCOMPL_STAT GetRa (mcsDOUBLE &ra);
    virtual mcsCOMPL_STAT GetDec(mcsDOUBLE &dec);

    // Return the star ID
    virtual mcsCOMPL_STAT GetId (char* starId,
                                 const mcsUINT32 maxLength);

    // Return whether the star is the same as another given one
    virtual mcsLOGICAL IsSame(vobsSTAR                    &star,
                              vobsSTAR_COMP_CRITERIA_LIST *criteriaList = NULL);

    // Update the star properties with the given star ones
    virtual mcsCOMPL_STAT Update(vobsSTAR &star, mcsLOGICAL overwrite=mcsFALSE);

    // Return the number of star properties
    virtual mcsINT32 NbProperties(void);
    
    // Print out all star properties
    virtual void Display(mcsLOGICAL showPropId=mcsFALSE);
    
protected:
    // Add a property. Should be only called by constructors.
    mcsCOMPL_STAT AddProperty(const char*         id,
                               const char*        name,
                               const vobsPROPERTY_TYPE  type,
                               const char*        unit        = NULL,
                               const char*        format      = NULL,
                               const char*        link        = NULL,
                               const char*        description = NULL);

    PropertyMap                                _propertyList;
    PropertyMap::iterator                      _propertyListIterator;
    
    std::map<const int, const char*>           _propertyOrder;
    std::map<const int, const char*>::iterator _propertyOrderIterator;

private:
    mcsDOUBLE                 _ra;  // parsed RA
    mcsDOUBLE                 _dec; // parsed DEC
    
    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);
};


#endif /*!vobsSTAR_H*/
/*___oOo___*/

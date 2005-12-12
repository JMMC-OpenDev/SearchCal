#ifndef vobsSTAR_H
#define vobsSTAR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.h,v 1.44 2005-12-12 14:05:55 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.43  2005/12/07 16:49:18  lafrasse
* Added support for 'description' attribute in VOTable column descriptors FIELD.
*
* Revision 1.42  2005/12/05 15:58:39  scetre
* *** empty log message ***
*
* Revision 1.41  2005/12/02 17:43:30  lafrasse
* Added property unit handling
*
* Revision 1.40  2005/12/02 12:02:23  scetre
* Added J H K cous
*
* Revision 1.39  2005/11/24 08:13:50  scetre
* Changed mother class of filter from vobsFILTER to vobsBASE_FILTER
*
* Revision 1.38  2005/11/23 08:32:48  scetre
* Added properties for faint K
*
* Revision 1.37  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.36  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.35  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.34  2005/09/06 12:37:41  scetre
* Added INST_WAVELENGTH_VALUE as UCD for the wavelength in order to get magnitude according to the flux in II/225 catalog
*
* Revision 1.33  2005/08/03 14:00:57  scetre
* Changed ucd name of vobsSTAR_OBS_METHOD and vobsSTAR_INST_WAVELENGTH_VALUE
*
* Revision 1.32  2005/03/06 10:44:03  gzins
* Removed GetSpectralClass and GetLuminosityClass; not applicable due to the complexity of the spectral type format
*
* Revision 1.31  2005/02/23 07:50:42  gzins
* Added vobsSTAR_DIAM12 and vobsSTAR_DIAM12_ERROR definitions
*
* Revision 1.30  2005/02/13 15:25:59  gzins
* Added showPropId parameter to Display() method
*
* Revision 1.29  2005/02/11 10:44:00  gzins
* Added UD, LD and UDDK diameter star properties
*
* Revision 1.28  2005/02/10 16:42:38  gluck
* Correct wrong type for confidenceIndex: mcsINT32 -> vobsCONFIDENCE_INDEX
*
* Revision 1.27  2005/02/10 08:07:38  scetre
* changed parser and hd, hip, dm number id in order to get all of them even if they have the same UCD
*
* Revision 1.26  2005/02/08 20:36:14  gzins
* Changed some parameters from char* to const char*
*
* Revision 1.25  2005/02/08 07:16:56  gzins
* Changed value parameter type: char* to const char*
*
* Revision 1.24  2005/02/07 09:10:04  gzins
* Fixed wrong type for isComputed parameter; mcsFLOAT instead of mcsLOGICAL
*
* Revision 1.23  2005/02/04 14:31:30  scetre
* add getLuminosityClass method
*
* Revision 1.22  2005/02/04 09:59:05  gzins
* Added star properties for MIDI catalog
*
* Revision 1.21  2005/01/26 14:11:42  scetre
* change assignement operator and pass list as private member of the class vobsSTAR
*
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
#define vobsSTAR_ID_HD                          "ID_HD"
#define vobsSTAR_ID_HIP                         "ID_HIP"
#define vobsSTAR_ID_DM                          "ID_DM"
#define vobsSTAR_ID_TYC1                        "ID_TYC1"
#define vobsSTAR_ID_CATALOG                     "ID_CATALOG"
#define vobsSTAR_ID_2MASS                       "ID_2MASS"
#define vobsSTAR_ID_DENIS                       "ID_DENIS"
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
#define vobsSTAR_CODE_MULT_FLAG                 "CODE_MULT_FLAG"
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
#define vobsSTAR_PHOT_FLUX_IR_12                "PHOT_FLUX_IR_12"
#define vobsSTAR_PHOT_FLUX_IR_12_ERROR          "PHOT_FLUX_IR_12_ERROR"
#define vobsSTAR_REF_STAR                       "REF_STAR"
#define vobsSTAR_PHYS_TEMP_EFFEC                "PHYS_TEMP_EFFEC"
#define vobsSTAR_PHYS_TEMP_EFFEC_ERROR          "PHYS_TEMP_EFFEC_ERROR"
#define vobsSTAR_PHOT_EXTINCTION_TOTAL          "PHOT_EXTINCTION_TOTAL"
#define vobsSTAR_CHI2_QUALITY                   "CHI2_QUALITY"
#define vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC         "SP_TYP_PHYS_TEMP_EFFEC"


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
    virtual mcsCOMPL_STAT SetPropertyValue
               (const char *id, const char *value,
                const char *origin,
                vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH, 
                mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetPropertyValue
                (const char *propertyId, 
                 mcsFLOAT value,
                 const char *origin,
                 vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH, 
                 mcsLOGICAL overwrite=mcsFALSE);

    // Methods to retreive the star property
    virtual vobsSTAR_PROPERTY *GetProperty(char *id) ;
    virtual vobsSTAR_PROPERTY *GetNextProperty(mcsLOGICAL init = mcsFALSE);
    virtual const char *GetPropertyValue(char *id);
    virtual mcsCOMPL_STAT GetPropertyValue(char *id, mcsFLOAT *value);
    virtual vobsPROPERTY_TYPE GetPropertyType(char *id) ;
    
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
    virtual mcsINT32 NbProperties(void);
    
    // Method to print out all star properties
    virtual void Display(mcsLOGICAL showPropId=mcsFALSE);
    
protected:
    // Method to add a property. Should be only called by constructors.
    mcsCOMPL_STAT AddProperty(char *id, char *name,
                              vobsPROPERTY_TYPE type, char *unit,
                              char *format=NULL, char *description=NULL);
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

#ifndef vobsSTAR_H
#define vobsSTAR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.h,v 1.1 2004-07-26 07:41:42 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
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
 * Constants definition
 */

#define vobsNB_STAR_PROPERTIES 29       /**< Number of properties */
#define vobsSTAR_PROP_NOT_SET  "99.99"  /**< Default value of the empty
                                          properties */


/*
 * Type definition
 */

typedef mcsSTRING32 vobsSTAR_PROPERTY;


/*
 * Enumeration type definition
 */

/**
 * vobsUCD_ID is an enumeration which allow correspondance between an id and
 * an UCD.
 */
typedef enum
{
    UNKNOWN_ID = -1,            /**< id if no UCD */
    DATA_LINK_ID,               /**< hd id */
    ID_MAIN_ID,                 /**< hip id */
    POS_EQ_RA_MAIN_ID,          /**< Ra id */
    POS_EQ_DEC_MAIN_ID,         /**< dec id */
    POS_EQ_PMDEC_ID,            /**< pmdec id */
    POS_EQ_PMRA_ID,             /**< pmra id */
    POS_PARLX_TRIG_ID,          /**< parallax id */
    SPECT_TYPE_MK_ID,           /**< sptype id */
    CODE_VARIAB_ID,             /**< vflag id */
    CODE_MULT_FLAG_ID,          /**< mflag id */
    POS_GAL_LAT_ID,             /**< glat id */
    POS_GAL_LON_ID,             /**< glon id */
    VELOC_HC_ID,                /**< veloc id */
    EXTENSION_DIAM_ID,          /**< diam id */
    OBS_METHOD_ID,              /**< meth id */
    INST_WAVELENGTH_VALUE_ID,   /**< wavelength id */
    PHOT_JHN_U_ID,              /**< U id */
    PHOT_JHN_B_ID,              /**< B id */
    PHOT_JHN_V_ID,              /**< V id */
    PHOT_JHN_R_ID,              /**< R id */
    PHOT_JHN_I_ID,              /**< I id */
    PHOT_JHN_J_ID,              /**< J id */
    PHOT_JHN_H_ID,              /**< H id */
    PHOT_JHN_K_ID,              /**< K id */
    PHOT_JHN_L_ID,              /**< L id */
    PHOT_JHN_M_ID,              /**< M id */
    PHOT_IR_N_10_4_ID,          /**< N id */
    VELOC_ROTAT_ID,             /**< veloc rotat id */
    PHOT_COLOR_EXCESS_ID        /**< color id */
} vobsUCD_ID;

/*
 * Class declaration
 */

/**
 * vobsSTAR is a class which caracterise a star.
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
    vobsSTAR(const vobsSTAR &star);

    // Destructor
    virtual ~vobsSTAR();

    // Method to set the star properties
    virtual mcsCOMPL_STAT SetProperty(char *ucd, char *value);
    virtual mcsCOMPL_STAT SetProperty(vobsUCD_ID ucdId, char *value);
    
    // Methods to retreive the star properties
    virtual mcsCOMPL_STAT GetProperty(char *ucd, char *value) const;
    virtual mcsCOMPL_STAT GetProperty(vobsUCD_ID ucdId, char *value) const;
    virtual mcsCOMPL_STAT GetProperty(char *ucd, int *value) const;
    virtual mcsCOMPL_STAT GetProperty(vobsUCD_ID ucdId, int *value) const;
    virtual mcsCOMPL_STAT GetProperty(char *ucd, float *value) const;
    virtual mcsCOMPL_STAT GetProperty(vobsUCD_ID ucdId, float *value) const;
    
    // Method to get RA and DEC (in arcsecond) 
    virtual mcsCOMPL_STAT GetRa(float &ra);
    virtual mcsCOMPL_STAT GetDec(float &dec);
    
    // Method to know if another star is the same than this one
    virtual mcsLOGICAL IsSame (vobsSTAR &star);

    // Method to update star properties from the given star
    virtual mcsLOGICAL Update (vobsSTAR &star);

    // Method to print out all star properties
    virtual void Display();
    
protected:
    // Method to obtain id from the UCD
    virtual vobsUCD_ID Ucd2Id(char *ucd) const;

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsSTAR& operator=(const vobsSTAR&);

    /** Table containing the star properties */
    vobsSTAR_PROPERTY _properties[vobsNB_STAR_PROPERTIES];
};

#endif /*!vobsSTAR_H*/
/*___oOo___*/

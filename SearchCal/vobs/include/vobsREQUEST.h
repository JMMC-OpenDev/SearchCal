#ifndef vobsREQUEST_H
#define vobsREQUEST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsREQUEST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Enum definition
 */

/**
 * Define all the handled search area geometries
 */
enum vobsSEARCH_AREA_GEOM
{
    vobsUNKNOWN, /**< unknown search area geometry */
    vobsBOX, /**< rectangular search area geometry*/
    vobsCIRCLE /**< circular search area geometry*/
} ;


/*
 * Class declaration
 */

/**
 * Class containing user requirements for catalog interrogation.
 *
 * Class used to store user requirements needed to query a catalog. User
 * requirements can be considered as constraints in the catalog interrogation
 * context. Indeed, star search in catalogs will be based on these requirements
 * (constraints) to select stars which satisfy them. These constraints may be
 * star position (ra, dec, ...), star magnitude, ...
 */
class vobsREQUEST
{
public:
    // Constructor
    vobsREQUEST();

    // Destructor
    virtual ~vobsREQUEST();

    // Copy
    virtual mcsCOMPL_STAT Copy(const vobsREQUEST& request);

    // Set and get science object name
    virtual mcsCOMPL_STAT SetObjectName(const char* objectName);
    virtual const char* GetObjectName(void) const;

    // Set and get science object right ascension
    virtual mcsCOMPL_STAT SetObjectRa(const char* objectRa);
    virtual const char* GetObjectRa(void) const;
    virtual mcsDOUBLE GetObjectRaInDeg(void) const;

    // Set and get science object declination
    virtual mcsCOMPL_STAT SetObjectDec(const char* objectDec);
    virtual const char* GetObjectDec(void) const;
    virtual mcsDOUBLE GetObjectDecInDeg(void) const;

    // Indicates if the science object has coordinates
    mcsLOGICAL hasObjectRaDec() const;
    
    // Set and get science object proper motion right ascension
    virtual mcsCOMPL_STAT SetPmRa(const mcsDOUBLE pmRa);
    virtual mcsDOUBLE GetPmRa(void) const;

    // Set and get science object proper motion declination
    virtual mcsCOMPL_STAT SetPmDec(const mcsDOUBLE pmDec);
    virtual mcsDOUBLE GetPmDec(void) const;

    // Set and get science object magnitude
    virtual mcsCOMPL_STAT SetObjectMag(const mcsDOUBLE objectMag);
    virtual mcsDOUBLE GetObjectMag(void) const;

    // Set and get search band
    virtual mcsCOMPL_STAT SetSearchBand(const char* searchBand);
    virtual const char* GetSearchBand(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  minimum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMinMagRange(const mcsDOUBLE minMagRange);
    virtual mcsDOUBLE GetMinMagRange(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  maximum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMaxMagRange(const mcsDOUBLE maxMagRange);
    virtual mcsDOUBLE GetMaxMagRange(void) const;

    // Set and get rectangular search area size
    virtual mcsCOMPL_STAT SetSearchArea(const mcsDOUBLE deltaRa,
                                        const mcsDOUBLE deltaDec);
    virtual mcsCOMPL_STAT GetSearchArea(mcsDOUBLE &deltaRa,
                                        mcsDOUBLE &deltaDec) const;

    // Set and get circular search area size
    virtual mcsCOMPL_STAT SetSearchArea(const mcsDOUBLE radius);
    virtual mcsCOMPL_STAT GetSearchArea(mcsDOUBLE &radius) const;

    // Get the search area geometry, according to which methods were used to set
    // the search area size
    virtual vobsSEARCH_AREA_GEOM GetSearchAreaGeometry(void) const;

    // Display request content (constraints)
    virtual mcsCOMPL_STAT Display(void) const;

    /**
     * Set cone search radius (arcsec) to search arround given star coordinates (secondary requests).
     *
     * @param radius cone search radius (arcsec) to search arround given star coordinates (secondary requests)
     */
    inline void SetConeSearchRadius(const mcsDOUBLE radius) __attribute__((always_inline))
    {
        _coneSearchRadius = radius;
    }

    /**
     * Get cone search radius (arcsec) to search arround given star coordinates (secondary requests).
     *
     * @return cone search radius (arcsec) to search arround given star coordinates (secondary requests)
     */
    inline mcsDOUBLE GetConeSearchRadius(void) const __attribute__((always_inline))
    {
        return _coneSearchRadius;
    }

protected:

private:
    // Declaration of assignment operator as private methods, in order to hide
    // them from the users.
    vobsREQUEST(const vobsREQUEST& request);
    vobsREQUEST& operator=(const vobsREQUEST&) ;

    /** Science object name */
    mcsSTRING64 _objectName;

    /** Science object right ascension */
    mcsSTRING32 _objectRa;
    mcsDOUBLE _objectRaInDeg;

    /** Science object declination */
    mcsSTRING32 _objectDec;
    mcsDOUBLE _objectDecInDeg;

    /** proper motion right ascension (mas/yr) */
    mcsDOUBLE _pmRa;

    /** proper motion right ascension (mas/yr) */
    mcsDOUBLE _pmDec;

    /** Science object magnitude */
    mcsDOUBLE _objectMag;

    /**
     * Band on which search should be realised and which correspond to a
     * specific catalog
     */
    mcsSTRING4 _searchBand;

    /**
     * Maximum magnitude difference between the selected object minimum
     * magnitude and the science object magnitude
     */
    mcsDOUBLE _minMagRange;

    /**
     * Maximum magnitude difference between the selected object maximum
     * magnitude and the science object magnitude
     */
    mcsDOUBLE _maxMagRange;

    /**
     * Search area geometry, implicitly defined according to which methods were
     * used to set the search area size.
     */
    vobsSEARCH_AREA_GEOM _searchAreaGeometry;

    /** Rectangular area ra range (arcmin) in which catalog stars will be selected */
    mcsDOUBLE _deltaRa;

    /** Rectangular area dec range (arcmin) in which catalog stars will be selected */
    mcsDOUBLE _deltaDec;

    /** Circular area radius (arcmin) in which catalog stars will be selected */
    mcsDOUBLE _radius;

    /** Cone search radius (arcsec) to search arround given star coordinates (secondary requests) */
    mcsDOUBLE _coneSearchRadius;

} ;


#endif /*!vobsREQUEST_H*/

/*___oOo___*/

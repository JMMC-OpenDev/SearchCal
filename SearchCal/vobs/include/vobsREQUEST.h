#ifndef vobsREQUEST_H
#define vobsREQUEST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.h,v 1.25 2011-03-03 13:09:42 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.24  2005/11/30 15:26:50  gzins
* Added _objectDecInDeg member
*
* Revision 1.23  2005/11/23 17:30:21  lafrasse
* Added circular search box geometry support and normalized area size methods
*
* Revision 1.22  2005/11/23 15:49:45  lafrasse
* Removed _maxNbOfSelectedObjects member and associated code
*
* Revision 1.21  2005/11/23 15:41:15  lafrasse
* Removed GetMaxNbOfSelectedObjects() and SetMaxNbOfSelectedObjects() methods
*
* Revision 1.20  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.19  2005/02/14 15:22:29  scetre
* changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
*
* Revision 1.18  2005/02/14 15:09:12  scetre
* changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
*
* Revision 1.17  2005/02/13 15:24:50  gzins
* Change Min/MaxDeltaMag to Min/MaxRangeMag
*
* Revision 1.16  2005/02/11 14:44:11  gluck
* Changed Set/Get max number of selected object type parameters: mcsUINT32 -> mcsINT32
*
* Revision 1.15  2005/02/11 14:08:08  gluck
* Changed max number of selected object type: mcsUINT32 -> mcsINT32
*
* Revision 1.14  2005/02/07 19:35:05  gzins
* Changed RA, DEC and band type as well as prototypes of the related methods
*
* Revision 1.13  2005/02/07 17:28:11  gluck
* New vobsREQUEST class implementation
*
* Revision 1.12  2005/02/04 08:06:28  scetre
* changed mcsSTRING32 to mcsSTRING256 for constraints
*
* Revision 1.11  2005/01/26 08:10:32  scetre
* change history
*
* scetre    26-Jul-2004  Created
*
*
*******************************************************************************/

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
   vobsUNKNOWN,  /**< unknown search area geometry */
   vobsBOX,      /**< rectangular search area geometry*/
   vobsCIRCLE    /**< circular search area geometry*/
};


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
    virtual mcsCOMPL_STAT Copy(vobsREQUEST& request);

    // Set and get science object name
    virtual mcsCOMPL_STAT SetObjectName(const char *objectName);
    virtual const char*   GetObjectName(void) const;

    // Set and get science object right ascension */
    virtual mcsCOMPL_STAT SetObjectRa(const char *objectRa);
    virtual const char*   GetObjectRa(void) const;

    // Set and get science object declinaison
    virtual mcsCOMPL_STAT SetObjectDec(const char *objectDec);
    virtual const char*   GetObjectDec(void) const;

    // Set and get science object magnitude
    virtual mcsCOMPL_STAT SetObjectMag(const mcsDOUBLE objectMag);
    virtual mcsDOUBLE      GetObjectMag(void) const;

    // Set and get search band
    virtual mcsCOMPL_STAT SetSearchBand(const char *searchBand);
    virtual const char*   GetSearchBand(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  minimum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMinMagRange(const mcsDOUBLE minMagRange);
    virtual mcsDOUBLE      GetMinMagRange(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  maximum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMaxMagRange(const mcsDOUBLE maxMagRange);
    virtual mcsDOUBLE      GetMaxMagRange(void) const;

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

    // Display request containt (constraints)
    virtual mcsCOMPL_STAT Display(void);

protected:

private:
    // Declaration of assignment operator as private methods, in order to hide
    // them from the users.
    vobsREQUEST(const vobsREQUEST& request);
    vobsREQUEST& operator=(const vobsREQUEST&);

    /** Science object name */
    string _objectName;

    /** Science object right ascension */
    string   _objectRa;

    /** Science object declinaison */
    string   _objectDec;
    mcsDOUBLE _objectDecInDeg;

    /** Science object magnitude */
    mcsDOUBLE _objectMag;

    /**
     * Band on which search should be realised and which correspond to a
     * specific catalog
     */
    string _searchBand;

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

    /** Rectangular area ra range in which catalog stars will be selected */
    mcsDOUBLE _deltaRa;

    /** Rectangular area dec range in which catalog stars will be selected */
    mcsDOUBLE _deltaDec;

    /** Circular area radius in which catalog stars will be selected */
    mcsDOUBLE _radius;

};


#endif /*!vobsREQUEST_H*/

/*___oOo___*/

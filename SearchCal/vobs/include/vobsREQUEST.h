#ifndef vobsREQUEST_H
#define vobsREQUEST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.h,v 1.21 2005-11-23 15:41:15 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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
 * \file
 * Declaration of vobsREQUEST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


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
    virtual const char * GetObjectName(void) const;
    
    // Set and get science object right ascension */
    virtual mcsCOMPL_STAT SetObjectRa(const char *objectRa);
    virtual const char *GetObjectRa(void) const;
    
    // Set and get science object declinaison
    virtual mcsCOMPL_STAT SetObjectDec(const char *objectDec);
    virtual const char *GetObjectDec(void) const;

    // Set and get science object magnitude
    virtual mcsCOMPL_STAT SetObjectMag(const mcsFLOAT objectMag);
    virtual mcsFLOAT GetObjectMag(void) const;

    // Set and get search band
    virtual mcsCOMPL_STAT SetSearchBand(const char *searchBand);
    virtual const char *GetSearchBand(void) const;

    // Set and get object ra difference in which catalog stars will be 
    // selected
    virtual mcsCOMPL_STAT SetDeltaRa(const mcsFLOAT deltaRa);
    virtual mcsFLOAT GetDeltaRa(void) const;

    // Set and get object dec difference in which catalog stars will be 
    // selected
    virtual mcsCOMPL_STAT SetDeltaDec(const mcsFLOAT deltaDec);
    virtual mcsFLOAT GetDeltaDec(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  minimum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMinMagRange(const mcsFLOAT minMagRange);
    virtual mcsFLOAT GetMinMagRange(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  maximum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMaxMagRange(const mcsFLOAT maxMagRange);
    virtual mcsFLOAT GetMaxMagRange(void) const;
    
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
    string _objectRa;
    
    /** Science object declinaison */
    string _objectDec;

    /** Science object magnitude */
    mcsFLOAT _objectMag;

    /**
     * Band on which search should be realised and which correspond to a
     * specific catalog
     */
    string _searchBand;

    /** Object ra range in which catalog stars will be selected */
    mcsFLOAT _deltaRa;

    /** Object dec range in which catalog stars will be selected */
    mcsFLOAT _deltaDec;

    /**
     * Maximum magnitude difference between the selected object minimum
     * magnitude and the science object magnitude
     */
    mcsFLOAT _minMagRange;
    
    /**
     * Maximum magnitude difference between the selected object maximum
     * magnitude and the science object magnitude
     */
    mcsFLOAT _maxMagRange;

    /** Maximum number of selected objects */
    mcsINT32 _maxNbOfSelectedObjects;
};
#endif /*!vobsREQUEST_H*/


/*___oOo___*/

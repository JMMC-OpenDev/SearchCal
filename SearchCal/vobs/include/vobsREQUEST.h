#ifndef vobsREQUEST_H
#define vobsREQUEST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.h,v 1.13 2005-02-07 17:28:11 gluck Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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
    
    // Set and get science object name
    virtual mcsCOMPL_STAT SetObjectName(const char *objectName);
    virtual const char * GetObjectName(void) const;
    
    // Set and get science object right ascension */
    virtual mcsCOMPL_STAT SetObjectRa(const mcsFLOAT objectRa);
    virtual mcsFLOAT GetObjectRa(void) const;
    
    // Set and get science object declinaison
    virtual mcsCOMPL_STAT SetObjectDec(const mcsFLOAT objectDec);
    virtual mcsFLOAT GetObjectDec(void) const;

    // Set and get science object magnitude
    virtual mcsCOMPL_STAT SetObjectMag(const mcsFLOAT objectMag);
    virtual mcsFLOAT GetObjectMag(void) const;

    // Set and get search band
    virtual mcsCOMPL_STAT SetSearchBand(const char searchBand);
    virtual char GetSearchBand(void) const;

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
    virtual mcsCOMPL_STAT SetMinDeltaMag(const mcsFLOAT minDeltaMag);
    virtual mcsFLOAT GetMinDeltaMag(void) const;

    //  Set and get maximum magnitude difference between the selected object
    //  maximum magnitude and the science object magnitude
    virtual mcsCOMPL_STAT SetMaxDeltaMag(const mcsFLOAT maxDeltaMag);
    virtual mcsFLOAT GetMaxDeltaMag(void) const;
    
    // Set and get maximum number of selected objects
    virtual mcsCOMPL_STAT SetMaxNbOfSelectedObjects(const mcsUINT32 
                                           maxNbOfSelectedObjects);
    virtual mcsUINT32 GetMaxNbOfSelectedObjects(void) const;
    
    // Display request containt (constraints)
    virtual mcsCOMPL_STAT Display(void);

protected:

private:
    // Declaration of assignment operator as private methods, in order to hide
    // them from the users.
    vobsREQUEST(const vobsREQUEST&);
    vobsREQUEST& operator=(const vobsREQUEST&);

    /** Science object name */
    string _objectName;
    
    /** Science object right ascension */
    mcsFLOAT _objectRa;
    
    /** Science object declinaison */
    mcsFLOAT _objectDec;

    /** Science object magnitude */
    mcsFLOAT _objectMag;

    /**
     * Band on which search should be realised and which correspond to a
     * specific catalog
     */
    char _searchBand;

    /** Object ra range in which catalog stars will be selected */
    mcsFLOAT _deltaRa;

    /** Object dec range in which catalog stars will be selected */
    mcsFLOAT _deltaDec;

    /**
     * Maximum magnitude difference between the selected object minimum
     * magnitude and the science object magnitude
     */
    mcsFLOAT _minDeltaMag;
    
    /**
     * Maximum magnitude difference between the selected object maximum
     * magnitude and the science object magnitude
     */
    mcsFLOAT _maxDeltaMag;

    /** Maximum number of selected objects */
    mcsUINT32 _maxNbOfSelectedObjects;
};
#endif /*!vobsREQUEST_H*/


/*___oOo___*/

#ifndef vobsREQUEST_H
#define vobsREQUEST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.h,v 1.1 2004-07-13 13:41:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include<stdio.h>
#include<iostream>
#include<string.h>
#include <vector>
/*
 * constants
 */
/*none*/

class vobsREQUEST
{
public:
    vobsREQUEST();
    vobsREQUEST(char *wvl ,char *sMg,char *mRg,char *sRa,char *sDEC,char *vis,char *rAerr, char*b);
    virtual ~vobsREQUEST();

    char *GetStarName();
    char *GetWaveLength();
    char *GetSoMagnitude();
    char *GetMagnitudeRange();
    char *GetSearchRA();
    char *GetSearchDEC();
    char *GetVisibility();
    char *GetReqAbsErr();
    char *GetBand();
    int Build();
   
    void SetStarName(char *x);
    void SetWaveLength(char *x);
    void SetSoMagnitude(char *x);
    void SetMagnitudeRange(char *x);
    void SetSearchRA(char *x);
    void SetSearchDEC(char *x);
    void SetVisibility(char *x);
    void SetReqAbsErr(char *f);
    void SetBand(char *c); 
    
    void Create(char *name, char *wvl ,char *sMg,char *mRg,char *sRa,char *sDEC,char *vis,char *rAerr, char*b);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    //vobsREQUEST& operator=(const vobsREQUEST&);
    //vobsREQUEST (const vobsREQUEST&);
    int build;
    char *starName;
    char *wavelength;
    char *soMagnitude;
    char *magnitudeRange;
    char *searchRA;
    char *searchDEC;
    char *visibility;
    char *reqAbsErr;
    char *band;
        
//    std::vector<char *>listOfCatalog;
};


#endif /*!vobsREQUEST_H*/


/*___oOo___*/

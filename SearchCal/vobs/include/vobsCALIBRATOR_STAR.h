#ifndef vobvobsRATOR_STAR_H
#define vobvobsRATOR_STAR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCALIBRATOR_STAR.h,v 1.1 2004-07-13 13:41:09 scetre Exp $"
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

class vobsCALIBRATOR_STAR
{
 public:
     vobsCALIBRATOR_STAR();// constructor
     virtual ~vobsCALIBRATOR_STAR();// destructor
     
     void Sethip(char *x);
     void Sethd(char *x);
     void Setraj2000(char *x);
     void Setdej2000(char *x);
     void SetpmDE(char *x);
     void SetpmRA(char *x);
     void Setplx(char *x);
     void SetSpType(char *x);
     void SetvFlag(char *x);
     void SetmFlag(char *x);
     void SetgLat(char *x);
     void SetgLon(char *x);
     void SetradVel(char *x);
     void Setld(char *x);
     void Sete_ld(char *x);
     void Setud(char *x);
     void Sete_ud(char *x);
     void Setmeth(char *x);
     void Setlambda(char *x);
     void Setuddk(char *x);
     void Sete_uddk(char *x);
     void SetU(char *x);
     void SetB(char *x);
     void SetV(char *x);
     void SetR(char *x);
     void SetI(char *x);
     void SetJ(char *x);
     void SetH(char *x);
     void SetK(char *x);
     void SetL(char *x);
     void SetM(char *x);
     void SetN(char *x);
     void SetrotVel(char *x);
     void Sete_B_V(char *x);
     
     char * Gethip();
     char * Gethd();
     char * Getraj2000();
     char * Getdej2000();
     char * GetpmDE();
     char * GetpmRA();
     char * Getplx();
     char * GetSpType();
     char * GetvFlag();
     char * GetmFlag();
     char * GetgLat();
     char * GetgLon();
     char * GetradVel();
     char * Getld();
     char * Gete_ld();
     char * Getud();
     char * Gete_ud();
     char * Getmeth();
     char * Getlambda();
     char * Getuddk();
     char * Gete_uddk();
     char * GetU();
     char * GetB();
     char * GetV();
     char * GetR();
     char * GetI();
     char * GetJ();
     char * GetH();
     char * GetK();
     char * GetL();
     char * GetM();
     char * GetN();
     char * GetrotVel();
     char * Gete_B_V();

     void AddElement(char *ucd, char *value);

     void View();
 protected:

 private:
     //list of the data from the CDS

     /*******identificators*********/
     char *hip;        // hip number 
     // DATA_LINK
     char *hd;         // hd number 
     // ID_MAIN
     
     /*****astrometric data*********/
     char *raj2000;    // right ascension J2000 
     // POS_EQ_RA_MAIN
     char *dej2000;    // declinaison J2000 
     // POS_EQ_DEC_MAIN
     char *pmDE;       // proper motion in declinaison
     // POS_EQ_PMDEC
     char *pmRA;       // proper motion in right ascension
     // POS_EQ_PMRA
     char *plx;        // trigonometric parallax
     // POS_PARLX_TRIG
     
     /*****Spectral type************/
     char *spType;     // spectral type and luminosity class if avaible
     // SPECT_TYPE_MK
     
     /**Variability - Multiplicity**/
     char *vFlag;      // variability flag
     // CODE_VARIAB
     char *mFlag;      // multiplicity flag
     // CODE_MULT_FLAG
     
     /*******Coordonate*************/
     char *gLat;       // galactic latitude
     // POS_GAL_LAT
     char *gLon;       // galatic longitude
     // POS_GAL_LON
     
     char *radVel;     // heliocentric radial velocity
     // VELOC_HC
     
     /************CHARM*************/
     char *ld;         // limb darkened disc diameter
     // EXTENSION_DIAM
     char *e_LD;       // error limb dark...
     // ERROR
     char *ud;         // uniform disc diameter
     // EXTENSION_DIAM 
     char *e_UD;       // error uniform disc ...
     // ERROR 
     char *meth;       // method of measurement
     // OBS_METHOD
     char *lambda;     // wavelength
     // INST_WAVELENGTH_VALUE
     
     /************LBSCI*************/
     char *uddk;       // uniform disc diameter in K band
     // EXTENSION_DIAM 
     char *e_uddk;     // error uddk
     // ERROR
     
     /********photometry***********/
     /*Johnson's magnitude in differents bands*/
     char *U; // PHOT_JHN_U
     char *B; // PHOT_JHN_B
     char *V; // PHOT_JHN_V
     char *R; // PHOT_JHN_R
     char *I; // PHOT_JHN_I
     char *J; // PHOT_JHN_J
     char *H; // PHOT_JHN_H
     char *K; // PHOT_JHN_K
     char *L; // PHOT_JHN_L
     char *M; // PHOT_JHN_M
     char *N; // PHOT_JHN_N
     /********Miscellaneous********/
     char *rotVel;     // rotation velocity v sini
     // VELOC_ROTAT
     char *e_B_V;      // colour excess E(B-V)i
     // PHOT_COLOR_EXCESS

};



#endif /*!vobsCALIBRATOR_STAR_H*/


/*___oOo___*/

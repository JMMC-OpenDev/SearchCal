/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCALIBRATOR_STAR.C,v 1.2 2004-07-19 09:25:00 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsCALIBRATOR_STAR.C,v 1.2 2004-07-19 09:25:00 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

#include<stdlib.h>
#include<string.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#define MODULE_ID "vobs"

/*
 * Local Headers 
 */
#include "vobsCALIBRATOR_STAR.h"

/*
 * Local Variables
 */


/*
 * Local Functions
 */
// Class constructor
vobsCALIBRATOR_STAR::vobsCALIBRATOR_STAR()
{
     hip="99.99";        // hip number 
     hd="99.99";         // hd number 
     raj2000="99.99";    // right ascension J2000 
     dej2000="99.99";    // declinaison J2000 
     pmDE="99.99";       // proper motion in declinaison
     pmRA="99.99";       // proper motion in right ascension
     plx="99.99";        // trigonometric parallax
     spType="99.99";     // spectral type and luminosity class if avaible
     vFlag="99.99";      // variability flag
     mFlag="99.99";      // multiplicity flag
     gLat="99.99";       // galactic latitude
     gLon="99.99";       // galatic longitude
     radVel="99.99";     // heliocentric radial velocity
     ld="99.99";         // limb darkened disc diameter
     e_LD="99.99";       // error limb dark...
     ud="99.99";         // uniform disc diameter
     e_UD="99.99";       // error uniform disc ...
     meth="99.99";       // method of measurement
     lambda="99.99";     // wavelength
     uddk="99.99";       // uniform disc diameter in K band
     e_uddk="99.99";     // error uddk
     U="99.99"; // PHOT_JHN_U
     B="99.99"; // PHOT_JHN_B
     V="99.99"; // PHOT_JHN_V
     R="99.99"; // PHOT_JHN_R
     I="99.99"; // PHOT_JHN_I
     J="99.99"; // PHOT_JHN_J
     H="99.99"; // PHOT_JHN_H
     K="99.99"; // PHOT_JHN_K
     L="99.99"; // PHOT_JHN_L
     M="99.99"; // PHOT_JHN_M
     N="99.99"; // PHOT_IR_N:10.4
     rotVel="99.99";     // rotation velocity v sini
     e_B_V="99.99";      // colour excess E(B-V)i


}

// Class destructor 
vobsCALIBRATOR_STAR::~vobsCALIBRATOR_STAR()
{
    /*
     delete[] hip;        // hip number 
     delete[] hd;         // hd number 
     delete[] raj2000;    // right ascension J2000 
     delete[] dej2000;    // declinaison J2000 
     delete[] pmDE;       // proper motion in declinaison
     delete[] pmRA;       // proper motion in right ascension
     delete[] plx;        // trigonometric parallax
     delete[] spType;     // spectral type and luminosity class if avaible
     delete[] vFlag;      // variability flag
     delete[] mFlag;      // multiplicity flag
     delete[] gLat;       // galactic latitude
     delete[] gLon;       // galatic longitude
     delete[] radVel;     // heliocentric radial velocity
     delete[] ld;         // limb darkened disc diameter
     delete[] e_LD;       // error limb dark...
     delete[] ud;         // uniform disc diameter
     delete[] e_UD;       // error uniform disc ...
     delete[] meth;       // method of measurement
     delete[] lambda;     // wavelength
     delete[] uddk;       // uniform disc diameter in K band
     delete[] e_uddk;     // error uddk
     delete[] U; // PHOT_JHN_U
     delete[] B; // PHOT_JHN_B
     delete[] V; // PHOT_JHN_V
     delete[] R; // PHOT_JHN_R
     delete[] I; // PHOT_JHN_I
     delete[] J; // PHOT_JHN_J
     delete[] H; // PHOT_JHN_H
     delete[] K; // PHOT_JHN_K
     delete[] L; // PHOT_JHN_L
     delete[] M; // PHOT_JHN_M
     delete[] N; // PHOT_JHN_N
     delete[] rotVel;     // rotation velocity v sini
     delete[] e_B_V;      // colour excess E(B-V)i

*/

}

//Methods to Set parameters

void vobsCALIBRATOR_STAR::Sethip(char *x)
{
    hip=new char[strlen(x)+1];
    strcpy(hip,x);
}

void vobsCALIBRATOR_STAR::Sethd(char *x)
{
    hd=new char[strlen(x)+1];
    strcpy(hd,x);
}

void vobsCALIBRATOR_STAR::Setraj2000(char *x)
{
    raj2000=new char[strlen(x)+1];
    strcpy(raj2000,x);
}

void vobsCALIBRATOR_STAR::Setdej2000(char *x)
{
    dej2000=new char[strlen(x)+1];
    strcpy(dej2000,x);
}

void vobsCALIBRATOR_STAR::SetpmDE(char *x)
{
    pmDE=new char[strlen(x)+1];
    strcpy(pmDE,x);
}

void vobsCALIBRATOR_STAR::SetpmRA(char *x)
{
    pmRA=new char[strlen(x)+1];
    strcpy(pmRA,x);
}

void vobsCALIBRATOR_STAR::Setplx(char *x)
{
    plx=new char[strlen(x)+1];
    strcpy( plx,x);
}

void vobsCALIBRATOR_STAR::SetSpType(char *x)
{
    spType=new char[strlen(x)+1];
    strcpy(spType,x);
}

void vobsCALIBRATOR_STAR::SetvFlag(char *x)
{
    vFlag=new char[strlen(x)+1];
    strcpy(vFlag,x);
}

void vobsCALIBRATOR_STAR::SetmFlag(char *x)
{
    mFlag=new char[strlen(x)+1];
    strcpy(mFlag,x);
}

void vobsCALIBRATOR_STAR::SetgLat(char *x)
{
    gLat=new char[strlen(x)+1];
    strcpy(gLat,x);
}

void vobsCALIBRATOR_STAR::SetgLon(char *x)
{
    gLon=new char[strlen(x)+1];
    strcpy(gLon,x);
}

void vobsCALIBRATOR_STAR::SetradVel(char *x)
{
    radVel=new char[strlen(x)+1];
    strcpy(radVel,x);
}

void vobsCALIBRATOR_STAR::Setld(char *x)
{
    ld=new char[strlen(x)+1];
    strcpy(ld,x);
}

void vobsCALIBRATOR_STAR::Sete_ld(char *x)
{
    e_LD=new char[strlen(x)+1];
    strcpy(e_LD,x);
}

void vobsCALIBRATOR_STAR::Setud(char *x)
{
    ud=new char[strlen(x)+1];
    strcpy(ud,x);
}

void vobsCALIBRATOR_STAR::Sete_ud(char *x)
{
    e_UD=new char[strlen(x)+1];
    strcpy( e_UD,x);
}

void vobsCALIBRATOR_STAR::Setmeth(char *x)
{
    meth=new char[strlen(x)+1];
    strcpy(meth,x);
}

void vobsCALIBRATOR_STAR::Setlambda(char *x)
{
    lambda=new char[strlen(x)+1];
    strcpy(lambda,x);
}

void vobsCALIBRATOR_STAR::Setuddk(char *x)
{
    uddk=new char[strlen(x)+1];
    strcpy(uddk,x);
}

void vobsCALIBRATOR_STAR::Sete_uddk(char *x)
{
    e_uddk=new char[strlen(x)+1];
    strcpy(e_uddk,x);
}

void vobsCALIBRATOR_STAR::SetU(char *x)
{
    U=new char[strlen(x)+1];
    strcpy(U,x);
}

void vobsCALIBRATOR_STAR::SetB(char *x)
{
    B=new char[strlen(x)+1];
     strcpy(B,x);


}

void vobsCALIBRATOR_STAR::SetV(char *x)
{
    V=new char[strlen(x)+1];
     strcpy(V,x);

}

void vobsCALIBRATOR_STAR::SetR(char *x)
{
    R=new char[strlen(x)+1];
     strcpy(R,x);

}

void vobsCALIBRATOR_STAR::SetI(char *x)
{
    I=new char[strlen(x)+1];
     strcpy(I,x);

}

void vobsCALIBRATOR_STAR::SetJ(char *x)
{
    J=new char[strlen(x)+1];
     strcpy(J,x);

}

void vobsCALIBRATOR_STAR::SetH(char *x)
{
    H=new char[strlen(x)+1];
     strcpy(H,x);

}

void vobsCALIBRATOR_STAR::SetK(char *x)
{
    K=new char[strlen(x)+1];
     strcpy(K,x);

}

void vobsCALIBRATOR_STAR::SetL(char *x)
{
    L=new char[strlen(x)+1];
     strcpy(L,x);

}

void vobsCALIBRATOR_STAR::SetM(char *x)
{
    M=new char[strlen(x)+1];
     strcpy(M,x);

}

void vobsCALIBRATOR_STAR::SetN(char *x)
{
    N=new char[strlen(x)+1];
    strcpy( N,x);

}

void vobsCALIBRATOR_STAR::SetrotVel(char *x)
{
    rotVel=new char[strlen(x)+1];
     strcpy(rotVel,x);

}

void vobsCALIBRATOR_STAR::Sete_B_V(char *x)
{
    e_B_V=new char[strlen(x)+1];
     strcpy(e_B_V,x);

}
/*************************************************************/
char * vobsCALIBRATOR_STAR::Gethip()
{
    return hip;
}

char * vobsCALIBRATOR_STAR::Gethd()
{   
    return hd;   
}

char * vobsCALIBRATOR_STAR::Getraj2000()
{
    return raj2000;   
}

char * vobsCALIBRATOR_STAR::Getdej2000()
{
    return dej2000;   
}

char * vobsCALIBRATOR_STAR::GetpmDE()
{
    return pmDE;   
}

char * vobsCALIBRATOR_STAR::GetpmRA()
{
    return pmRA;   
}

char * vobsCALIBRATOR_STAR::Getplx()
{
    return plx;   
}

char * vobsCALIBRATOR_STAR::GetSpType()
{
    return spType;   
}

char * vobsCALIBRATOR_STAR::GetvFlag()
{
    return vFlag;   
}

char * vobsCALIBRATOR_STAR::GetmFlag()
{
    return mFlag;   
}

char * vobsCALIBRATOR_STAR::GetgLat()
{
    return gLat;   
}

char * vobsCALIBRATOR_STAR::GetgLon()
{
    return gLon;   
}

char * vobsCALIBRATOR_STAR::GetradVel()
{
    return radVel;   
}

char * vobsCALIBRATOR_STAR::Getld()
{
    return ld;   
}

char * vobsCALIBRATOR_STAR::Gete_ld()
{
    return e_LD;   
}

char * vobsCALIBRATOR_STAR::Getud()
{
    return ud;   
}


char * vobsCALIBRATOR_STAR::Gete_ud()
{   
    return e_UD;   
}

char * vobsCALIBRATOR_STAR::Getmeth()
{
    return meth;   
}

char * vobsCALIBRATOR_STAR::Getlambda()
{
    return lambda;   
}

char * vobsCALIBRATOR_STAR::Getuddk()
{
    return uddk;   
}

char * vobsCALIBRATOR_STAR::Gete_uddk()
{
    return e_uddk;   
}

char * vobsCALIBRATOR_STAR::GetU()
{
    return U;   
}

char * vobsCALIBRATOR_STAR::GetB()
{
    return B;   
}

char * vobsCALIBRATOR_STAR::GetV()
{
    return V;   
}

char * vobsCALIBRATOR_STAR::GetR()
{
    return R;   
}

char * vobsCALIBRATOR_STAR::GetI()
{
    return I;   
}

char * vobsCALIBRATOR_STAR::GetJ()
{ 
    return J;   
}

char * vobsCALIBRATOR_STAR::GetH()
{
    return H;   
}

char * vobsCALIBRATOR_STAR::GetK()
{
    return K;   

}

char * vobsCALIBRATOR_STAR::GetL()
{
    return L;    
}

char * vobsCALIBRATOR_STAR::GetM()
{
    return M;   
}

char * vobsCALIBRATOR_STAR::GetN()
{
    return N;   
}

char * vobsCALIBRATOR_STAR::GetrotVel()
{
    return rotVel;   
}

char * vobsCALIBRATOR_STAR::Gete_B_V()
{
    return e_B_V;   
}

/*************************************************************/
// Method to add one parameters in the list
void vobsCALIBRATOR_STAR::AddElement(char *ucd, char *value)
{
    if (strcmp(ucd,"DATA_LINK")==0)
        Sethip(value);
    else if (strcmp(ucd,"ID_MAIN")==0)
        Sethd(value);
    else if ((strcmp(ucd,"POS_EQ_RA_MAIN")==0)&&(strcmp(Getraj2000(),"99.99")==0))
        Setraj2000(value);
    else if ((strcmp(ucd,"POS_EQ_DEC_MAIN")==0)&&(strcmp(Getdej2000(),"99.99")==0))
        Setdej2000(value);
    else if (strcmp(ucd,"POS_EQ_PMDEC")==0)
        SetpmDE(value);
    else if (strcmp(ucd,"POS_EQ_PMRA")==0)
        SetpmRA(value);
    else if (strcmp(ucd,"POS_PARLX_TRIG")==0)
        Setplx(value);
    else if (strcmp(ucd,"SPECT_TYPE_MK")==0)
        SetSpType(value);
    else if (strcmp(ucd,"CODE_VARIAB")==0)
        SetvFlag(value);
    else if (strcmp(ucd,"CODE_MULT_FLAG")==0)
        SetmFlag(value);
    else if (strcmp(ucd,"POS_GAL_LAT")==0)
        SetgLat(value);
    else if (strcmp(ucd,"POS_GAL_LON")==0)
        SetgLon(value);
    else if (strcmp(ucd,"VELOC_HC")==0)
        SetradVel(value);
    else if (strcmp(ucd,"EXTENSION_DIAM")==0)
        Setld(value);
 
    else if (strcmp(ucd,"OBS_METHOD")==0)
        Setmeth(value);
    else if (strcmp(ucd,"INST_WAVELENGTH_VALUE")==0)
        Setlambda(value);

    else if (strcmp(ucd,"PHOT_JHN_U")==0)
        SetU(value);
    else if (strcmp(ucd,"PHOT_JHN_B")==0)
        SetB(value);
    else if (strcmp(ucd,"PHOT_JHN_V")==0)
        SetV(value);
    else if (strcmp(ucd,"PHOT_JHN_R")==0)
        SetR(value);
    else if (strcmp(ucd,"PHOT_JHN_I")==0)
        SetI(value);
    else if (strcmp(ucd,"PHOT_JHN_J")==0)
        SetJ(value);
    else if (strcmp(ucd,"PHOT_JHN_H")==0)
        SetH(value);
    else if (strcmp(ucd,"PHOT_JHN_K")==0)
        SetK(value);
    else if (strcmp(ucd,"PHOT_JHN_L")==0)
        SetL(value);
    else if (strcmp(ucd,"PHOT_JHN_M")==0)
        SetM(value);
    else if (strcmp(ucd,"PHOT_IR_N:10.4")==0)
        SetN(value);
    else if (strcmp(ucd,"VELOC_ROTAT")==0)
        SetrotVel(value);
    else if (strcmp(ucd,"PHOT_COLOR_EXCESS")==0)
        Sete_B_V(value);
}


void vobsCALIBRATOR_STAR::View()
{
    cout << "-----------------------------------------" << endl;
    if (strcmp(Gethip(),"99.99")!=0) cout << "hip ="<< hip <<"."<< endl;
    if (strcmp(Gethd(),"99.99")!=0) cout << "hd ="<< hd <<"."<< endl;
    if (strcmp(Getraj2000(),"99.99")!=0) cout << "raj2000 =" << raj2000 <<"."<<endl;
    if (strcmp(Getdej2000(),"99.99")!=0) cout  << "dej2000 =" << dej2000 <<"."<< endl;
    if (strcmp(GetpmDE(),"99.99")!=0) cout << "pmDE =" << pmDE <<"."<< endl;
    if (strcmp(GetpmRA(),"99.99")!=0) cout << "pmRA =" << pmRA <<"."<<endl;
    if (strcmp(Getplx(),"99.99")!=0) cout << "plx=" << plx <<"."<< endl;
    if (strcmp(GetSpType(),"99.99")!=0) cout << "spType=" << spType <<"."<< endl;
    if (strcmp(GetvFlag(),"99.99")!=0) cout << "vFlag=" << vFlag <<"."<< endl;
    if (strcmp(GetmFlag(),"99.99")!=0) cout << "mFlag=" << mFlag <<"."<< endl;
    if (strcmp(GetgLat(),"99.99")!=0) cout << "gLat=" << gLat <<"."<< endl;
    if (strcmp(GetgLon(),"99.99")!=0) cout << "gLon=" << gLon <<"."<< endl;
    if (strcmp(GetradVel(),"99.99")!=0) cout << "radVel=" << radVel <<"."<<endl; 
    if (strcmp(Getmeth(),"99.99")!=0) cout << "meth=" << meth <<"."<<endl;
    if (strcmp(Getld(),"99.99")!=0) cout << "ld=" << ld << "."<<endl;
    if (strcmp(Getlambda(),"99.99")!=0) cout << "lambda=" << lambda << "."<<endl;
    if (strcmp(Getuddk(),"99.99")!=0) cout << "uddk=" << uddk << "."<<endl;
    if (strcmp(GetU(),"99.99")!=0) cout << "U =" << U <<"."<< endl;
    if (strcmp(GetB(),"99.99")!=0) cout << "B =" << B <<"."<< endl;
    if (strcmp(GetV(),"99.99")!=0) cout << "V =" << V <<"."<< endl;
    if (strcmp(GetR(),"99.99")!=0) cout << "R =" << R <<"."<< endl;
    if (strcmp(GetI(),"99.99")!=0) cout << "I =" << I <<"."<< endl;
    if (strcmp(GetJ(),"99.99")!=0) cout << "J =" << J <<"."<< endl;
    if (strcmp(GetH(),"99.99")!=0) cout << "H =" << H <<"."<< endl;
    if (strcmp(GetK(),"99.99")!=0) cout << "K =" << K <<"."<< endl;
    if (strcmp(GetL(),"99.99")!=0) cout << "L =" << L <<"."<< endl;
    if (strcmp(GetM(),"99.99")!=0) cout << "M =" << M <<"."<< endl;
    if (strcmp(GetN(),"99.99")!=0) cout << "N =" << N <<"."<< endl;
    if (strcmp(GetrotVel(),"99.99")!=0) cout << "rotVel =" << rotVel <<"."<< endl;
    if (strcmp(Gete_B_V(),"99.99")!=0) cout << "e_B_V =" << e_B_V <<"."<< endl;
  
    /**/
}



/*___oOo___*/

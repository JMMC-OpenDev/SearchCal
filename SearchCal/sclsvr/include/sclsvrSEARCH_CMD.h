/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef sclsvrSEARCH_CMD_H
#define sclsvrSEARCH_CMD_H

/**
 * \file
 * sclsvrSEARCH_CMD class declaration.
 * This file has been automatically generated. If this file is missing in your
 * modArea, just type make all to regenerate.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers
 */
#include "cmd.h"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the SEARCH command 
 */

class sclsvrSEARCH_CMD: public cmdCMD
{
public:
    sclsvrSEARCH_CMD(string name, string params);
    virtual ~sclsvrSEARCH_CMD();
    virtual mcsCOMPL_STAT get_objetName(char * *objetName);
    virtual mcsCOMPL_STAT get_mag(mcsDOUBLE  *mag);
    virtual mcsCOMPL_STAT get_maxReturn(mcsINT32  *maxReturn);
    virtual mcsLOGICAL hasDefault_maxReturn();
    virtual mcsCOMPL_STAT getDefault_maxReturn(mcsINT32  *maxReturn);
    virtual mcsCOMPL_STAT get_diffRa(mcsINT32  *diffRa);
    virtual mcsLOGICAL hasDefault_diffRa();
    virtual mcsCOMPL_STAT getDefault_diffRa(mcsINT32  *diffRa);
    virtual mcsCOMPL_STAT get_diffDec(mcsINT32  *diffDec);
    virtual mcsLOGICAL hasDefault_diffDec();
    virtual mcsCOMPL_STAT getDefault_diffDec(mcsINT32  *diffDec);
    virtual mcsCOMPL_STAT get_band(char * *band);
    virtual mcsLOGICAL hasDefault_band();
    virtual mcsCOMPL_STAT getDefault_band(char * *band);
    virtual mcsCOMPL_STAT get_minMagRange(char * *minMagRange);
    virtual mcsCOMPL_STAT get_maxMagRange(char * *maxMagRange);
    virtual mcsCOMPL_STAT get_ra(char * *ra);
    virtual mcsLOGICAL hasDefault_ra();
    virtual mcsCOMPL_STAT getDefault_ra(char * *ra);
    virtual mcsCOMPL_STAT get_dec(char * *dec);
    virtual mcsLOGICAL hasDefault_dec();
    virtual mcsCOMPL_STAT getDefault_dec(char * *dec);
    virtual mcsCOMPL_STAT get_baseMin(mcsDOUBLE  *baseMin);
    virtual mcsLOGICAL hasDefault_baseMin();
    virtual mcsCOMPL_STAT getDefault_baseMin(mcsDOUBLE  *baseMin);
    virtual mcsCOMPL_STAT get_baseMax(mcsDOUBLE  *baseMax);
    virtual mcsLOGICAL hasDefault_baseMax();
    virtual mcsCOMPL_STAT getDefault_baseMax(mcsDOUBLE  *baseMax);
    virtual mcsCOMPL_STAT get_lambda(mcsDOUBLE  *lambda);
    virtual mcsCOMPL_STAT get_vis(mcsDOUBLE  *vis);
    virtual mcsLOGICAL hasDefault_vis();
    virtual mcsCOMPL_STAT getDefault_vis(mcsDOUBLE  *vis);
    virtual mcsCOMPL_STAT get_visErr(mcsDOUBLE  *visErr);
    virtual mcsLOGICAL hasDefault_visErr();
    virtual mcsCOMPL_STAT getDefault_visErr(mcsDOUBLE  *visErr);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     sclsvrSEARCH_CMD(const sclsvrSEARCH_CMD&);
     sclsvrSEARCH_CMD& operator=(const sclsvrSEARCH_CMD&);

};

#endif /*!sclsvrSEARCH_CMD_H*/

/*___oOo___*/

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

class sclsvrSEARCH_CMD: public cmdCOMMAND
{
public:
sclsvrSEARCH_CMD(string name, string params);
    virtual ~sclsvrSEARCH_CMD();
    virtual mcsCOMPL_STAT GetObjetName(char **objetName);
    virtual mcsCOMPL_STAT GetMag(mcsDOUBLE *mag);
    virtual mcsCOMPL_STAT GetMaxReturn(mcsINT32 *maxReturn);
    virtual mcsLOGICAL HasDefaultMaxReturn();
    virtual mcsCOMPL_STAT GetDefaultMaxReturn(mcsINT32 *maxReturn);
    virtual mcsCOMPL_STAT GetDiffRa(mcsINT32 *diffRa);
    virtual mcsLOGICAL HasDefaultDiffRa();
    virtual mcsCOMPL_STAT GetDefaultDiffRa(mcsINT32 *diffRa);
    virtual mcsCOMPL_STAT GetDiffDec(mcsINT32 *diffDec);
    virtual mcsLOGICAL HasDefaultDiffDec();
    virtual mcsCOMPL_STAT GetDefaultDiffDec(mcsINT32 *diffDec);
    virtual mcsCOMPL_STAT GetBand(char **band);
    virtual mcsLOGICAL HasDefaultBand();
    virtual mcsCOMPL_STAT GetDefaultBand(char **band);
    virtual mcsCOMPL_STAT GetMinMagRange(char **minMagRange);
    virtual mcsCOMPL_STAT GetMaxMagRange(char **maxMagRange);
    virtual mcsCOMPL_STAT GetRa(char **ra);
    virtual mcsLOGICAL HasDefaultRa();
    virtual mcsCOMPL_STAT GetDefaultRa(char **ra);
    virtual mcsCOMPL_STAT GetDec(char **dec);
    virtual mcsLOGICAL HasDefaultDec();
    virtual mcsCOMPL_STAT GetDefaultDec(char **dec);
    virtual mcsCOMPL_STAT GetBaseMin(mcsDOUBLE *baseMin);
    virtual mcsLOGICAL HasDefaultBaseMin();
    virtual mcsCOMPL_STAT GetDefaultBaseMin(mcsDOUBLE *baseMin);
    virtual mcsCOMPL_STAT GetBaseMax(mcsDOUBLE *baseMax);
    virtual mcsLOGICAL HasDefaultBaseMax();
    virtual mcsCOMPL_STAT GetDefaultBaseMax(mcsDOUBLE *baseMax);
    virtual mcsCOMPL_STAT GetLambda(mcsDOUBLE *lambda);
    virtual mcsCOMPL_STAT GetVis(mcsDOUBLE *vis);
    virtual mcsLOGICAL HasDefaultVis();
    virtual mcsCOMPL_STAT GetDefaultVis(mcsDOUBLE *vis);
    virtual mcsCOMPL_STAT GetVisErr(mcsDOUBLE *visErr);
    virtual mcsLOGICAL HasDefaultVisErr();
    virtual mcsCOMPL_STAT GetDefaultVisErr(mcsDOUBLE *visErr);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     sclsvrSEARCH_CMD(const sclsvrSEARCH_CMD&);
     sclsvrSEARCH_CMD& operator=(const sclsvrSEARCH_CMD&);

};

#endif /*!sclsvrSEARCH_CMD_H*/

/*___oOo___*/

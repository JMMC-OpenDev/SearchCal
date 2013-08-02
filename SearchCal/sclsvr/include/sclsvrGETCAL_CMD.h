/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef sclsvrGETCAL_CMD_H
#define sclsvrGETCAL_CMD_H

/**
 * \file
 * Generated for sclsvrGETCAL_CMD class declaration.
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
 * Command name definition
 */
#define sclsvrGETCAL_CMD_NAME "GETCAL"

/*
 * Command definition file
 */
#define sclsvrGETCAL_CDF_NAME "sclsvrGETCAL.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the GETCAL command 
 */

class sclsvrGETCAL_CMD: public cmdCOMMAND
{
public:
    sclsvrGETCAL_CMD(string name, string params);
    virtual ~sclsvrGETCAL_CMD();


    virtual mcsCOMPL_STAT GetObjectName(char **_objectName_);
    virtual mcsCOMPL_STAT GetMag(mcsDOUBLE *_mag_);
    virtual mcsCOMPL_STAT GetDiffRa(mcsDOUBLE *_diffRa_);
    virtual mcsLOGICAL HasDefaultDiffRa(void);
    virtual mcsCOMPL_STAT GetDefaultDiffRa(mcsDOUBLE *_diffRa_);
    virtual mcsCOMPL_STAT GetDiffDec(mcsDOUBLE *_diffDec_);
    virtual mcsLOGICAL HasDefaultDiffDec(void);
    virtual mcsCOMPL_STAT GetDefaultDiffDec(mcsDOUBLE *_diffDec_);
    virtual mcsCOMPL_STAT GetRadius(mcsDOUBLE *_radius_);
    virtual mcsLOGICAL IsDefinedRadius(void);
    virtual mcsCOMPL_STAT GetBand(char **_band_);
    virtual mcsLOGICAL HasDefaultBand(void);
    virtual mcsCOMPL_STAT GetDefaultBand(char **_band_);
    virtual mcsCOMPL_STAT GetMinMagRange(mcsDOUBLE *_minMagRange_);
    virtual mcsLOGICAL IsDefinedMinMagRange(void);
    virtual mcsCOMPL_STAT GetMaxMagRange(mcsDOUBLE *_maxMagRange_);
    virtual mcsLOGICAL IsDefinedMaxMagRange(void);
    virtual mcsCOMPL_STAT GetRa(char **_ra_);
    virtual mcsCOMPL_STAT GetDec(char **_dec_);
    virtual mcsCOMPL_STAT GetBaseMax(mcsDOUBLE *_baseMax_);
    virtual mcsLOGICAL HasDefaultBaseMax(void);
    virtual mcsCOMPL_STAT GetDefaultBaseMax(mcsDOUBLE *_baseMax_);
    virtual mcsCOMPL_STAT GetWlen(mcsDOUBLE *_wlen_);
    virtual mcsCOMPL_STAT GetFile(char **_file_);
    virtual mcsLOGICAL IsDefinedFile(void);
    virtual mcsCOMPL_STAT GetBright(mcsLOGICAL *_bright_);
    virtual mcsLOGICAL IsDefinedBright(void);
    virtual mcsLOGICAL HasDefaultBright(void);
    virtual mcsCOMPL_STAT GetDefaultBright(mcsLOGICAL *_bright_);
    virtual mcsCOMPL_STAT GetNoScienceStar(mcsLOGICAL *_noScienceStar_);
    virtual mcsLOGICAL IsDefinedNoScienceStar(void);
    virtual mcsLOGICAL HasDefaultNoScienceStar(void);
    virtual mcsCOMPL_STAT GetDefaultNoScienceStar(mcsLOGICAL *_noScienceStar_);
    virtual mcsCOMPL_STAT GetOutputFormat(mcsDOUBLE *_outputFormat_);
    virtual mcsLOGICAL IsDefinedOutputFormat(void);
    virtual mcsLOGICAL HasDefaultOutputFormat(void);
    virtual mcsCOMPL_STAT GetDefaultOutputFormat(mcsDOUBLE *_outputFormat_);
    virtual mcsCOMPL_STAT GetDiagnose(mcsLOGICAL *_diagnose_);
    virtual mcsLOGICAL IsDefinedDiagnose(void);
    virtual mcsLOGICAL HasDefaultDiagnose(void);
    virtual mcsCOMPL_STAT GetDefaultDiagnose(mcsLOGICAL *_diagnose_);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     sclsvrGETCAL_CMD(const sclsvrGETCAL_CMD&);
     sclsvrGETCAL_CMD& operator=(const sclsvrGETCAL_CMD&);

};

#endif /*!sclsvrGETCAL_CMD_H*/

/*___oOo___*/

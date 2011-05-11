#ifndef fndOBJECT_H
#define fndOBJECT_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of the fndOBJECT class.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Class declaration
 */

/**
 * Foundation class of the inheritance scheme
 * 
 * This is the base class for all new classes. All new classes must be derived
 * from this one. This root class, from which all other classes are
 * hierarchically derived, is typical of many generic foundation libraries in
 * C++ and is the foundation of the inheritance scheme in many other OO
 * languages like SmallTalk or Java.
 *
 * \n
 * \ex
 * \n Declaration of a new class 
 * \code
 * class newCLASS : public fndOBJECT
 * {
 * public:
 * newCLASS();
 * virtual ~newCLASS();
 * ... 
 *
 * protected:
 * ... 
 *
 * private:
 * ... 
 * };
 * \endcode
 *
 */

class fndOBJECT
{
public:
    fndOBJECT();
    virtual ~fndOBJECT();

protected:

private:
};


#endif /*!fndOBJECT_H*/

/*___oOo___*/

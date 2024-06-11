#ifndef evhIOSTREAM_KEY_H
#define evhIOSTREAM_KEY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of the evhIOSTREAM_KEY class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "mcs.h"
#include "evhKEY.h"

/*
 * Class declaration
 */

/**
 * Class to hold I/O stream event id keys.
 * 
 * This class derives from evhKEY, and is used when attaching a callback to a
 * stream I/O type event. It is just characterized by the descriptor of the
 * stream (file descriptor or socket descriptor) to which the callback has to
 * associated; i.e. callback which has to be executed when data is received on
 * this stream.
 */
class evhIOSTREAM_KEY : public evhKEY
{
public:
    evhIOSTREAM_KEY(const int sd = -1);
    evhIOSTREAM_KEY (const evhIOSTREAM_KEY&);
    virtual ~evhIOSTREAM_KEY();

    evhIOSTREAM_KEY& operator=(const evhIOSTREAM_KEY&);

    virtual mcsLOGICAL IsSame(const evhKEY& key);
    virtual mcsLOGICAL Match(const evhKEY& key);

    virtual evhIOSTREAM_KEY &SetSd(const int sd);
    virtual int              GetSd() const;

protected:

private:
    int  _sd;  /** Stream descriptor */
};

#endif /*!evhIOSTREAM_KEY_H*/

/*___oOo___*/

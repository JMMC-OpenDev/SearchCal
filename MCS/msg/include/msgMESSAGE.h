#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.h,v 1.2 2004-11-19 23:55:17 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed all method name first letter to upper case, and
*                        re-commented
*
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local Headers 
 */
#include "msg.h"


/*
 * Class declaration
 */

/**
 * msgMESSAGE is a class that wraps the msgMESSAGE_RAW C structure, with all the
 * needed accessors.
 *
 * It is used to encapsulate all the data to be sent and received with the help
 * of the msgMANAGER_IF object.
 * 
 * \sa msgMESSAGE_RAW and msgHEADER C structures, and msg.h in general
 * \sa msgMANAGER_IF
 */

class msgMESSAGE
{

public:
    // Constructor
    msgMESSAGE                               (const mcsLOGICAL isInternal
                                              = mcsFALSE);

    // Destructor
    virtual ~msgMESSAGE                      ();
    
    // Accessors
    virtual char*            GetSender       ();
    virtual mcsCOMPL_STAT    SetSender       (const char     *buffer);

    virtual char*            GetSenderEnv    ();
    virtual mcsCOMPL_STAT    SetSenderEnv    (const char     *senderEnv);

    virtual char*            GetRecipient    ();
    virtual mcsCOMPL_STAT    SetRecipient    (const char     *recipient);

    virtual char*            GetRecipientEnv ();
    virtual mcsCOMPL_STAT    SetRecipientEnv (const char     *recipientEnv);

    virtual msgTYPE          GetType         ();
    virtual mcsCOMPL_STAT    SetType         (const msgTYPE   type);

    virtual char*            GetIdentifier   ();
    virtual mcsCOMPL_STAT    SetIdentifier   (const char     *identificator);

    virtual char*            GetCommand      ();
    virtual mcsCOMPL_STAT    SetCommand      (const char     *command);

    virtual mcsLOGICAL       IsLastReply     ();
    virtual mcsLOGICAL       IsInternal      ();

    virtual msgHEADER*       GetHeaderPtr    ();

    virtual char*            GetBodyPtr      ();
    virtual mcsINT32         GetBodySize     ();
    virtual mcsCOMPL_STAT    SetBody         (const char     *buffer,
                                              const mcsINT32  bufLen=0);

    virtual msgMESSAGE_RAW*  GetMessageRaw   ();


protected:

    
private:
     // The only member
     msgMESSAGE_RAW _message;

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgMESSAGE(const msgMESSAGE&);
     msgMESSAGE& operator=(const msgMESSAGE&);
};


#endif /*!msgMESSAGE_H*/

/*___oOo___*/

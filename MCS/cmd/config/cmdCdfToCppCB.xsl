<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:output method="text" omit-xml-declaration="yes" indent="no"/>

<!--
    ********************************************************************************
    JMMC project

    "@(#) $Id: cmdCdfToCppCB.xsl,v 1.10 2006-10-05 15:09:01 mella Exp $"

    History
    ~~~~~~~
    $Log: not supported by cvs2svn $
    Revision 1.9  2006/10/05 09:31:26  mella
    Add header and change some doxygen documentation

    Revision 1.8  2006/10/03 13:48:55  mella
    Correct some subtil errors...

    Revision 1.7  2006/09/28 15:23:14  mella
    Add _NAME to one command name define

    Revision 1.6  2006/09/28 14:52:37  mella
    remove automatically generated... message

    Revision 1.5  2005/12/14 14:56:56  mella
    add comments

    Revision 1.4  2005/06/01 08:12:58  mella
    Add more code

    Revision 1.3  2005/06/01 05:56:13  mella
    Change case for filename

    Revision 1.2  2005/06/01 05:38:12  mella
    Add doxygen part for CB method

    Revision 1.1  2005/05/31 14:56:53  mella
    First revision

    ********************************************************************************
-->
    
    <xsl:param name="moduleName"/>
    <xsl:param name="cdfFilename"/>
    
    <!-- ********************************************************* -->
    <!-- This TEMPLATE is the main part                            -->
    <!-- it calls one main template for each cmd nodes             -->
    <!-- INPUT : the module name                                   -->
    <!-- OUTPUT: the generated files  usind <mod><MNEMO>CB.C       -->
    <!-- ********************************************************* -->
    <xsl:template match="/">
 
        <xsl:for-each select="/cmd">
            <xsl:variable name="properModuleName"><xsl:call-template name="convertcase">
                    <xsl:with-param name="toconvert" select="$moduleName"/>
                    <xsl:with-param name="conversion">proper</xsl:with-param>
            </xsl:call-template></xsl:variable>    
            <xsl:variable name="properMnemonic"><xsl:call-template name="convertcase">
                    <xsl:with-param name="toconvert" select="./mnemonic"/>
                    <xsl:with-param name="conversion">proper</xsl:with-param>
            </xsl:call-template></xsl:variable>   
            <xsl:variable name="CBFileName" select="concat($properModuleName,$properMnemonic,'CB')"/>
            <xsl:variable name="CBName" select="concat($properMnemonic,'CB')"/>
            <xsl:document href="{concat($moduleName,$properMnemonic,'CB.cpp')}" method="text">
            <xsl:call-template name="cmdCppCB">
                <xsl:with-param name="CBName" select="$CBName"/>
            </xsl:call-template>
            </xsl:document> 
        </xsl:for-each>
    </xsl:template>
        
    <!-- ********************************************************* -->
    <!-- This TEMPLATE is the main part to generate the c++ header -->
    <!-- file from a given command node.                           -->
    <!-- ********************************************************* -->
    <xsl:template name="cmdCppCB">
         <xsl:variable name="cmdClassName"><xsl:value-of select="$moduleName"/><xsl:call-template name="convertcase">
                    <xsl:with-param name="toconvert" select="./mnemonic"/>
                    <xsl:with-param name="conversion">upper</xsl:with-param>
            </xsl:call-template>_CMD</xsl:variable>    
     
            <xsl:variable name="cmdName"><xsl:value-of select="$moduleName"/><xsl:call-template name="convertcase">
                    <xsl:with-param name="toconvert" select="./mnemonic"/>
                    <xsl:with-param name="conversion">proper</xsl:with-param>
            </xsl:call-template>Cmd</xsl:variable>    
            
            <xsl:variable name="lowerMnemo"><xsl:call-template name="convertcase">
                    <xsl:with-param name="toconvert" select="./mnemonic"/>
                    <xsl:with-param name="conversion">lower</xsl:with-param>
            </xsl:call-template></xsl:variable>/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdCdfToCppCB.xsl,v 1.10 2006-10-05 15:09:01 mella Exp $"
 *
 * History
 * -------
 * $Log&#x24;
 *
 ******************************************************************************/



/**
 * @file
 * Definition of <xsl:value-of select="$CBName"/> callback.
 * Generated from <xsl:value-of select="./mnemonic"/> cdf file.
 */
 
 
/*
 * System Headers
 */
#include &lt;iostream&gt;
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "msg.h"
#include "log.h"

/*
 * Local Headers
 */
#include "<xsl:value-of select="$moduleName"/>.h"
#include "<xsl:value-of select="$moduleName"/>Private.h"
#include "<xsl:value-of select="$cmdClassName"/>.h"
#include "<xsl:value-of select="$moduleName"/>ReplaceByClassName.h"

/*
 * Public methods
 */
 
/**
 * Callback for <xsl:value-of select="./mnemonic"/> command.
 *
 * @param msg originator message.
 *
 * @return an evhCB completion status code (evhCB_SUCCESS or evhCB_FAILURE)
 */
 evhCB_COMPL_STAT <xsl:value-of select="$moduleName"/>ReplaceByClassName::<xsl:value-of select="$CBName"/>(msgMESSAGE &amp;msg, void *)
 {
    logExtDbg("replaceByClassName::<xsl:value-of select="$CBName"/>()");

    /////// Cut &amp; Paste the next lines to attach this callback into your server
    // Attach <xsl:value-of select="./mnemonic"/> command callback
    evhCMD_KEY <xsl:value-of select="$lowerMnemo"/>CmdKey(<xsl:value-of select="$cmdClassName"/>_NAME, <xsl:value-of select="$moduleName"/><xsl:value-of select="./mnemonic"/>_CDF_NAME);
    evhCMD_CALLBACK <xsl:value-of select="$lowerMnemo"/>CB(this, (evhCMD_CB_METHOD)&amp;<xsl:value-of select="$moduleName"/>ReplaceByClassName::<xsl:value-of select="$CBName"/>);
    AddCallback(<xsl:value-of select="$lowerMnemo"/>CmdKey, <xsl:value-of select="$CBName"/>);
    /////// End of Cut &amp; Paste

    
    //  <xsl:value-of select="./mnemonic"/> command
    <xsl:value-of select="$cmdClassName"/><xsl:text> </xsl:text> <xsl:value-of select="$cmdName"/> (msg.GetCommand(), msg.GetBody());

    // Parse command
    if (<xsl:value-of select="$cmdName"/>.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Prepare reply
    logWarning("CODE_MUST_BE_DEFINED");
    msg.SetBody("<xsl:value-of select="$CBName"/> CODE_MUST_BE_DEFINED");
    
    // Send reply
    if (SendReply(msg) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    
    return evhCB_SUCCESS;
 }

/*___oOo___*/
</xsl:template>

  <xsl:variable name="lcletters">abcdefghijklmnopqrstuvwxyz</xsl:variable>

  <xsl:variable name="ucletters">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>

  <xsl:template name='convertcase'>
    <xsl:param name='toconvert' />

    <xsl:param name='conversion' />

    <xsl:choose>
      <xsl:when test='$conversion="lower"'>
        <xsl:value-of
        select="translate($toconvert,$ucletters,$lcletters)" />
      </xsl:when>

      <xsl:when test='$conversion="upper"'>
        <xsl:value-of
        select="translate($toconvert,$lcletters,$ucletters)" />
      </xsl:when>

      <xsl:when test='$conversion="proper"'>
        <xsl:call-template name='convertpropercase'>
          <xsl:with-param name='toconvert'>
            <xsl:value-of
            select="translate($toconvert,$ucletters,$lcletters)" />
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      
      <xsl:when test='$conversion="upfirst"'>
        <xsl:call-template name='convertpropercase'>
          <xsl:with-param name='toconvert'>
            <xsl:value-of
                select="$toconvert" />
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>

      <xsl:otherwise>
        <xsl:value-of select='$toconvert' />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name='convertpropercase'>
    <xsl:param name='toconvert' />

    <xsl:if test="string-length($toconvert) > 0">
      <xsl:variable name='f'
      select='substring($toconvert, 1, 1)' />

      <xsl:variable name='s' select='substring($toconvert, 2)' />

      <xsl:call-template name='convertcase'>
        <xsl:with-param name='toconvert' select='$f' />

        <xsl:with-param name='conversion'>upper</xsl:with-param>
      </xsl:call-template>

      <xsl:choose>
        <xsl:when test="contains($s,' ')">
        <xsl:value-of select='substring-before($s," ")' />

          
        <xsl:call-template name='convertpropercase'>
          <xsl:with-param name='toconvert'
          select='substring-after($s," ")' />
        </xsl:call-template>
        </xsl:when>

        <xsl:otherwise>
          <xsl:value-of select='$s' />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>

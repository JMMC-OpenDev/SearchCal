<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:exslt="http://exslt.org/common"
 xmlns:math="http://exslt.org/math"
 xmlns:date="http://exslt.org/dates-and-times"
 xmlns:func="http://exslt.org/functions"
 xmlns:set="http://exslt.org/sets"
 xmlns:str="http://exslt.org/strings"
 xmlns:dyn="http://exslt.org/dynamic"
 xmlns:saxon="http://icl.com/saxon"
 xmlns:xalanredirect="org.apache.xalan.xslt.extensions.Redirect"
 xmlns:xt="http://www.jclark.com/xt"
 xmlns:libxslt="http://xmlsoft.org/XSLT/namespace"
 xmlns:test="http://xmlsoft.org/XSLT/"
 extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
 exclude-result-prefixes="math str">
<xsl:output omit-xml-declaration="yes" indent="no"/>
<xsl:param name="inputFile">-</xsl:param>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: mkfSTKToYorickWrapperForC.xsl,v 1.1 2005-06-30 15:35:02 mella Exp $"

 History
 ~~~~~~~
 $Log: not supported by cvs2svn $

********************************************************************************
 NAME
 mkfSTKToYorickWrapperForCpp

 DESCRIPTION
 Produce the yorick .i code to be able to use it using Yorick
 The given xml file should be obtained using swig onto the right-wrap.cpp file 
 which does not include any class description but only cdecl.
after special preprocess:
 cpp -DSWIG oidata-wrap.cpp tmpoidata-wrap.cpp
 swig -xml -c++ -module oidata tmpoidata-wrap.cpp
$ xsltproc mkfSTKToYorickWrapperForCpp.xsl ~/sw/oidata/src/tmpoidata-wrap_wrap.xml > ~/sw/ymcs/src/oidata.i

-->

<xsl:include href="mkfSTK_WriteFunctionPrototype.xsl"/>
<xsl:include href="mkfEXSLT_StrReplace.xsl"/>

<xsl:variable name="moduleName" select="top/attributelist/attribute[@name='module']/@value"/>
<xsl:template match="/">
/***** MODULE   '<xsl:value-of select="$moduleName"/>' *****/

<xsl:for-each select=".//cdecl">
    <xsl:if test="not(.//attribute[@name='access'])">
        <xsl:call-template name="wrapForClassMethod"/>
    </xsl:if>
</xsl:for-each>
</xsl:template>

<xsl:template name="wrapForClassMethod">
    <xsl:variable name="methName" select="./attributelist/attribute[@name='name']/@value"/>
    <xsl:variable name="methNameIndex">
        <xsl:call-template name="appendMethodIndex">
            <xsl:with-param name="methodList" select="ancestor::class/cdecl[attributelist/attribute/@value=$methName and attributelist/attribute/@name='name']"/>
            <xsl:with-param name="method" select="."/>
        </xsl:call-template>
    </xsl:variable>
/* Wrapping '<xsl:value-of select="$methName"/>' method */   
extern <xsl:value-of select="$methName"/>;
/* PROTOTYPE
    <xsl:call-template name="WriteYorickType">
        <xsl:with-param name="type" select="./attributelist/attribute[@name='type']/@value"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>( <xsl:if test=".//parmlist"><xsl:call-template name="WriteParametersTypeForYorickPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
*/
/* DOCUMENT  <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>( <xsl:if test=".//parmlist"><xsl:call-template name="WriteParametersTypeForYorickPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
  * C-prototype:
    ------------
 <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>  (<xsl:if test=".//parmlist"> <xsl:call-template name="WriteParametersForPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
*/

</xsl:template>

<xsl:template name="appendMethodIndex">
    <!-- Used to place index in C for C++ polymorphic methods -->
    <xsl:param name="methodlist"/>
    <xsl:param name="method"/>
    <xsl:if test="count($methodList)>1">
        <xsl:for-each select="$methodList">
            <xsl:if test="./@id=$method/@id">_<xsl:value-of select="position()"/></xsl:if>
        </xsl:for-each>
    </xsl:if>
    
</xsl:template>

<!-- Cette fonction ecrit l'entete d'une fonction C -->
<!-- Mais le noeud passer doit etre une cdecl -->
<xsl:template name="WriteParametersTypeForYorickPrototype">
    <xsl:param name="Noeud"/>
    <xsl:if test="contains(.//attribute[@name='decl']/@value,'f(')">

        <!-- Ecriture ddu type des parametres de la fonction -->

        <xsl:for-each select=".//parmlist/parm/attributelist/attribute[@name='type']" >
            <xsl:variable name="type" select="./@value"/>
            <xsl:call-template name="WriteYorickType">
                <xsl:with-param name="type" select="$type"/>
            </xsl:call-template>
            <xsl:if test="not(position()=last())">
                <xsl:text>, </xsl:text>
            </xsl:if>
        </xsl:for-each>
        <xsl:text></xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="WriteYorickType">
    <xsl:param name="type"/>
    <xsl:variable name="typeMod"> 
        <xsl:call-template name="str:replace">
            <xsl:with-param name="string" select="$type" />
            <xsl:with-param name="search" select="'q(const).'" />
            <xsl:with-param name="replace" select="''" />
        </xsl:call-template>
    </xsl:variable>

            <xsl:choose>
                <xsl:when test="starts-with($type,$moduleName)">pointer </xsl:when>
                <xsl:when test="$typeMod='mcsCOMPL_STAT'">int </xsl:when>
                <xsl:when test="$typeMod='mcsLOGICAL'">char </xsl:when>
                <xsl:when test="$typeMod='p.char'">string </xsl:when>
                <xsl:when test="starts-with($typeMod,'p.')">pointer </xsl:when>
                <xsl:when test="$typeMod='int'">int </xsl:when>
                <xsl:when test="$typeMod='short'">short </xsl:when>
                <xsl:when test="$typeMod='void'">void </xsl:when>
                <xsl:when test="$typeMod='double'">double </xsl:when>
                <xsl:when test="$typeMod='float'">float </xsl:when>
                <!-- used for yorick wrappers 
                unsigned char are actually used as char PB todo :solve this
                <xsl:when test="$typeMod='mcsCOMPL_STAT'">int </xsl:when>
                <xsl:when test="$typeMod='mcsINT8'">char </xsl:when>
                <xsl:when test="$typeMod='mcsUINT8'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsINT16'">short </xsl:when>
                <xsl:when test="$typeMod='mcsUINT16'">unsigned short </xsl:when>
                <xsl:when test="$typeMod='mcsINT32'">int </xsl:when>
                <xsl:when test="$typeMod='mcsUINT32'">unsigned int </xsl:when>
                <xsl:when test="$typeMod='mcsDOUBLE'">double </xsl:when>
                <xsl:when test="$typeMod='mcsFLOAT'">float </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES4'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES8'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES12'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES16'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES20'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES32'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES48'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES64'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES80'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES128'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES256'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES512'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES1024'">unsigned char </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING4'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING8'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING12'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING16'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING20'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING32'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING48'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING64'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING80'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING128'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING256'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING512'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING1024'">char * </xsl:when>
                -->
                <xsl:when test="$typeMod='mcsCOMPL_STAT'">int </xsl:when>
                <xsl:when test="$typeMod='mcsINT8'">char </xsl:when>
                <xsl:when test="$typeMod='mcsUINT8'">char </xsl:when>
                <xsl:when test="$typeMod='mcsINT16'">short </xsl:when>
                <xsl:when test="$typeMod='mcsUINT16'">short </xsl:when>
                <xsl:when test="$typeMod='mcsINT32'">int </xsl:when>
                <xsl:when test="$typeMod='mcsUINT32'">int </xsl:when>
                <xsl:when test="$typeMod='mcsDOUBLE'">double </xsl:when>
                <xsl:when test="$typeMod='mcsFLOAT'">float </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES4'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES8'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES12'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES16'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES20'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES32'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES48'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES64'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES80'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES128'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES256'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES512'">char </xsl:when>
                <xsl:when test="$typeMod='mcsBYTES1024'">char </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING4'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING8'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING12'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING16'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING20'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING32'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING48'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING64'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING80'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING128'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING256'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING512'">char * </xsl:when>
                <xsl:when test="$typeMod='mcsSTRING1024'">char * </xsl:when>
                <xsl:otherwise>_TYPE_<xsl:value-of select="$typeMod"/>_NOT_SUPPORTED_in_mkfSTKToYorickWrapperForCpp_xsl_file </xsl:otherwise>
            </xsl:choose>
</xsl:template>

</xsl:stylesheet>

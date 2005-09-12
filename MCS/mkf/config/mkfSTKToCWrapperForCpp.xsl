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

 "@(#) $Id: mkfSTKToCWrapperForCpp.xsl,v 1.5 2005-09-12 06:38:00 mella Exp $"

 History
 ~~~~~~~
 $Log: not supported by cvs2svn $
 Revision 1.4  2005/08/30 07:38:50  mella
 Add swig defines for enum types

 Revision 1.3  2005/06/30 13:28:35  mella
 improve generation and place #define to change type if needed

 Revision 1.2  2005/06/29 13:26:45  mella
 Improve code generation completion

 Revision 1.1  2005/06/29 09:22:58  mella
 First revision


********************************************************************************
 NAME
 mkfSTKToCWrapperForCpp

 DESCRIPTION
 Produce the CPP wrapper code to be able to use it using C

-->

<xsl:include href="mkfSTK_WriteFunctionPrototype.xsl"/>

<xsl:template match="/">
#if SWIG
#define mcsCOMPL_STAT      int    

#define mcsINT8            char               
#define mcsUINT8           unsigned char      
#define mcsINT16           short              
#define mcsUINT16          unsigned short     
#define mcsINT32           int                
#define mcsUINT32          unsigned int       
#define mcsDOUBLE          double             
#define mcsFLOAT           float              
                    
#define mcsBYTES4       unsigned char      
#define mcsBYTES8       unsigned char      
#define mcsBYTES12     unsigned char      
#define mcsBYTES16     unsigned char      
#define mcsBYTES20     unsigned char      
#define mcsBYTES32     unsigned char      
#define mcsBYTES48     unsigned char      
#define mcsBYTES64     unsigned char      
#define mcsBYTES80     unsigned char      
#define mcsBYTES128   unsigned char      
#define mcsBYTES256   unsigned char      
#define mcsBYTES512   unsigned char      
#define mcsBYTES1024 unsigned char      
                    
#define mcsSTRING4      char *              
#define mcsSTRING8      char *              
#define mcsSTRING12    char *              
#define mcsSTRING16    char *              
#define mcsSTRING20    char *              
#define mcsSTRING32    char *              
#define mcsSTRING48    char *              
#define mcsSTRING64    char *              
#define mcsSTRING80    char *              
#define mcsSTRING128  char *              
#define mcsSTRING256  char *              
#define mcsSTRING512  char *              
#define mcsSTRING1024 char *              

/*
 *Enum redifinition
 */
 <xsl:for-each select="//enum">
#define <xsl:value-of select="./attributelist/attribute[@name='name']/@value"/> int
</xsl:for-each>

#else

#include "mcs.h"
#include "<xsl:value-of select="top/attributelist/attribute[@name='module']/@value"/>.h"    
<xsl:for-each select="//class">#include "<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>.h"
</xsl:for-each>
#endif
extern "C"{ 
    <xsl:call-template name="wrapForClasses"/>
}
</xsl:template>

<xsl:template name="wrapForClasses">
  <xsl:for-each select="//class">
    <xsl:call-template name="wrapForClass"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="wrapForClass">
/* 
 * Wrapping '<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>' class 
 */
     
/* constructor '<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>' method */
    <!-- Constructor using default constructor todo check for parameters-->
    <xsl:for-each select=".//constructor">
        <xsl:if test="not(.//attribute[@name='access'])">
void <xsl:value-of select=".//attribute[@name='sym_name']/@value"/>_new(void **obj<xsl:if test=".//parmlist">, <xsl:call-template name="WriteParametersForPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
{
    *obj = new <xsl:value-of select=".//attribute[@name='sym_name']/@value"/>(<xsl:if test=".//parmlist"><xsl:call-template name="WriteParametersForFunctionCall">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>);
}
    </xsl:if>
    <xsl:text> </xsl:text>
</xsl:for-each>

/* destructor '<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>' method */
    <!-- Destructors -->
    <xsl:for-each select=".//destructor">
        <xsl:variable name="className" select="ancestor::class/attributelist/attribute[@name='name']/@value"/>
        <xsl:if test="not(.//attribute[@name='access'])">
        <!-- remove ~ using substring function -->
void <xsl:value-of select="$className"/>_delete(void *obj<xsl:if test=".//parmlist">, <xsl:call-template name="WriteParametersForPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
{
    <xsl:value-of select="$className"/> *o = (<xsl:value-of select="$className"/> *)obj;
    delete o;
}
    </xsl:if>
    <xsl:text> </xsl:text>
    </xsl:for-each>


    <!-- Methods -->
<xsl:for-each select=".//cdecl">
    <xsl:if test="not(.//attribute[@name='access'])">
        <xsl:call-template name="wrapForClassMethod"/>
    </xsl:if>
</xsl:for-each>
</xsl:template>

<xsl:template name="wrapForClassMethod">
    <xsl:variable name="className" select="ancestor::class/attributelist/attribute[@name='name']/@value"/>
    <xsl:variable name="methName" select="./attributelist/attribute[@name='name']/@value"/>
    <xsl:variable name="methNameIndex">
        <xsl:call-template name="appendMethodIndex">
            <xsl:with-param name="methodList" select="ancestor::class/cdecl[attributelist/attribute/@value=$methName and attributelist/attribute/@name='name']"/>
            <xsl:with-param name="method" select="."/>
        </xsl:call-template>
    </xsl:variable>
/* Wrapping '<xsl:value-of select="$methName"/>' method */   
<xsl:call-template name="WriteType">
        <xsl:with-param name="Type" select="./attributelist/attribute[@name='type']/@value"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
<xsl:value-of select="$className"/>
    <xsl:text>_</xsl:text>
    <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>  ( void * obj <xsl:if test=".//parmlist">, <xsl:call-template name="WriteParametersForPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
{
    <xsl:value-of select="$className"/> *o = (<xsl:value-of select="$className"/> *)obj;
    return o-><xsl:value-of select="$methName"/>( <xsl:call-template name="WriteParametersForFunctionCall">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
    </xsl:call-template>);
}
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


</xsl:stylesheet>

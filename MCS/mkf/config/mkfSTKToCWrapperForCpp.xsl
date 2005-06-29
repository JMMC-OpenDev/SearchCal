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

 "@(#) $Id: mkfSTKToCWrapperForCpp.xsl,v 1.1 2005-06-29 09:22:58 mella Exp $"

 History
 ~~~~~~~
 $Log: not supported by cvs2svn $

********************************************************************************
 NAME
 mkfSTKToCWrapperForCpp

 DESCRIPTION
 Produce the CPP wrapper code to be able to use it using C

-->

<xsl:include href="mkfSTK_WriteFunctionPrototype.xsl"/>

<xsl:template match="/">
    <xsl:call-template name="wrapForClasses"/>
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
     
    /* Default constructor '<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>' method */
     <!-- Constructor -->
    <xsl:for-each select=".//constructor">
    <xsl:if test="not(.//attribute[@name='access'])">void <xsl:value-of
            select=".//attribute[@name='sym_name']/@value"/>_new(void **obj, <xsl:call-template name="WritePrototypeParameters">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
    </xsl:call-template>);
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
    /* Wrapping '<xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>' method */
    <xsl:call-template name="WriteType">
        <xsl:with-param name="Type" select="./attributelist/attribute[@name='type']/@value"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="ancestor::class/attributelist/attribute[@name='name']/@value"/>
    <xsl:text>_</xsl:text>
    <xsl:value-of select=".//attribute[@name='sym_name']/@value"/>
    <xsl:text>( void * obj, </xsl:text>
    <xsl:call-template name="WritePrototypeParameters">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
    </xsl:call-template>
    <xsl:text>);</xsl:text>
<xsl:value-of select="'&#10;'"/>
</xsl:template>

</xsl:stylesheet>

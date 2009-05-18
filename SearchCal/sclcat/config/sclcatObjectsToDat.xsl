<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatObjectsToDat.xsl,v 1.1 2009-05-18 15:49:35 mella Exp $"
#
# History
#
# $Log: not supported by cvs2svn $
# Revision 1.12  2009/05/11 10:36:18  mella
# fix exoplanet name for starname column
#
# Revision 1.11  2009/05/06 13:54:34  mella
# add common column description
#
# Revision 1.10  2009/04/29 10:45:16  mella
# add orbit column in html and latex
#
# Revision 1.9  2009/04/27 10:04:35  mella
# add better comment
#
# Revision 1.8  2009/01/22 14:12:10  mella
# improve presetation
#
# Revision 1.7  2007/03/27 14:53:51  scetre
# Updated documentation
#
# Revision 1.6  2007/03/26 11:53:03  scetre
# Moved documentation blok in the first xml tag
#
# Revision 1.5  2007/03/26 11:34:23  scetre
# Added header documentation
#
#
#*******************************************************************************
-->
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
    xmlns:vot="http://www.ivoa.net/xml/VOTable/v1.1"
    extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
    exclude-result-prefixes="math str">
    <xsl:output method="text" />

    <xsl:include href="sclcatObjectsCommon.xsl"/>

    <xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
    <xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>

    <!-- acces filtered calibrators -->
    <xsl:variable name="calibrators" select="document($calibratorsFilename)"/>
    <xsl:variable name="primaStars" select="/"/>

    <xsl:template match="/">
        <xsl:for-each select="$calibrators//star[./calibrator]">
            <xsl:sort select="./calibrator[1]/dist" data-type="number" />
            <xsl:variable name="simbadName" select="./@simbadName"/>
            <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
            <xsl:call-template name="starInTable">
                <xsl:with-param name="object" select="$object"/>
            </xsl:call-template>    
        </xsl:for-each>
        <xsl:for-each select="$calibrators//star[not(./calibrator)]">
            <xsl:variable name="simbadName" select="./@simbadName"/>
            <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
            <xsl:call-template name="starInTable">
                <xsl:with-param name="object" select="$object"/>
            </xsl:call-template>    
        </xsl:for-each>
        <xsl:for-each select="//star">
            <xsl:variable name="simbadName" select="./simbadName"/>
            <xsl:if test="not($calibrators//star[@simbadName=$simbadName])">
                <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
                <xsl:if test="$object">
                    <xsl:call-template name="starInTable">
                        <xsl:with-param name="object" select="$object"/>
                    </xsl:call-template>    
                </xsl:if>
                <xsl:if test="not($object)">
                    <xsl:call-template name="starInTable">
                        <xsl:with-param name="object" select="."/>
                    </xsl:call-template>    
                </xsl:if>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

    <xsl:template name="starInTable">
        <xsl:param name="object"/>
        <xsl:variable name="simbadName" select="$object/name"/>
        <xsl:variable name="primaStar" select="$primaStars//star[./simbadName=$simbadName]"/>
        <xsl:variable name="votHtmlFileName" select="concat($simbadName,'.vot.html')"/>
        <xsl:variable name="votFileName" select="concat($simbadName,'.vot')"/>

        <xsl:if
            test="$calibrators//star[./@simbadName=$simbadName]">
            <xsl:value-of
                select="count($calibrators//star[./@simbadName=$simbadName]/calibrator)"/>
            <xsl:value-of select="' '"/>
            <xsl:value-of select="$object/ra"/>
            <xsl:value-of select="' '"/>
            <xsl:value-of select="$object/dec"/>
            <xsl:value-of select="'&#10;'"/>
            <xsl:for-each select="$calibrators//star[./@simbadName=$simbadName]/calibrator[./calibInfo/accepted]">
                <xsl:value-of select="./ra"/>
                <xsl:value-of select="' '"/>
                <xsl:value-of select="./dec"/>
                <xsl:value-of select="'&#10;'"/>
            </xsl:for-each>
        </xsl:if>
        </xsl:template>
    </xsl:stylesheet>

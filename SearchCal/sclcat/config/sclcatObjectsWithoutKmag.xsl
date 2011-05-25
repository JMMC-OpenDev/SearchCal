<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

##
# Create a file containing one block per star. 
# This file will be used to get some informations from vizier services
#
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
    extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
    exclude-result-prefixes="math str">
    <xsl:output method="text" />

    <xsl:param name="mainFile">../config/sclcatSimbadList.xml</xsl:param>

    <xsl:template match="/">
        <xsl:for-each select="//star">
            <xsl:variable name="simbadName" select="./simbadName"/>
            <xsl:variable name="object" select="document($mainFile)//object[name=$simbadName]"/>
            <xsl:if test="not($object/kmag)">
                <xsl:if test="not($object/ra and $object/dec)">
                    <xsl:message>'<xsl:value-of select="$simbadName"/>' has no ra and dec </xsl:message>
                </xsl:if>
                <xsl:if test="$object/ra and $object/dec">
                    <xsl:value-of select="'# query by coords&#10;'"/>
                    <xsl:value-of select="concat('%',$simbadName,'&#10;')"/>
                    <xsl:value-of select="concat($object/ra, ' ', $object/dec,'&#10;')"/>
                    <xsl:value-of select="'# query by name&#10;'"/>
                    <xsl:value-of select="concat('%',$simbadName,'&#10;')"/>
                    <xsl:value-of select="concat($simbadName,'&#10;&#10;')"/>
                </xsl:if>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

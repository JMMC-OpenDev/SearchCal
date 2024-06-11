<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatObjectsToDat.xsl,v 1.4 2009/11/17 20:41:20 mella Exp $"
#
# History
#
# $Log: sclcatObjectsToDat.xsl,v $
# Revision 1.4  2009/11/17 20:41:20  mella
# remove err mesg
#
# Revision 1.3  2009/11/16 14:20:56  mella
# add null value for missing kmag
#
# Revision 1.2  2009/11/16 09:09:58  mella
# add kmag and galactical coordinates in the final dat file
#
# Revision 1.1  2009/05/18 15:49:35  mella
# output table.dat
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
    <xsl:include href="url-encode.xsl"/>
	<xsl:template match="/">
        <xsl:for-each select="//star[./calibrator[not(.//seemsScienceStar) and pmra/text() and pmdec/text()] and pmra/text() and pmdec/text()]">
			<xsl:variable name="starName" select="translate(./@simbadName,' ','')"/>
            <xsl:variable name="filename" select="concat('diagdep/',$starName,'.svg')"/>
            <xsl:variable name="starRa" select="./ra"/>
			<xsl:variable name="starDec" select="./dec"/>
			<xsl:variable name="starPmRa" select="./pmra"/>
            <xsl:variable name="starPmDec" select="./pmdec"/>
            <xsl:variable name="params">
                <xsl:for-each select="./calibrator[not(.//seemsScienceStar) and pmra/text() and pmdec/text()]">
                    <xsl:value-of select="'&amp;data[]='"/>
                    <xsl:call-template name="url-encode">
                        <xsl:with-param name="str">
                            <xsl:value-of select="$starRa"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="$starDec"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="./ra"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="./dec"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="$starPmRa"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="$starPmDec"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="./pmra"/>
                            <xsl:value-of select="' '"/>
                            <xsl:value-of select="./pmdec"/>
                            <xsl:value-of select="' '"/>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:variable>
            <xsl:document href="{$filename}">
                <!-- <xsl:message>Generating <xsl:value-of select="$filename"/></xsl:message> -->
                <xsl:copy-of select="document(concat('http://jmmc.fr/~mella/diagdep/diagdep?',$params))"/>
            </xsl:document>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>

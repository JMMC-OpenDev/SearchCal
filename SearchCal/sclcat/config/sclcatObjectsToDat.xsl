<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatObjectsToDat.xsl,v 1.2 2009-11-16 09:09:58 mella Exp $"
#
# History
#
# $Log: not supported by cvs2svn $
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

	<xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
	<xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>

	<!-- acces filtered calibrators -->
	<xsl:variable name="calibrators" select="document($calibratorsFilename)"/>
	<xsl:variable name="primaStars" select="/"/>

	<xsl:template match="/">
		<xsl:text># starRa starDec calRa calDec diamFlag starKMag calKmag calGalLat calGalLon&#10;</xsl:text>
		<xsl:for-each select="$calibrators//star[./calibrator]">
			<xsl:variable name="simbadName" select="./@simbadName"/>
			<xsl:variable name="starRa" select="./ra"/>
			<xsl:variable name="starDec" select="./dec"/>
			<xsl:variable name="starKMag" select="./kmag"/>
			<xsl:for-each select="./calibrator[not(.//seemsScienceStar)]">
				<xsl:value-of select="$starRa"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="$starDec"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="./ra"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="./dec"/>
				<xsl:value-of select="' '"/>
				<xsl:choose>
					<xsl:when test=".//accepted">
						<xsl:value-of select="'OK '"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'NOK '"/>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:value-of select="$starKMag"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="./kmag"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="./glat"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="./glon"/>
				<xsl:value-of select="' '"/>
				<xsl:value-of select="'&#10;'"/>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>

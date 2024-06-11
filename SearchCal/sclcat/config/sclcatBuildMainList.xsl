<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

##
# Create the list of PRIMA stars in a xml file according to the exoplanet file
# and the aliases file
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
    xmlns:exoplanet="http://exoplanet.eu"
    extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
    exclude-result-prefixes="math str">
    <xsl:output method="xml" 
        indent="yes"
        encoding="UTF-8"
				/>

    <xsl:param name="aliasFile">../config/sclcatAliases.xml</xsl:param>

    <xsl:template match="/">
        <xsl:comment> Generated with sclcatBuildMain.xsl</xsl:comment>
        <list>
            <xsl:if test="//catalogue">
                <xsl:message>Using old exoplanet catalogue</xsl:message>
                <xsl:call-template name="exoplanet" select="/" /> 
            </xsl:if>
            <xsl:if test="//exoplanet:VOTABLE">
                <xsl:message>Using new votable exoplanet catalogue </xsl:message>
                <xsl:call-template name="exoplanet2" select="/" /> 
            </xsl:if>
        </list>
    </xsl:template>
    
    <!-- This template remove the namespace of current node and associated
    descendants -->
    <xsl:template name="removeNS">
        <xsl:element name="{local-name()}">
            <xsl:copy-of select="@*|text()"/>
            <xsl:for-each select="./exoplanet:*">
                <xsl:call-template name="removeNS"/>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>

    <xsl:template name="exoplanet2">

        <xsl:variable name="votable">
            <xsl:for-each select="/exoplanet:*">
                <xsl:call-template name="removeNS"/>
            </xsl:for-each>
        </xsl:variable>
        <xsl:variable name="starList">
            <xsl:for-each select="//exoplanet:TR">
                <e>
                    <xsl:value-of select="./exoplanet:TD[13]" />
                </e>
            </xsl:for-each>
        </xsl:variable>

        <xsl:for-each select="set:distinct(exslt:node-set($starList)/e)">
            <xsl:variable name="starName">
                <xsl:value-of select="." />
            </xsl:variable>
            <xsl:variable name="simbadName">
                <xsl:choose>
                    <xsl:when test="document($aliasFile)//object[@name=$starName]">
                        <xsl:value-of select="document($aliasFile)//object[@name=$starName]/@alias"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="$starName"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
						<star>
										<xsl:comment>ra extracted from exoplanet votable</xsl:comment>
										<ra>
                    <xsl:for-each select="exslt:node-set($votable)//TR[TD[13]=$starName]">
														<xsl:if test="position()=1">
																		<xsl:value-of select="./TD[21]"/>
														</xsl:if>
                    </xsl:for-each>
										</ra>
                <name><xsl:value-of select="$starName"/></name>
                <exoplanetName><xsl:value-of select="$starName"/></exoplanetName>
                <xsl:if test="document($aliasFile)//object[@name=$starName]">
                    <alias><xsl:value-of select="document($aliasFile)//object[@name=$starName]/@alias"/></alias>
                </xsl:if>
                <simbadName><xsl:value-of select="$simbadName"/></simbadName>

                <planet>
                    <xsl:for-each
                        select="exslt:node-set($votable)//TR[TD[13]=$starName]">
                        <name><xsl:value-of select="./TD[1]"/></name>
                    </xsl:for-each>
                </planet>
            </star>
        </xsl:for-each>
    </xsl:template>

    <xsl:template name="exoplanet">
        <xsl:for-each select="//star">

            <xsl:variable name="starName">
                <xsl:if test="name/@cat">                    
                    <xsl:value-of select="concat(name/@cat, ' ')" />
                </xsl:if>
                <xsl:value-of select="name" />
            </xsl:variable>

            <xsl:variable name="simbadName">
                <xsl:choose>
                    <xsl:when test="document($aliasFile)//object[@name=$starName]">
                        <xsl:value-of select="document($aliasFile)//object[@name=$starName]/@alias"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="$starName"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <star>
                <name><xsl:value-of select="$starName"/></name>
                <exoplanetName><xsl:value-of select="name"/></exoplanetName>
                <!-- <xsl:if test="document($aliasFile)//object[name=$starName]"/> -->
                <xsl:if test="document($aliasFile)//object[@name=$starName]">
                    <alias><xsl:value-of select="document($aliasFile)//object[@name=$starName]/@alias"/></alias>
                </xsl:if>
                <simbadName><xsl:value-of select="$simbadName"/></simbadName>
                <xsl:for-each select="./planet">
                    <planet>
                        <name><xsl:value-of select="name"/></name>
                    </planet>
                </xsl:for-each>
            </star>
        </xsl:for-each>
    </xsl:template>

</xsl:stylesheet>

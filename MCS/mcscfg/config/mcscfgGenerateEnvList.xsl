<?xml version="1.0"?>

<!--
********************************************************************************
JMMC project

"@(#) $Id: mcscfgGenerateEnvList.xsl,v 1.6 2006-10-23 12:03:04 mella Exp $"

History
~~~~~~~
$Log: not supported by cvs2svn $
Revision 1.5  2005/12/06 07:44:12  mella
Generate default localhost if no hostname founded

Revision 1.4  2005/12/02 13:49:06  mella
Do not generate localhost anymore

Revision 1.3  2005/12/02 12:57:53  mella
Add Common Header


********************************************************************************
NAME
mcscfgGenerateEnvList.xsl

DESCRIPTION
generate on simple envlist file from a given xml decription and given hostname
if the hostname is not found into the description, the default localhost entry
is outputed.
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
    <xsl:output omit-xml-declaration="yes" indent="no"/>
    <xsl:param name="hostname"></xsl:param>
    
    <xsl:template match="/">
        <xsl:choose>
            <xsl:when test="//host[@name=$hostname]">	
                <xsl:call-template name="t1"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:for-each select="//env[parent::host/@name='localhost']">
                    <xsl:value-of select="./@name"/>
                    <xsl:value-of select="' '"/>
                    <xsl:value-of select="./parent::host/@name"/>
                    <xsl:value-of select="' '"/>
                    <xsl:value-of select="./port"/>
                    <xsl:value-of select="'&#10;'"/>
                </xsl:for-each>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="t1">
        <xsl:for-each select="//env[parent::host/@name=$hostname]">
            <xsl:value-of select="@name"/>
            <xsl:value-of select="' '"/>
            <xsl:value-of select="'localhost'"/>
            <xsl:value-of select="' '"/>
            <xsl:value-of select="port"/>
            <xsl:value-of select="'&#10;'"/>
        </xsl:for-each>
    </xsl:template>

</xsl:stylesheet>

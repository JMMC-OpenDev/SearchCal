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
    <xsl:output method="xml" 
        indent="yes"
        doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
        encoding="ISO-8859-1"
        doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

    <xsl:param name="outputFile">../config/sclcatPrimaCandidates.xml</xsl:param>
    <xsl:param name="aliasFile">../config/sclcatAliases.xml</xsl:param>

    <xsl:template match="/">
        <list>
            <xsl:if test="//catalogue">
                <xsl:call-template name="exoplanet" select="/" /> 
            </xsl:if>
        </list>
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
                <!--
                <xsl:if test="document($aliasFile)//object[name=$starName]">
                    -->
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

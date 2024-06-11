<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    <xsl:output method="text"/>

    <xsl:include href="sclcatObjectsCommon.xsl"/>
    
    <xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
    <xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>

    <!-- acces filtered calibrators -->
    <xsl:variable name="calibrators" select="document($calibratorsFilename)"/>
    <xsl:variable name="primaStars" select="/"/>

    <xsl:template match="/">
        <!-- Column header 
        <xsl:for-each select="exslt:node-set($columns)/*">
            <xsl:value-of select="' '"/>
            <xsl:value-of select="@desc"/>
            <xsl:value-of select="' &amp; '"/>
        </xsl:for-each>
        <xsl:value-of select="'&#10;'"/>
        <xsl:for-each select="exslt:node-set($columns)/*">
            <xsl:value-of select="' '"/>
            <xsl:value-of select="@unit"/>
            <xsl:value-of select="' &amp; '"/>
        </xsl:for-each>
        <xsl:value-of select="'\\&#10;'"/>
        -->
        <!-- table for all stars with one or more calibrator-->
        <xsl:for-each select="$calibrators//star[./calibrator[./calibInfo/accepted]]">
								<!--<xsl:sort select="./calibrator[1]/dist" data-type="number" />-->
            <xsl:sort select="translate(ra,' ','')" data-type="number" />
            <xsl:variable name="simbadName" select="./@simbadName"/>
            <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
            <xsl:call-template name="starInTable">
                <xsl:with-param name="object" select="$object"/>
            </xsl:call-template>    
        </xsl:for-each>
    </xsl:template>
    
    <!-- this template surround by $ the given string or the first word of the given string ifany white space is encountered
    -->
    <xsl:template name="latexFilter">
        <xsl:param name="inputStr"/>
        <xsl:for-each select="str:tokenize($inputStr)">
            <xsl:if test="starts-with(.,'-')"><xsl:value-of select="'$'"/></xsl:if>
            <xsl:if test="starts-with(.,'+')"><xsl:value-of select="'$'"/></xsl:if>
            <xsl:value-of select="."/>
            <xsl:if test="starts-with(.,'-')"><xsl:value-of select="'$'"/></xsl:if>
            <xsl:if test="starts-with(.,'+')"><xsl:value-of select="'$'"/></xsl:if>
            <xsl:if test="not(position()=last())">
                <xsl:value-of select="' '"/>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

    <xsl:template name="starInTable">
        <xsl:param name="object"/>
        <xsl:variable name="simbadName" select="$object/name"/>
        <xsl:variable name="primaStar" select="$primaStars//star[./simbadName=$simbadName]"/>
        <xsl:variable name="votHtmlFileName" select="concat($simbadName,'.vot.html')"/>
        <xsl:variable name="votFileName" select="concat($simbadName,'.vot')"/>
        <xsl:text>\noalign{\smallskip}\hline\noalign{\smallskip}&#10;</xsl:text> 
        <!-- star line -->
        <xsl:for-each select="exslt:node-set($columns)/*[not(@name='dist')]">
            <xsl:variable name="selector" select="@name"/>
            <xsl:variable name="value">
                <xsl:choose>
                    <xsl:when test="$selector='name'">
                        <xsl:value-of select="$primaStar/name"/>
                    </xsl:when>
                    <xsl:when test="$selector='orbit'">
                        <xsl:value-of select="$calibrators//star[./@simbadName=$simbadName]/orbit/@latex"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="$object/*[name()=$selector]"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            <xsl:call-template name="latexFilter">
                <xsl:with-param name="inputStr" select="$value"/>
            </xsl:call-template>    
            <xsl:choose>
                <xsl:when test="position()=last()">
                    <xsl:value-of select="'\\&#10;'"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="'&amp;'"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
        <!-- place one line per accepted calibrator -->
        <xsl:for-each select="$calibrators//star[./@simbadName=$simbadName]/calibrator[./calibInfo/accepted]">
            <xsl:variable name="tmpCalib" select="."/>
            <xsl:for-each select="exslt:node-set($columns)/*[not(@name='dist')]">
                <xsl:variable name="selector" select="@name"/>
                <xsl:variable name="value">
                    <xsl:choose>
                        <xsl:when test="$selector='dist'">
                            <!-- mult by 60 to be in arcmin -->
                            <xsl:value-of select="$tmpCalib/dist * 60"/>
                        </xsl:when>
                        <xsl:when test="$selector='name'">
                        </xsl:when>
                        <xsl:when test="$selector='dist1'">
                            <xsl:value-of select="$tmpCalib/calibInfo/dist[1]/value"/>
                        </xsl:when>
                        <xsl:when test="$selector='dist2'">
                            <xsl:value-of select="$tmpCalib/calibInfo/dist[2]/value"/>
                        </xsl:when>
                        <xsl:when test="$selector='minDist'">
                            <xsl:if test="number($tmpCalib/calibInfo/minDist/value)">
                                <xsl:value-of select="$tmpCalib/calibInfo/minDist/value"/>
                                <xsl:value-of select="' in '"/>
                                <xsl:value-of select="$tmpCalib/calibInfo/minDist/year"/>
                            </xsl:if>
                        </xsl:when>
                        <xsl:when test="$selector='calibrators'">
                            <xsl:value-of  select="concat('2M',$tmpCalib/name)"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$tmpCalib/*[name()=$selector]"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>
                <xsl:call-template name="latexFilter">
                    <xsl:with-param name="inputStr" select="$value"/>
                </xsl:call-template>    
                <xsl:choose>
                    <xsl:when test="position()=last()">
                        <xsl:value-of select="'\\&#10;'"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="'&amp;'"/>
                    </xsl:otherwise>
                </xsl:choose>

            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

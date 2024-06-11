<?xml version="1.0" encoding="UTF-8" ?>
<!--
********************************************************************************
* JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
********************************************************************************
DESCRIPTION
- transform VOTable into CSV files
- accept external parameter to specify field separator (fieldSeparator)
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:VOT="http://www.ivoa.net/xml/VOTable/v1.3"
                xmlns:exslt="http://exslt.org/common"
                extension-element-prefixes="exslt VOT">

    <xsl:output method="text" encoding="UTF-8" />

    <xsl:param name="fieldSeparator"><xsl:text>&#x9;</xsl:text></xsl:param>
   		
    <xsl:template match="/">
        <xsl:apply-templates select="VOT:VOTABLE" />
    </xsl:template>


    <xsl:template match="VOT:VOTABLE">
        <xsl:text>#  This file was generated using sclguiVOTableToTSV.xsl</xsl:text>

        <!-- Print description -->
        <xsl:text>&#10;#</xsl:text>
        <xsl:apply-templates select="VOT:DESCRIPTION"/>
        <xsl:text>&#10;</xsl:text>

        <xsl:variable name="table" select="VOT:RESOURCE/VOT:TABLE"/>

        <!-- store mappings for future output of TD linked to non hidden fields -->
        <xsl:variable name="mappings">
            <xsl:for-each select="$table/VOT:FIELD">
                <xsl:if test="not(@type='hidden')">
                    <xsl:element name="mapping">
                        <xsl:attribute name="pos"><xsl:value-of select="position()"/></xsl:attribute>
                    </xsl:element>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>
        <xsl:variable name="mappingNodeSet" select="exslt:node-set($mappings)" />

        <!--
        <xsl:message>
            <xsl:for-each select="$mappingNodeSet/*">
                mapping {
                    pos: <xsl:value-of select="@pos"/>
                }
            </xsl:for-each>
        </xsl:message>
        -->

        <!-- Print field ucd -->
        <xsl:for-each select="$mappingNodeSet/*">
            <xsl:variable name="iPos" select="number(@pos)"/>
            <xsl:value-of disable-output-escaping="yes" select="$table/VOT:FIELD[$iPos]/@ucd"/>
            <xsl:value-of disable-output-escaping="yes" select="$fieldSeparator"/>
        </xsl:for-each>

        <!-- Print fields name as header (no confidence nor origin fields) -->
        <xsl:text>&#10;</xsl:text>
        <xsl:for-each select="$mappingNodeSet/*">
            <xsl:variable name="iPos" select="number(@pos)"/>
            <xsl:value-of disable-output-escaping="yes" select="$table/VOT:FIELD[$iPos]/@name"/>
            <xsl:value-of disable-output-escaping="yes" select="$fieldSeparator"/>
        </xsl:for-each>

        <!-- Print field unit -->
        <xsl:text>&#10;# </xsl:text>
        <xsl:for-each select="$mappingNodeSet/*">
            <xsl:variable name="iPos" select="number(@pos)"/>
            <xsl:value-of disable-output-escaping="yes" select="$table/VOT:FIELD[$iPos]/@unit"/>
            <xsl:value-of disable-output-escaping="yes" select="$fieldSeparator"/>
        </xsl:for-each>

        <!-- Print data -->
        <xsl:apply-templates select="$table/VOT:DATA/VOT:TABLEDATA/VOT:TR">
            <xsl:with-param name="mappingNodeSet" select="$mappingNodeSet"/>
        </xsl:apply-templates>
    </xsl:template>


    <xsl:template match="VOT:TR">
        <xsl:param name="mappingNodeSet" />
        <xsl:text>&#10;</xsl:text>
        <xsl:apply-templates select="$mappingNodeSet/*">
            <xsl:with-param name="trNode" select="."/>
        </xsl:apply-templates>
    </xsl:template>


    <!-- note: xsltproc requires xhtml namespace: match="xhtml:mapping" -->
    <xsl:template match="mapping">
        <xsl:param name="trNode" />
        <xsl:variable name="iPos" select="number(@pos)"/>
        <xsl:value-of disable-output-escaping="yes" select="$trNode/VOT:TD[$iPos]/text()"/>
        <xsl:value-of disable-output-escaping="yes" select="$fieldSeparator"/>
    </xsl:template>


    <!-- Read line separated description and output them prefixed with dash --> 
    <xsl:template match="VOT:DESCRIPTION"><xsl:text>#  </xsl:text>
        <xsl:call-template name="SubstringReplace">
            <xsl:with-param name="stringIn" select="."/>
            <xsl:with-param name="substringIn" select="'&#10;'"/>
            <xsl:with-param name="substringOut" select="'&#10;#'"/>
        </xsl:call-template>
    </xsl:template>


    <xsl:template name="SubstringReplace">
        <xsl:param name="stringIn"/>
        <xsl:param name="substringIn"/>
        <xsl:param name="substringOut"/>
        <xsl:choose>
            <xsl:when
                test="contains($stringIn,$substringIn)">
                <xsl:value-of select="concat(substring-before($stringIn,$substringIn),$substringOut)"/>
                <xsl:call-template name="SubstringReplace">
                    <xsl:with-param name="stringIn" select="substring-after($stringIn,$substringIn)"/>
                    <xsl:with-param name="substringIn" select="$substringIn"/>
                    <xsl:with-param name="substringOut" select="$substringOut"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$stringIn"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>


</xsl:stylesheet>

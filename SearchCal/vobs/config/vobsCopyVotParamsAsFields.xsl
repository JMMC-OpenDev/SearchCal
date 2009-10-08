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
 xmlns:VOT="http://www.ivoa.net/xml/VOTable/v1.1"
 xmlns="http://www.ivoa.net/xml/VOTable/v1.1"
 extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
 exclude-result-prefixes="math str">
 <xsl:output method="xml" indent="yes" />
 <xsl:template match="/">
     <xsl:apply-templates />
 </xsl:template>

<!-- add one TD in all TR for every param at the beginning -->
<xsl:template match="//VOT:TR" priority="10">
    <xsl:element name="{name()}">
        <xsl:apply-templates select="./@*"/>
        <xsl:for-each select="//VOT:PARAM">
            <xsl:element name="TD">
                <xsl:value-of select="@value"/>
            </xsl:element>
            <xsl:element name="TD">
            </xsl:element>
            <xsl:element name="TD">
            </xsl:element>
        </xsl:for-each>
        <xsl:apply-templates />
    </xsl:element>
</xsl:template>

<!-- add one FIELD for every param at the beginning -->
<xsl:template match="//VOT:FIELD[position()=1]" priority="10">
    <xsl:for-each select="//VOT:PARAM">
        <xsl:variable name="paramName" select="@name"/>
        <xsl:element name="FIELD">
            <xsl:apply-templates select="./@*[not(name()='value')]"/>
            <xsl:attribute name="ID">
                <xsl:value-of select="concat('id_',$paramName)"/>
            </xsl:attribute>
            <xsl:element name="DESCRIPTION"> Is a copy of a param extracted from original searchCal votable </xsl:element>
        </xsl:element>
        <xsl:element name="FIELD">
            <xsl:attribute name="ID">
                <xsl:value-of select="concat('id_',$paramName,'.origin')"/>
            </xsl:attribute>
        </xsl:element>
        <xsl:element name="FIELD">
            <xsl:attribute name="ID">
                <xsl:value-of select="concat('id_',$paramName,'.confidence')"/>
            </xsl:attribute>
        </xsl:element>
    </xsl:for-each>
    <xsl:element name="{name()}">
        <xsl:apply-templates select="./@*"/>
       <xsl:apply-templates />
    </xsl:element>
</xsl:template>

<!-- add one GROUP for every param at the beginning -->
<xsl:template match="//VOT:GROUP[position()=1]" priority="10">
    <xsl:for-each select="//VOT:PARAM">
        <xsl:variable name="paramName" select="@name"/>
        <xsl:element name="GROUP">
                <xsl:attribute name="name">
                    <xsl:value-of select="$paramName"/>
                </xsl:attribute>
            <xsl:element name="FIELDref">
                <xsl:attribute name="ref">
                    <xsl:value-of select="concat('id_',$paramName)"/>
                </xsl:attribute>
            </xsl:element>
            <xsl:element name="FIELDref">
                <xsl:attribute name="ref">
                    <xsl:value-of select="concat('id_',$paramName,'_origin')"/>
                </xsl:attribute>
            </xsl:element>
            <xsl:element name="FIELDref">
                <xsl:attribute name="ref">
                    <xsl:value-of select="concat('id_',$paramName,'_confidence')"/>
                </xsl:attribute>
            </xsl:element>
        </xsl:element>
    </xsl:for-each>
    <xsl:element name="{name()}">
        <xsl:apply-templates select="./@*"/>
       <xsl:apply-templates />
    </xsl:element>
</xsl:template>


<!-- Copy elements as original -->
<xsl:template match="*" priority="1">
    <xsl:element name="{name()}">
        <xsl:apply-templates select="./@*"/>
        <xsl:apply-templates />
    </xsl:element>
</xsl:template>

<!-- Copy comment text and attributes as original -->
<xsl:template match="comment()|text()|@*">
    <xsl:copy-of select="."/>
</xsl:template>

</xsl:stylesheet>

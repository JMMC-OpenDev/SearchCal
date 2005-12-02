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
<xsl:output omit-xml-declaration="yes" indent="no"/>
<xsl:param name="hostname"></xsl:param>
<xsl:template match="/">
  <xsl:call-template name="t1"/>
  <xsl:call-template name="t2"/>
</xsl:template>
<xsl:template name="t1">
    <xsl:for-each select="//env[parent::host/@name=$hostname]">
    <xsl:value-of select="@name"/>
    <xsl:value-of select="' '"/>
    <xsl:value-of select="parent::host/@name"/>
    <xsl:value-of select="' '"/>
    <xsl:value-of select="."/>
    <xsl:value-of select="'&#10;'"/>
  </xsl:for-each>
</xsl:template>
<xsl:template name="t2">
    <xsl:for-each select="//env[parent::host/@name=$hostname]">
    <xsl:value-of select="@name"/>
    <xsl:value-of select="' localhost '"/>
    <xsl:value-of select="."/>
    <xsl:value-of select="'&#10;'"/>
  </xsl:for-each>
</xsl:template>
</xsl:stylesheet>

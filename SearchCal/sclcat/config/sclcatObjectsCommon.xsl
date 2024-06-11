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
    <xsl:variable name="columns">
        <c name="name" desc="Star name" />
        <c name="calibrators" desc="Calibrators"/>
        <c name="ra" desc="Right ascension" unit="(h min s)"/>
        <c name="dec" desc="Declination" unit="(\degr\ \arcmin\ \arcsec\)"/>
        <c name="pmra" desc="Proper" unit="motion ($\alpha$)"/>
        <c name="pmdec" desc="Proper" unit="motion ($\delta$)"/>
        <c name="plx" desc="Parallax" unit="(mas)"/>
        <c name="orbit" desc="Size of" unit="(mas)"/>
        <c name="kmag" desc="K mag" unit=""/>
        <c name="dist1" desc="distance" unit=""/>
        <c name="dist2" desc="distance" unit=""/>
        <c name="minDist" desc="Minimal distance" unit=""/>
    </xsl:variable>

</xsl:stylesheet>

<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

##
# This file update the given sclcatSimbadList.xml file according to a given
# votable which results from the following post (the file content is generated 
# sclcatObjectToMissingKmag.xsl)
# this is not automated by can be run by hand
#
Content-Disposition: form-data; name="-to"
4
Content-Disposition: form-data; name="-file"
-c
Content-Disposition: form-data; name="-from"
-3
Content-Disposition: form-data; name="-this"
-3
Content-Disposition: form-data; name="-out.max"
20
Content-Disposition: form-data; name="-out.form"
VOTable
Content-Disposition: form-data; name="-order"
I
Content-Disposition: form-data; name="-c.eq"
J2000
Content-Disposition: form-data; name="-oc.form"
dec
Content-Disposition: form-data; name="-c.r"
3
Content-Disposition: form-data; name="-c.u"
arcsec
Content-Disposition: form-data; name="-c.geom"
r
Content-Disposition: form-data; name="-out.add"
_r
Content-Disposition: form-data; name="-out.add"
_RA*-c.eq,_DE*-c.eq
Content-Disposition: form-data; name="-out.add"
_RAJ,_DEJ
Content-Disposition: form-data; name="-sort"
_r
Content-Disposition: form-data; name="-source"
II/246/out
Content-Disposition: form-data; name="RAJ2000"

Content-Disposition: form-data; name="DEJ2000"

Content-Disposition: form-data; name="2MASS"

Content-Disposition: form-data; name="Jmag"

Content-Disposition: form-data; name="e_Jmag"

Content-Disposition: form-data; name="Hmag"

Content-Disposition: form-data; name="e_Hmag"

Content-Disposition: form-data; name="-out"

Kmag
Content-Disposition: form-data; name="Kmag"

Content-Disposition: form-data; name="e_Kmag"

Content-Disposition: form-data; name="Qflg"

Content-Disposition: form-data; name="Rflg"

Content-Disposition: form-data; name="Bflg"

Content-Disposition: form-data; name="Cflg"

Content-Disposition: form-data; name="Xflg"
Content-Disposition: form-data; name="Aflg"
Content-Disposition: form-data; name="-ignore"
Opt=*
Content-Disposition: form-data; name="Opt"
Opt
Content-Disposition: form-data; name="-meta.ucd"
u
Content-Disposition: form-data; name="-meta"
0
Content-Disposition: form-data; name="-meta.foot"
1
Content-Disposition: form-data; name="-ref"
VIZ4b16228d61b1
Content-Disposition: form-data; name="-out.form"
List
Content-Disposition: form-data; name="-out.add"
_1
Content-Disposition: form-data; name="-c"; filename="myList.txt"
Content-Type: text/plain
# query by coords
%Gl 581          
15 19 26.8250 -07 43 20.209
# query by name            
%Gl 581                    
Gl 581   
....
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
    <xsl:output method="xml" indent="yes"/>

    <xsl:param name="vizierFile">vizier.vot</xsl:param>

    <xsl:template match="/">
        <xsl:apply-templates />
    </xsl:template>

    <xsl:template match="//object[not(kmag)]" priority="2">
        <xsl:element name="{name()}">
            <xsl:apply-templates select="./@*"/>
            <xsl:apply-templates />
            <xsl:variable name="simbadName" select="./name"/>
            <xsl:variable name="kmag" select="document($vizierFile)//vot:TR[starts-with(vot:TD[1],$simbadName)]/vot:TD[5]"/>
            <xsl:if test="not($kmag)">
                <xsl:message>'<xsl:value-of select="$simbadName"/>' not found</xsl:message>
            </xsl:if>
            <xsl:if test="$kmag">
                <xsl:message>'<xsl:value-of select="$simbadName"/>' kmag=<xsl:value-of select="$kmag"/> </xsl:message>
                <xsl:comment>kmag extracted from 2mass</xsl:comment>
                <xsl:element name="kmag"><xsl:value-of select="$kmag"/></xsl:element>
            </xsl:if>

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

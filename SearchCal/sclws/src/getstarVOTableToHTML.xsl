<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:exslt="http://exslt.org/common"
    xmlns:math="http://exslt.org/math"
    xmlns:set="http://exslt.org/sets"
    xmlns:str="http://exslt.org/strings"
    xmlns:VOT="http://www.ivoa.net/xml/VOTable/v1.1"
    xmlns="http://www.w3.org/1999/xhtml"
    extension-element-prefixes="exslt math set str"
    exclude-result-prefixes="math str">
    <!--
    browser not supported extensions : 
    https://developer.mozilla.org/fr/EXSLT

    xmlns:func="http://exslt.org/functions"
    xmlns:date="http://exslt.org/dates-and-times"
    xmlns:dyn="http://exslt.org/dynamic"
    xmlns:saxon="http://icl.com/saxon"
    xmlns:xalanredirect="org.apache.xalan.xslt.extensions.Redirect"
    xmlns:xt="http://www.jclark.com/xt"
    xmlns:libxslt="http://xmlsoft.org/XSLT/namespace"
    xmlns:test="http://xmlsoft.org/XSLT/"
    extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"

    xmlns="http://www.ivoa.net/xml/VOTable/v1.1"
    -->

    <xsl:output method="xml" indent="yes" />
    <xsl:variable name="legend">
        <confidence ref='MEDIUM' title='MEDIUM' color="#D8D8D8" description='Medium confidence index'/>
        <confidence ref='HIGH' title='HIGH' color="#EFEFEF" description='High confidence index'/>
        <confidence ref='LOW' title='LOW' color="#6E6E6E" description='Low confidence index'/>
        <!-- use program fr.jmmc.mcs.astro.Catalog -->
  <catalog ref='I/280' title='ASCC-2.5' color='#ffaa80' description='All-sky Compiled Catalogue of 2.5 million stars'/>
  <catalog ref='I/284' title='USNO-B' color='#ffd580' description='The USNO-B1.0 Catalog'/>
  <catalog ref='II/225/catalog' title='CIO' color='#ffff80' description='Catalog of Infrared Observations, Edition 5'/>
  <catalog ref='II/7A/catalog' title='JP11' color='#d5ff80' description='UBVRIJKLMNH Photoelectric Catalogue'/>
  <catalog ref='II/246/out' title='2MASS' color='#aaff80' description='2MASS All-Sky Catalog of Point Sources'/>
  <catalog ref='V/50/catalog' title='BSC' color='#80ff80' description='Bright Star Catalogue, 5th Revised Ed.'/>
  <catalog ref='J/A+A/433/1155' title='Merand' color='#80ffaa' description='Calibrator stars for 200m baseline interferometry'/>
  <catalog ref='J/A+A/431/773/charm2' title='CHARM2' color='#80ffd5' description='CHARM2, an updated of CHARM catalog'/>
  <catalog ref='B/denis' title='DENIS' color='#80ffff' description='The DENIS database'/>
  <catalog ref='J/A+A/413/1037' title='J-K DENIS' color='#80d4ff' description='J-K DENIS photometry of bright southern stars'/>
  <catalog ref='I/196/main' title='HIC' color='#80aaff' description='Hipparcos Input Catalogue, Version 2'/>
  <catalog ref='J/A+A/393/183/catalog' title='LBSI' color='#8080ff' description='Catalogue of calibrator stars for LBSI'/>
  <catalog ref='MIDI' title='MIDI' color='#aa80ff' description='Photometric observations and angular size estimates of mid infrared interferometric calibration sources'/>
  <catalog ref='V/36B/bsc4s' title='SBSC' color='#d580ff' description='The Supplement to the Bright Star Catalogue'/>
  <catalog ref='B/sb9/main' title='SB9' color='#ff80ff' description='SB9: 9th Catalogue of Spectroscopic Binary Orbits'/>
  <catalog ref='B/wds/wds' title='WDS' color='#ff80d4' description='The Washington Visual Double Star Catalog'/>
  <catalog ref='II/297/irc' title='AKARI' color='#ff80aa' description='AKARI/IRC mid-IR all-sky Survey (ISAS/JAXA, 2010)'/>
    </xsl:variable>

    <xsl:variable name="fromRef" select="'./+'"/>
    <xsl:variable name="toRef" select="'___'"/>
    <xsl:variable name="cssHeader">
        <xsl:value-of select="'&#10;'"/>
        <style type="text/css">       
            <xsl:value-of select="'&#10;'"/>
            <xsl:comment>                  
                .legend {
                position : fixed ;
                bottom : 0 ;
                font-size : 60%;
                }
                .content {                    
                background-color:#F0F0F0;  
                border:1px solid #BBBBBB;          
                color: #010170;                    
                padding: 1px;                      
                margin: 1px;                       
                font-size : 70%;
                }                                      
                .vbox {                                
                border:1px solid #CCCCCC;              
                padding: 3px;                          
                margin: 3px;                           
                float: left;                           
                }                                      
                .box {                                 
                border:1px solid #CCCCCC;              
                padding: 3px;                          
                margin: 3px;                           
                }                                      
                table {                                
                border-collapse:collapse;      
                }                                      
                td {                                   
                border: 1px solid #000099;             
                }                                      
                <xsl:for-each select="exslt:node-set($legend)/*"><xsl:value-of select="concat('                td.',translate(@ref,$fromRef,$toRef),' { background-color : ',@color,' }&#10;')"/>
                </xsl:for-each>
                th {                                   
                border: 1px solid #000099;             
                background-color:#FFFFDD;      
                }                                      
            </xsl:comment>                         
        </style>                               

    </xsl:variable>
    <xsl:variable name="root" select="/" />

    <!-- define as global variables the frequently used sets --> 
    <xsl:variable name="fieldNodes" select="/VOT:VOTABLE/VOT:RESOURCE/VOT:TABLE/VOT:FIELD"/>
    <xsl:variable name="groupNodes" select="/VOT:VOTABLE/VOT:RESOURCE/VOT:TABLE/VOT:GROUP"/>

    <xsl:template match="/">
        <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
            <xsl:value-of select="'&#10;'"/>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
                <xsl:value-of select="'&#10;'"/>
                <link rel="stylesheet" href="/css/2col_leftNav.css" type="text/css" />
<!--                <link rel="stylesheet" href="/badcss/catalogues.css" type="text/css" /> -->
                <xsl:value-of select="'&#10;'"/>
                <xsl:copy-of select="$cssHeader"/>
                <xsl:value-of select="'&#10;'"/>
            </head>
            <body>

                <pre class="box">
                    <xsl:copy-of select="/VOT:VOTABLE/VOT:DESCRIPTION"/>
                </pre>

                <xsl:if test=".//VOT:INFO">
                    <pre class="box error">
                        <xsl:for-each select=".//VOT:INFO">
                            <xsl:copy-of select="."/>
                        </xsl:for-each>
                    </pre>
                </xsl:if>


                <table class="centered coloredtable nowrap">
                    <xsl:for-each select="$groupNodes">
                        <tr>
                            <xsl:variable name="groupIndex" select="position()"/>
                            <xsl:variable name="FIELD"
                                select="$fieldNodes[position()=($groupIndex*3)-2]"/>
                            <xsl:element name="td">
                                <xsl:value-of select="' '"/>
                                <xsl:element name="a">
                                    <xsl:if test="$FIELD/VOT:DESCRIPTION">
                                        <xsl:attribute name="title">
                                            <xsl:value-of select="$FIELD/VOT:DESCRIPTION"/>
                                        </xsl:attribute>
                                    </xsl:if>
                                    <xsl:value-of select="@name"/>
                                    <xsl:if test="@unit">
                                        <xsl:value-of select="concat('[',@unit,']')"/>
                                    </xsl:if>
                                </xsl:element>
                                <xsl:value-of select="' '"/>
                            </xsl:element>
                            <xsl:variable name="trNode" select="/VOT:VOTABLE/VOT:RESOURCE/VOT:TABLE/VOT:DATA/VOT:TABLEDATA/VOT:TR"/>
                            <!--                           <xsl:apply-templates select="/VOT:VOTABLE/VOT:RESOURCE/VOT:TABLE/VOT:DATA/VOT:TABLEDATA/VOT:TR/VOT:TD[position()=($groupIndex*3)-2]"/>-->


                            <xsl:variable name="value" select="concat(' ',$trNode/VOT:TD[($groupIndex * 3) - 2],' ')"/>
                            <xsl:variable name="origin" select="translate($trNode/VOT:TD[($groupIndex * 3) - 1],'/+.','___')"/>
                            <xsl:variable name="confidence" select="translate($trNode/VOT:TD[$groupIndex * 3],'/+.','___')"/>
                            <xsl:variable name="link" select="$fieldNodes[position()=($groupIndex*3)-2]/VOT:LINK/@href"/>

                            <xsl:element name="td">
                                <xsl:attribute name="class">
                                    <xsl:value-of select="concat($origin,' ', $confidence)"/>
                                </xsl:attribute>
                                <xsl:choose>
                                    <xsl:when test="$link and $value">
                                        <xsl:variable name="href">
                                            <xsl:value-of select="str:split($link,'${')[1]"/>
                                            <xsl:for-each select="str:split($link,'${')">
                                                <xsl:if test="position()>1">
                                                    <xsl:variable name="fname" select="substring-before(.,'}')"/>
                                                    <xsl:variable name="cindex">
                                                        <xsl:call-template name="getFieldIndex">
                                                            <xsl:with-param name="fname"><xsl:value-of select="$fname"/></xsl:with-param>
                                                        </xsl:call-template>
                                                    </xsl:variable>
                                                    <xsl:for-each select="$trNode/VOT:TD">
                                                        <xsl:if test="position()=$cindex">
                                                            <xsl:value-of select="."/>
                                                        </xsl:if>
                                                    </xsl:for-each>
                                                    <xsl:value-of select="substring-after(.,'}')"/>
                                                </xsl:if>
                                            </xsl:for-each>
                                        </xsl:variable>
                                        <xsl:element name="a">
                                            <xsl:attribute name="href">
                                                <xsl:value-of select="$href"/>
                                            </xsl:attribute>
                                            <xsl:value-of select="$value"/>
                                        </xsl:element>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:value-of select="$value"/>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:element>

                            <xsl:element name="td">
                                <xsl:value-of select="' '"/>
                                <xsl:if test="$FIELD/@unit">
                                    <xsl:value-of select="$FIELD/@unit"/>
                                </xsl:if>
                                <xsl:value-of select="' '"/>
                            </xsl:element>

                            <xsl:element name="td">
                                <xsl:value-of select="' '"/>
                                <xsl:value-of select="$FIELD/VOT:DESCRIPTION"/>
                                <xsl:value-of select="' '"/>
                            </xsl:element>

                            <xsl:element name="td">
                                <xsl:value-of select="' '"/>
                                <xsl:if test="$FIELD/@ucd">
                                    <xsl:value-of select="$FIELD/@ucd"/>
                                </xsl:if>
                                <xsl:value-of select="' '"/>
                            </xsl:element>



                        </tr>
                    </xsl:for-each>
                </table>
                <table class="coloredtable nowrap legend">
                    <tr>
                        <xsl:for-each select="exslt:node-set($legend)/*">
                            <td class="{translate(@ref,$fromRef,$toRef)}">
                                <a title="{@description}"
                                href="http://cdsarc.u-strasbg.fr/cgi-bin/VizieR?-source={@ref}">
                                    <xsl:value-of select="@title"/>
                                </a>
                            </td>
                        </xsl:for-each>
                    </tr>
                </table>
                <br/>
                <br/>
                <pre>
                <xsl:value-of select="document('log.xml')"/>
                </pre>
                <br/>
                <br/>
            </body>
        </html>
    </xsl:template>

    <xsl:template name="getFieldIndex">
        <xsl:param name="fname"/>
        <xsl:for-each select="$fieldNodes">
            <xsl:if test="@name=$fname">
                <xsl:value-of select="concat(position(),' ')"/>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

    <xsl:template match="VOT:TD" priority="10">
        <xsl:param name="trNode" select=".."/>
        <xsl:param name="groupIndex" select="position()"/>
        <xsl:variable name="value" select="concat(' ',$trNode/VOT:TD[($groupIndex * 3) - 2],' ')"/>
        <xsl:variable name="origin" select="translate($trNode/VOT:TD[($groupIndex * 3) - 1],'/+.','___')"/>
        <xsl:variable name="confidence" select="translate($trNode/VOT:TD[$groupIndex * 3],'/+.','___')"/>
        <xsl:variable name="link" select="$fieldNodes[position()=($groupIndex*3)-2]/VOT:LINK/@href"/>
        <xsl:element name="td">
            <xsl:attribute name="class">
                <xsl:value-of select="concat($origin,' ', $confidence)"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$link and $value">
                    <xsl:variable name="href">
                        <xsl:value-of select="str:split($link,'${')[1]"/>
                        <xsl:for-each select="str:split($link,'${')">
                            <xsl:if test="position()>1">
                                <xsl:variable name="fname" select="substring-before(.,'}')"/>
                                <xsl:variable name="cindex">
                                    <xsl:call-template name="getFieldIndex">
                                        <xsl:with-param name="fname"><xsl:value-of select="$fname"/></xsl:with-param>
                                    </xsl:call-template>
                                </xsl:variable>
                                <xsl:for-each select="$trNode/VOT:TD">
                                    <xsl:if test="position()=$cindex">
                                        <xsl:value-of select="."/>
                                    </xsl:if>
                                </xsl:for-each>
                                <xsl:value-of select="substring-after(.,'}')"/>
                            </xsl:if>
                        </xsl:for-each>
                    </xsl:variable>
                    <xsl:element name="a">
                        <xsl:attribute name="href">
                            <xsl:value-of select="$href"/>
                        </xsl:attribute>
                        <xsl:value-of select="$value"/>
                    </xsl:element>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$value"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <!-- add one TD in all TR for every param at the beginning -->
    <xsl:template match="VOT:TR" priority="10">
        <xsl:variable name="trNode" select="."/>
        <xsl:element name="tr">
            <xsl:apply-templates select="./@*"/>
            <td><xsl:value-of select="position()"/></td>
            <xsl:for-each select="$groupNodes">
                <xsl:variable name="groupIndex" select="position()"/>
                <!--<xsl:variable name="value" select="regexp:replace($trNode/VOT:TD[($groupIndex * 3) - 2],' ','gi','&#160;')"/>-->
                <xsl:variable name="value" select="concat(' ',$trNode/VOT:TD[($groupIndex * 3) - 2],' ')"/>
                <xsl:variable name="origin" select="translate($trNode/VOT:TD[($groupIndex * 3) - 1],'/+.','___')"/>
                <xsl:variable name="confidence" select="translate($trNode/VOT:TD[$groupIndex * 3],'/+.','___')"/>
                <xsl:variable name="link" select="$fieldNodes[position()=($groupIndex*3)-2]/VOT:LINK/@href"/>
                <xsl:element name="td">
                    <xsl:attribute name="class">
                        <xsl:value-of select="concat($origin,' ', $confidence)"/>
                    </xsl:attribute>
                    <xsl:choose>
                        <xsl:when test="$link and $value">
                            <xsl:variable name="href">
                                <xsl:value-of select="str:split($link,'${')[1]"/>
                                <xsl:for-each select="str:split($link,'${')">
                                    <xsl:if test="position()>1">
                                        <xsl:variable name="fname" select="substring-before(.,'}')"/>
                                        <xsl:variable name="cindex">
                                            <xsl:call-template name="getFieldIndex">
                                                <xsl:with-param name="fname"><xsl:value-of select="$fname"/></xsl:with-param>
                                            </xsl:call-template>
                                        </xsl:variable>
                                        <xsl:for-each select="$trNode/VOT:TD">
                                            <xsl:if test="position()=$cindex">
                                                <xsl:value-of select="."/>
                                            </xsl:if>
                                        </xsl:for-each>
                                        <xsl:value-of select="substring-after(.,'}')"/>
                                    </xsl:if>
                                </xsl:for-each>
                            </xsl:variable>
                            <xsl:element name="a">
                                <xsl:attribute name="href">
                                    <xsl:value-of select="$href"/>
                                </xsl:attribute>
                                <xsl:value-of select="$value"/>
                            </xsl:element>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$value"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>
</xsl:stylesheet>

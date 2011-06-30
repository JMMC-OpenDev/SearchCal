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
    <xsl:variable name="legend">
        <confidence ref='MEDIUM' title='MEDIUM' color="#D8D8D8" description='Medium confidence index'/>
        <confidence ref='HIGH' title='HIGH' color="#EFEFEF" description='High confidence index'/>
        <confidence ref='LOW' title='LOW' color="#6E6E6E" description='Low confidence index'/>
        <!-- use program fr.jmmc.mcs.astro.Catalog -->
        <catalog ref='I/280' title='ASCC-2.5' color='#ffad80' description='All-sky Compiled Catalogue of 2.5 million stars'/>
        <catalog ref='I/284' title='USNO-B' color='#ffda80' description='The USNO-B1.0 Catalog'/>
        <catalog ref='II/225/catalog' title='CIO' color='#f8ff80' description='Catalog of Infrared Observations, Edition 5'/>
        <catalog ref='II/7A/catalog' title='JP11' color='#cbff80' description='UBVRIJKLMNH Photoelectric Catalogue'/>
        <catalog ref='II/246/out' title='2MASS' color='#9eff80' description='2MASS All-Sky Catalog of Point Sources'/>
        <catalog ref='V/50/catalog' title='BSC' color='#80ff8f' description='Bright Star Catalogue, 5th Revised Ed.'/>
        <catalog ref='J/A+A/433/1155' title='Merand' color='#80ffbc' description='Calibrator stars for 200m baseline interferometry'/>
        <catalog ref='J/A+A/431/773/charm2' title='CHARM2' color='#80ffe9' description='CHARM2, an updated of CHARM catalog'/>
        <catalog ref='B/denis' title='DENIS' color='#80e8ff' description='The DENIS database'/>
        <catalog ref='J/A+A/413/1037' title='J-K DENIS' color='#80bcff' description='J-K DENIS photometry of bright southern stars'/>
        <catalog ref='I/196/main' title='HIC' color='#808eff' description='Hipparcos Input Catalogue, Version 2'/>
        <catalog ref='J/A+A/393/183/catalog' title='LBSI' color='#9e80ff' description='Catalogue of calibrator stars for LBSI'/>
        <catalog ref='MIDI' title='MIDI' color='#cb80ff' description='Photometric observations and angular size estimates of mid infrared interferometric calibration sources'/>
        <catalog ref='V/36B/bsc4s' title='SBSC' color='#f880ff' description='The Supplement to the Bright Star Catalogue'/>
        <catalog ref='B/sb9/main' title='SB9' color='#ff80d9' description='SB9: 9th Catalogue of Spectroscopic Binary Orbits'/>
        <catalog ref='B/wds/wds' title='WDS' color='#ff80ac' description='The Washington Visual Double Star Catalog'/>
    </xsl:variable>

    <xsl:variable name="fromRef" select="'./+'"/>
    <xsl:variable name="toRef" select="'___'"/>
    <xsl:variable name="cssHeader">
        <style type="text/css">        
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
                border: 2px solid #000099;             
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
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
                <xsl:copy-of select="$cssHeader"/>
            </head>
            <body>

                <table class="legend">
                    <tr>
                        <xsl:for-each select="exslt:node-set($legend)/*">
                            <td class="{translate(@ref,$fromRef,$toRef)}">
                                <a title="{@description}">
                                    <xsl:value-of select="@title"/>
                                </a>
                            </td>
                        </xsl:for-each>
                    </tr>
                </table>
                <table class="centered">
                    <thead>
                        <th>Index</th>
                        <xsl:for-each select="$groupNodes">
                            <xsl:variable name="groupIndex" select="position()"/>
                            <xsl:variable name="FIELD"
                                select="$fieldNodes[position()=($groupIndex*3)-2]"/>
                            <xsl:element name="th">
                                <xsl:element name="a">
                                    <xsl:attribute name="title">
                                        <xsl:value-of select="$FIELD/VOT:DESCRIPTION"/>
                                    </xsl:attribute>
                                    <xsl:value-of select="@name"/>
                                </xsl:element>
                            </xsl:element>
                        </xsl:for-each>
                    </thead>
                    <tbody>
                        <xsl:apply-templates select="/VOT:VOTABLE/VOT:RESOURCE/VOT:TABLE/VOT:DATA/VOT:TABLEDATA/VOT:TR"/>
                    </tbody>
                </table>
                <br/>
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
    <!-- add one TD in all TR for every param at the beginning -->
    <xsl:template match="VOT:TR" priority="10">
        <xsl:variable name="trNode" select="."/>
        <xsl:element name="tr">
            <xsl:apply-templates select="./@*"/>
            <td><xsl:value-of select="position()"/></td>
            <xsl:for-each select="$groupNodes">
                <xsl:variable name="groupIndex" select="position()"/>
                <xsl:variable name="value">
								<xsl:for-each select="str:split($trNode/VOT:TD[($groupIndex * 3) - 2])">
								<xsl:value-of select="concat(.,'&#160;')"/>
								</xsl:for-each>
								</xsl:variable>
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
                                <xsl:for-each
                                    select="str:split($link,'${')">
                                    <xsl:if test="position()>1">
                                        <xsl:variable name="fname" select="substring-before(.,'}')"/>
                                        <xsl:variable name="cindex">
                                            <xsl:call-template name="getFieldIndex">
                                                <xsl:with-param name="fname"><xsl:value-of select="$fname"/></xsl:with-param>
                                            </xsl:call-template>
                                        </xsl:variable>
                                        <xsl:for-each select="$trNode/VOT:TD">
                                            <xsl:if
                                                test="position()=$cindex">
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

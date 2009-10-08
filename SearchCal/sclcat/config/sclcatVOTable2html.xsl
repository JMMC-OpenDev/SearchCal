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
    <xsl:variable name="colorSets">
        <set><key>MEDIUM</key><value>#D8D8D8</value></set>
        <set><key>HIGH</key><value>#EFEFEF</value></set>
        <set><key>LOW</key><value>#6E6E6E</value></set>
        <set><key>J_A_A_433_1155</key><value>#C89292</value></set>
        <set><key>V_50_catalog</key><value>#B6FFE6</value></set>
        <set><key>II_246_out</key><value>#B6E8FF</value></set>
        <set><key>II_225_catalog</key><value>#F6B6FF</value></set>
        <set><key>V_36B_bsc4s</key><value>#88A0A6</value></set>
        <set><key>I_196_main</key><value>#78FB8B</value></set>
        <set><key>J_A_A_431_773_charm2</key><value>#B7FF5A</value></set>
        <set><key>MIDI</key><value>#C994CA</value></set>
        <set><key>I_284</key><value>#F1FB58</value></set>
        <set><key>I_280</key><value>#FFB6B6</value></set>
        <set><key>J_A_A_393_183_catalog</key><value>#9778FB</value></set>
        <set><key>B_sb9_main</key><value>#A688A0</value></set>
        <set><key>J_A_A_413_1037</key><value>#FFFADD</value></set>
        <set><key>II_7A_catalog</key><value>#B9B6FF</value></set>
        <set><key>B_denis</key><value>#FFF4B6</value></set>
    </xsl:variable>
    
    <xsl:variable name="cssHeader">
        <style type="text/css">        
            <xsl:comment>                  
                .content {                    
                background-color:#F0F0F0;  
                border:1px solid #BBBBBB;          
                color: #010170;                    
                padding: 1px;                      
                margin: 1px;                       
                font-family:Arial,Helvetica,sans-serif;
                font-size:11px;                        
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
<xsl:for-each select="exslt:node-set($colorSets)/*"><xsl:value-of select="concat('                td.',VOT:key,' { background-color : ',VOT:value,' }&#10;')"/>
                </xsl:for-each>
                th {                                   
                border: 1px solid #000099;             
                background-color:#FFFFDD;      
                }                                      
            </xsl:comment>                         
        </style>                               

    </xsl:variable>

    <xsl:template match="/">
        <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1"/>
                <xsl:copy-of select="$cssHeader"/>
            </head>
            <body>
                <table><tr>
                <xsl:for-each select="exslt:node-set($colorSets)/*">
                    <td class="{VOT:key}">
                        <xsl:value-of select="VOT:key"/>
                    </td>
                </xsl:for-each>
        </tr></table>
                <table>
                    <xsl:for-each select="//VOT:GROUP">
                        <xsl:element name="th">
                            <xsl:value-of select="@name"/>
                        </xsl:element>
                    </xsl:for-each>
                    <xsl:apply-templates select="//VOT:TR"/>
                </table>
            </body>
        </html>
    </xsl:template>

    <!-- add one TD in all TR for every param at the beginning -->
    <xsl:template match="//VOT:TR" priority="10">
        <xsl:variable name="trIndex" select="position()"/>
        <xsl:variable name="trNode" select="."/>
        <xsl:element name="tr">
            <xsl:apply-templates select="./@*"/>
            <xsl:for-each select="//VOT:GROUP">
                <xsl:variable name="groupIndex" select="position()"/>
                <xsl:variable name="value" select="$trNode/VOT:TD[($groupIndex * 3) - 2]"/>
                <xsl:variable name="origin" select="translate($trNode/VOT:TD[($groupIndex * 3) - 1],'/+.','___')"/>
                <xsl:variable name="confidence" select="translate($trNode/VOT:TD[$groupIndex * 3],'/+.','___')"/>
                <xsl:element name="td">
                        <xsl:attribute name="class">
                            <xsl:value-of select="concat($origin,' ', $confidence)"/>
                        </xsl:attribute>
                    <xsl:value-of select="$value"/>
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>
</xsl:stylesheet>

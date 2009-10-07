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
                td.J_A_A_433_1155 { background-color : #C89292; }
                td.MEDIUM { background-color : #D8D8D8; }
                td.HIGH { background-color : #EFEFEF; }
                td.MIDI { background-color : #C994CA; }
                td.V_50_catalog { background-color : #B6FFE6; }
                td.II_246_out { background-color : #B6E8FF; }
                td.II_225_catalog { background-color : #F6B6FF; }
                td.V_36B_bsc4s { background-color : #88A0A6; }
                td.I_196_main { background-color : #78FB8B; }
                td.J_A_A_431_773_charm2 { background-color : #B7FF5A; }
                td.LOW { background-color : #6E6E6E; }
                td.I_284 { background-color : #F1FB58; }
                td.I_280 { background-color : #FFB6B6; }
                td.J_A_A_393_183_catalog { background-color : #9778FB; }
                td.B_sb9_main { background-color : #A688A0; }
                td.J_A_A_413_1037 { background-color : #FFFADD; }
                td.II_7A_catalog { background-color : #B9B6FF; }
                td.B_denis { background-color : #FFF4B6; }

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
        <xsl:element name="{name()}">
            <xsl:apply-templates select="./@*"/>
            <xsl:for-each select="//VOT:GROUP">
                <xsl:variable name="groupIndex" select="position()"/>
                <xsl:variable name="value" select="//VOT:TR[$trIndex]/VOT:TD[($groupIndex * 3) - 2]"/>
                <xsl:variable name="origin" select="translate(//VOT:TR[$trIndex]/VOT:TD[($groupIndex * 3) - 1],'/+.','___')"/>
                <xsl:variable name="confidence" select="translate(//VOT:TR[$trIndex]/VOT:TD[$groupIndex * 3],'/+.','___')"/>
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

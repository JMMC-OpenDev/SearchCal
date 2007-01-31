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
    xmlns:vot="http://www.ivoa.net/xml/VOTable/v1.1"
    extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
    exclude-result-prefixes="math str">
    <xsl:output method="xml" 
        indent="yes"
        doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
        encoding="ISO-8859-1"
        doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

    <xsl:variable name="columns">
        <name/>
        <alias/>
        <candidates/>
        <calibrators/>
        <ra/>
        <dec/>
        <pmra/>
        <pmdec/>
        <plx/>
        <magK/>
        <dist/>
        <dist1/>
        <dist2/>
        <minDist/>
    </xsl:variable>

    <xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
    <xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>

    <!-- acces filtered calibrators -->
    <xsl:variable name="calibrators" select="document($calibratorsFilename)"/>

    <xsl:template match="/">
        <!--XHTML document outline-->
        <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
                <title>Source list</title>
                <link rel="stylesheet" href="./sclcat.css" type="text/css" />
                <style type="text/css">
                    <xsl:comment>
                        table {
                        border:2px solid #000099;
                        border-collapse:collapse;
                        margin:5px;
                        font-family:monospace;
                        text-align:center;
                        width: 100%;
                        /*
                        */
                        }
                        thead, tfoot {
                        background-color:#FFE099;
                        border:1px solid #6495ed;
                        }
                        tbody {
                        background-color:#FFFFFF;
                        border:1px solid #6495ed;
                        }
                        th {
                        border:1px dotted #0E0E99;
                        padding:5px;
                        background-color:#FFE099;
                        text-align:center;
                        }
                        tr
                        {
                        border:1px solid #0E0EE9;
                        padding:2px;
                        }
                        td
                        {
                        border:1px solid #0E0EE9;
                        padding:2px;
                        text-align:center;
                        }

                    </xsl:comment>
                </style>

            </head>
            <body>
                <h1> General informations </h1>
                The following list was build using <a
                    href="http://exoplanet.eu"> Exoplanet</a> potential list.
                Each data are then collected using <a href="http://simbad.u-strasbg.fr"> simbad web service </a>.
                <h1> Aliases </h1>
                To achieve data collection some aliases have been defined (most of these are
                used by Exoplanet site):
                <ul>
                    <li> E: exoplanet link</li>
                    <li> S: simbad link</li>
                </ul>
                <xsl:call-template name="listAliases"/>

                <h1> Source List </h1>
                
                <h4> Statistics </h4>
                <ul>
                    <li><xsl:value-of select="count(//star)"/> stars</li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=1])"/> stars with 1 calibrator </li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=2])"/> stars with 2 calibrators </li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)>2])"/> stars with more than 2 calibrators </li>
                    <li><xsl:value-of select="count($calibrators//calibrator)"/> calibrators</li> 
                    <li>average <xsl:value-of select="count(//star) div count($calibrators//calibrator)"/></li>
                </ul>

                <h4> Legends </h4>
                <ul>
                    <li> E: exoplanet link</li>
                    <li> S: simbad link</li>
                    <li>Orange: star has no pmra or pmdec information into simbad </li>
                    <li>Red: star has no position information into simbad </li>
                    <li>Gray: retained calibrators</li>
                </ul>
                <h4> Columns </h4>
                <ul>
                    <li> dist1 [arcmin] on year <xsl:value-of
                        select="$calibrators//calibrator[1]/calibInfo/dist[1]/year"/> </li>
                    <li> dist2 [arcmin] on year <xsl:value-of
                            select="$calibrators//calibrator[1]/calibInfo/dist[2]/year"/> </li>
                </ul>
                <!-- Print table -->
                <table>
                    <tr> 
                        <xsl:for-each select="exslt:node-set($columns)/*">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                    </tr>
                    <xsl:apply-templates select="//star" mode="table"/>
                </table>

                <hr/>
                Generated on <xsl:value-of select="date:date-time()"/><br/>
            </body>
        </html>
    </xsl:template>

    <xsl:template name="objectTo2MASSLink">
        <xsl:param name="ident" select="name"/>
        <xsl:param name="content" select="$ident"/>
        <xsl:element name="a">
            <xsl:attribute name="href">
                <xsl:value-of select="concat('http://cdsweb.u-strasbg.fr/viz-bin/VizieR-4?-source=II/246/out&amp;-out=2MASS&amp;2MASS=',$ident,'&amp;-out=Hmag&amp;-out=e_Hmag&amp;-out=Kmag&amp;-out=e_Kmag&amp;-out=Qflg&amp;-out=Rflg&amp;-out=Bflg&amp;-out=Cflg&amp;-out=Xflg&amp;-out=Aflg-meta.ucd=0')"/>
                
            </xsl:attribute>
            <xsl:value-of select="$content"/>
        </xsl:element>                       
    </xsl:template>


    <xsl:template name="objectToSimbadLink">
        <xsl:param name="ident" select="name"/>
        <xsl:param name="content" select="$ident"/>
        <xsl:element name="a">
            <xsl:attribute name="href"><xsl:value-of select="concat('http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=',$ident,'&amp;NbIdent=1&amp;Radius=10&amp;Radius.unit=arcmin&amp;submit=submit+id')"/>
            </xsl:attribute>
            <xsl:value-of select="$content"/>
        </xsl:element>                       
    </xsl:template>

    <xsl:template name="objectToExoplanetLink">
        <xsl:param name="ident" select="name"/>
        <xsl:param name="content" select="$ident"/>
        <xsl:element name="a">
            <xsl:attribute name="href"><xsl:value-of select="concat('http://exoplanet.eu/star.php?st=',$ident)"/>
            </xsl:attribute>
            <xsl:value-of select="$content"/>
        </xsl:element>                       

    </xsl:template>



    <xsl:template name="listAliases">
        <table>
            <xsl:for-each select="//star[./alias]">
                <tr>
                    <td>
                        <xsl:value-of select="name"/>
                        [
                        <xsl:call-template name="objectToSimbadLink">
                            <xsl:with-param name="ident" select="simbadName"/>
                            <xsl:with-param name="content" select="'S'"/>
                        </xsl:call-template>

                        <xsl:variable name="sourceName" select="./name"/>

                        <xsl:value-of select="' '"/>
                        <xsl:call-template name="objectToExoplanetLink">
                            <xsl:with-param name="ident" select="exoplanetName"/>
                            <xsl:with-param name="content" select="'E'"/>
                        </xsl:call-template>
                        ]


                    </td>
                    <td>
                        <xsl:call-template name="objectToSimbadLink">
                            <xsl:with-param name="ident" select="simbadName"/>
                        </xsl:call-template>
                    </td>
                </tr>
            </xsl:for-each>
        </table>

    </xsl:template>

    <xsl:template match="star" mode="table">
        <xsl:variable name="star" select="."/>
        <xsl:variable name="simbadName" select="./simbadName"/>
        <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
        <xsl:variable name="votHtmlFileName" select="concat($simbadName,'.vot.html')"/>
        <xsl:variable name="votFileName" select="concat($simbadName,'.vot')"/>
        <!-- first tr contains star info and next ones calibrators -->
        <xsl:element name="tr">
            <xsl:for-each select="exslt:node-set($columns)/*">
                <xsl:variable name="selector" select="name()"/>
                <xsl:element name="td">
                    <xsl:choose>
                        <xsl:when test="$selector='alias'">
                            <xsl:if test="$star/alias">X</xsl:if>
                        </xsl:when>
                        <xsl:when test="$selector='candidates'">
                            <xsl:value-of select="count($star/planet)"/>
                        </xsl:when>
                        <xsl:when test="$selector='name'">
                            <xsl:if test="not($object/pmra and $object/pmdec)">
                                <xsl:attribute name="style">background-color: #ff6633</xsl:attribute>
                            </xsl:if>
                            <xsl:if test="not($object/ra and $object/dec)">
                                <xsl:attribute name="style">background-color: #ff0000</xsl:attribute>
                            </xsl:if>
                            <xsl:variable name="sourceName" select="$star/*[name()=$selector]"/>
                            <xsl:value-of select="$sourceName"/>
                            [
                            <xsl:call-template name="objectToSimbadLink">
                                <xsl:with-param name="ident" select="$star/simbadName"/>
                                <xsl:with-param name="content" select="'S'"/>
                            </xsl:call-template>
                            <xsl:value-of select="' '"/>
                            <xsl:call-template name="objectToExoplanetLink">
                                <xsl:with-param name="ident" select="$star/exoplanetName"/>
                                <xsl:with-param name="content" select="'E'"/>
                            </xsl:call-template>
                            ]
                        </xsl:when>

                        <xsl:when test="$selector='calibrators'">
                            <!-- because vo table filename contains spaces we
                            can`t open them . hack just test if star is present
                            in calibrators list-->
                            <xsl:if
                                test="$calibrators//star[./@simbadName=$simbadName]">
                                <xsl:value-of select="' '"/>
                                <xsl:value-of
                                    select="count($calibrators//star[./@simbadName=$simbadName]/calibrator)"/>
                                <xsl:value-of select="'/'"/>
                                <xsl:value-of
                                    select="$calibrators//star[./@simbadName=$simbadName]/@scCount"/>
                                <xsl:value-of select="' '"/>
                                <a href="{$votFileName}">VOTABLE</a>
                                <xsl:value-of select="' '"/>
                                <a href="{$votHtmlFileName}">HTML</a>
                            </xsl:if>
                        </xsl:when>

                        <xsl:otherwise>
                            <xsl:value-of select="$object/*[name()=$selector]"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
        <!-- place one tr per calibrator -->
        <xsl:for-each select="$calibrators//star[./@simbadName=$simbadName]/calibrator">
            <xsl:variable name="tmpCalib" select="."/>
            <xsl:element name="tr">
                <xsl:attribute name="style">background-color: #CCCCCC</xsl:attribute>
                <xsl:for-each select="exslt:node-set($columns)/*">
                    <xsl:variable name="selector" select="name()"/>
                    <xsl:element name="td">
                        <xsl:choose>
                            <xsl:when test="$selector='dist'">
                                <!-- mult by 60 to be in arcmin -->
                                <xsl:value-of select="$tmpCalib/dist * 60"/>
                            </xsl:when>
                            <xsl:when test="$selector='dist1'">
                                <xsl:value-of select="$tmpCalib/calibInfo/dist[1]/value"/>
                            </xsl:when>
                            <xsl:when test="$selector='dist2'">
                                <xsl:value-of select="$tmpCalib/calibInfo/dist[2]/value"/>
                            </xsl:when>
                            <xsl:when test="$selector='minDist'">
                                <xsl:value-of select="$tmpCalib/calibInfo/minDist/value"/>
                                <xsl:value-of select="' in '"/>
                                <xsl:value-of select="$tmpCalib/calibInfo/minDist/year"/>
                            </xsl:when>
                            <xsl:when test="$selector='alias'">
                            </xsl:when>
                            <xsl:when test="$selector='candidates'">
                            </xsl:when>
                            <xsl:when test="$selector='name'">
                            </xsl:when>
                            <xsl:when test="$selector='calibrators'">
                                <xsl:call-template name="objectTo2MASSLink">
                                    <xsl:with-param name="ident" select="$tmpCalib/name"/>
                                </xsl:call-template>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="$tmpCalib/*[name()=$selector]"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:element>
                    <!--
                    <xsl:value-of select="$object/*[name()=$selector]"/>    
                    -->
                </xsl:for-each>
            </xsl:element>
        </xsl:for-each>
        <xsl:value-of select="'&#10;'"/>

    </xsl:template>

</xsl:stylesheet>

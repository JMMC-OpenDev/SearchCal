<?xml version="1.0"?>
<!--
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatObjectsToHtml.xsl,v 1.15 2009-10-26 12:31:22 mella Exp $"
#
# History
#
# $Log: not supported by cvs2svn $
# Revision 1.14  2009/10/08 20:45:37  mella
# fix output
#
# Revision 1.13  2009/08/24 12:40:08  mella
# Add description and unit into the table header
#
# Revision 1.12  2009/05/11 10:36:18  mella
# fix exoplanet name for starname column
#
# Revision 1.11  2009/05/06 13:54:34  mella
# add common column description
#
# Revision 1.10  2009/04/29 10:45:16  mella
# add orbit column in html and latex
#
# Revision 1.9  2009/04/27 10:04:35  mella
# add better comment
#
# Revision 1.8  2009/01/22 14:12:10  mella
# improve presetation
#
# Revision 1.7  2007/03/27 14:53:51  scetre
# Updated documentation
#
# Revision 1.6  2007/03/26 11:53:03  scetre
# Moved documentation blok in the first xml tag
#
# Revision 1.5  2007/03/26 11:34:23  scetre
# Added header documentation
#
#
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
    <xsl:output method="xml" 
        indent="yes"
        doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
        encoding="ISO-8859-1"
        doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

    <xsl:include href="sclcatObjectsCommon.xsl"/>

    <xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
    <xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>

    <!-- acces filtered calibrators -->
    <xsl:variable name="calibrators" select="document($calibratorsFilename)"/>
    <xsl:variable name="primaStars" select="/"/>

    <xsl:template match="/">
        <xsl:comment> Generated with sclcatObjectsToHtml.xsl</xsl:comment>      
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
		<table>
			<tr>
				<td>	
                <h4> Statistics </h4>
                <ul>
                    <li><xsl:value-of select="count(//star)"/> analysed stars</li>
                    <li><xsl:value-of select="count($calibrators//star[./calibrator])"/> stars with one or
                    more than one calibrator</li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=1])"/> stars with 1 calibrator </li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=2])"/> stars with 2 calibrators </li>
                    <li><xsl:value-of select="count($calibrators//star[count(./calibrator)>2])"/> stars with more than 2 calibrators </li>
                    <li><xsl:value-of select="count($calibrators//calibrator)"/> calibrators</li> 
                    <li>average <xsl:value-of select="count($calibrators//calibrator[./calibInfo/accepted]) div count(//star)"/> accepted calibrators per star</li>
                </ul>
	</td>
	<td>
                <h4> Legend </h4>
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
    </td>
    </tr>
    </table>
                <!-- Print table -->
                <table>
                    <tr> 
                        <xsl:for-each select="exslt:node-set($columns)/*">
                            <th>
                                <xsl:value-of select="@desc"/><br/>
                                <em><xsl:value-of select="@unit"/></em><br/>
                                <xsl:value-of select="@name"/>
                            </th>
                        </xsl:for-each>
                    </tr>
                    <xsl:for-each select="$calibrators//star[./calibrator]">
                        <xsl:sort select="./calibrator[1]/dist" data-type="number" />
                        <xsl:variable name="simbadName" select="./@simbadName"/>
                        <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
                        <xsl:call-template name="starInTable">
                            <xsl:with-param name="object" select="$object"/>
                        </xsl:call-template>    
                    </xsl:for-each>

                    <xsl:for-each select="$calibrators//star[not(./calibrator)]">
                        <xsl:variable name="simbadName" select="./@simbadName"/>
                        <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
                        <xsl:call-template name="starInTable">
                            <xsl:with-param name="object" select="$object"/>
                        </xsl:call-template>    
                    </xsl:for-each>

                    <xsl:for-each select="//star">
                        <xsl:variable name="simbadName" select="./simbadName"/>
                        <xsl:if test="not($calibrators//star[@simbadName=$simbadName])">
                            <xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
                            <xsl:if test="$object">
                                <xsl:call-template name="starInTable">
                                    <xsl:with-param name="object" select="$object"/>
                                </xsl:call-template>    
                            </xsl:if>
                            <xsl:if test="not($object)">
                                <xsl:call-template name="starInTable">
                                    <xsl:with-param name="object" select="."/>
                                </xsl:call-template>    
                            </xsl:if>
                        </xsl:if>
                    </xsl:for-each>
                </table>

                <hr/>
                Generated on <xsl:value-of select="date:date-time()"/><br/>
            </body>
        </html>
    </xsl:template>

    <xsl:template name="starInTable">
        <xsl:param name="object"/>
        <xsl:variable name="simbadName" select="$object/name"/>
        <xsl:variable name="primaStar" select="$primaStars//star[./simbadName=$simbadName]"/>
        <xsl:variable name="votHtmlFileName" select="concat($simbadName,'.vot.html')"/>
        <xsl:variable name="votFileName" select="concat($simbadName,'.vot')"/>
        
        <!-- first tr contains object info and next ones calibrators -->
        <xsl:element name="tr">
            <xsl:for-each select="exslt:node-set($columns)/*">
                <xsl:variable name="selector" select="@name"/>
                <xsl:element name="td">
                    <xsl:choose>
                        <xsl:when test="$selector='orbit'">
                            <!-- info have benn computed and stored into
                            calibrators file -->
                            <xsl:value-of select="$calibrators//star[./@simbadName=$simbadName]/orbit"/>
                        </xsl:when>
                        <xsl:when test="$selector='alias'">
                            <xsl:if test="$object/alias">X</xsl:if>
                        </xsl:when>
                        <xsl:when test="$selector='candidates'">
                            <xsl:value-of select="count($primaStar/planet)"/>
                        </xsl:when>
                        <xsl:when test="$selector='name'">
                            <xsl:if test="not($object/pmra and $object/pmdec)">
                                <xsl:attribute name="style">background-color: #ff6633</xsl:attribute>
                            </xsl:if>
                            <xsl:if test="not($object/ra and $object/dec)">
                                <xsl:attribute name="style">background-color: #ff0000</xsl:attribute>
                            </xsl:if>
                            <xsl:value-of select="$primaStar/name"/>
                            <xsl:comment>
                            <xsl:copy-of select="$object"/>
                        </xsl:comment>
                            [
                            <xsl:call-template name="objectToSimbadLink">
                                <xsl:with-param name="ident" select="$simbadName"/>
                                <xsl:with-param name="content" select="'S'"/>
                            </xsl:call-template>
                            <xsl:value-of select="' '"/>
                            <xsl:call-template name="objectToExoplanetLink">
                                <xsl:with-param name="ident" select="$primaStar/name"/>
                                <!-- exoplanet name is to short ??
                                <xsl:with-param name="ident" select="$primaStar/exoplanetName"/>-->
                                <xsl:with-param name="content" select="'E'"/>
                            </xsl:call-template>
                            ]
                        </xsl:when>

                        <xsl:when test="$selector='calibrators'">
                            <!-- because vo table filename contains spaces we
                            can`t open them . hack just test if object is present
                            in calibrators list-->
                            <xsl:if
                                test="$calibrators//star[./@simbadName=$simbadName]">
                                <xsl:value-of select="' '"/>
                                <xsl:value-of
					select="count($calibrators//star[./@simbadName=$simbadName]/calibrator[./calibInfo/accepted])"/>
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
        <!-- place one tr per accepted calibrator -->
        <xsl:for-each select="$calibrators//star[./@simbadName=$simbadName]/calibrator[./calibInfo/accepted]">
            <xsl:variable name="tmpCalib" select="."/>
            <xsl:element name="tr">
                <xsl:attribute name="style">background-color: #CCCCCC</xsl:attribute>
                <xsl:for-each select="exslt:node-set($columns)/*">
                    <xsl:variable name="selector" select="@name"/>
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
                                <xsl:if
                                    test="number($tmpCalib/calibInfo/minDist/value)">
                                    <xsl:value-of select="$tmpCalib/calibInfo/minDist/value"/>
                                    <xsl:value-of select="' in '"/>
                                    <xsl:value-of select="$tmpCalib/calibInfo/minDist/year"/>
                                </xsl:if>
                            </xsl:when>
                            <xsl:when test="$selector='alias'">
                            </xsl:when>
                            <xsl:when test="$selector='candidates'">
                            </xsl:when>
                            <xsl:when test="$selector='name'">
                            </xsl:when>
                            <xsl:when test="$selector='calibrators'">
                                    [<xsl:value-of  select="$tmpCalib/@index"/>]
                                    <xsl:value-of  select="' '"/>
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


    <xsl:template name="objectTo2MASSLink">
        <xsl:param name="ident" select="name"/>
        <xsl:param name="content" select="$ident"/>
        <xsl:element name="a">
            <xsl:attribute name="href">
                <xsl:value-of select="concat('http://vizier.u-strasbg.fr/viz-bin/VizieR-4?-source=II/246/out&amp;-out=2MASS&amp;2MASS=',$ident,'&amp;-out=Hmag&amp;-out=e_Hmag&amp;-out=Kmag&amp;-out=e_Kmag&amp;-out=Qflg&amp;-out=Rflg&amp;-out=Bflg&amp;-out=Cflg&amp;-out=Xflg&amp;-out=Aflg-meta.ucd=0')"/>
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

 
</xsl:stylesheet>

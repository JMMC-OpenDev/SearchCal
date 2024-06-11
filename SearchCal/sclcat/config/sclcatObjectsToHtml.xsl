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
  <xsl:output method="html" 
    indent="yes"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    encoding="ISO-8859-1"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

  <xsl:include href="sclcatObjectsCommon.xsl"/>
	<xsl:include href="url-encode.xsl"/>

  <xsl:param name="mainFilename">sclcatSimbadList.xml</xsl:param>
  <xsl:param name="calibratorsFilename">calibrators.xml</xsl:param>
  <xsl:param name="microlensingFilename">sclcatMicrolensingStars.xml</xsl:param>

  <!-- acces filtered calibrators -->
  <xsl:variable name="calibrators" select="document($calibratorsFilename)"/>
  <xsl:variable name="microlensings" select="document($microlensingFilename)"/>
  <xsl:variable name="primaStars" select="/"/>

  <xsl:template match="/">
    <xsl:comment> Generated with sclcatObjectsToHtml.xsl</xsl:comment>      
    <!--XHTML document outline-->
    <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
      <head>
        <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
        <title>Astrometric calibrators for exoplanet hosting stars</title>
        <link rel="stylesheet" href="/css/2col_leftNav.css" type="text/css" />
        <style type="text/css">
          <xsl:comment>
            .rejectedFlag {
            font-style: italic; 
            color: #FF0000;
            }
          </xsl:comment>
        </style>	
        <script type="text/javascript">
            <xsl:text>
            function toggleTRs(){
            var myStyle = (document.getElementById('rejectedFlagCheckBox').checked)? '' : 'none';
            rejectedFlags = document.getElementsByClassName('rejectedFlag');
            for ( i=0 ; rejectedFlags.length &#62; i ; i++ ) {
            rejectedFlags.item(i).style.display=myStyle;
            };
            }
            </xsl:text>
        </script>
</head>
      <body onload="toggleTRs()">
        <div id="masthead">
          <table width="800" border="0">
            <tbody>
              <tr>
                <td width="220" rowspan="1" colspan="1">
                  <div align="center">
                    <a href="/" shape="rect">
                      <img src="/images/jmmc_large.jpg" alt="JMMC logo" width="171" height="47" border="0" />
                    </a>
                  </div>
                </td>
                <td width="570" valign="middle" rowspan="1" colspan="1">
                  <div align="left">
                    <img src="/images/jmmc_logotext.gif" alt="Jean-Marie Mariotti Center" width="430" height="57" />
                  </div>
                </td>
              </tr>
            </tbody>
          </table>
          <div id="globalNav">
          </div>
        </div>

        <div id="vertical_content">
          <h1> Astrometric calibrators for exoplanet hosting stars</h1>
          <p>The following list was build using <a href="http://exoplanet.eu"> Exoplanet</a> potential list. <a href="#microlensing">Microlensing</a> detections have not been analysed.
            Each data are then collected using <a href="http://simbad.u-strasbg.fr"> Simbad web service </a>. Sometimes some <a href="#aliases">aliases</a> have been used to query Simbad.<br/>
            Get <a href="http://www.jmmc.fr/calex">more details</a> on this catalog.
          </p>
          <h1> Source List </h1>
          <div class="coloredtable">
            <table>
              <tbody>
                <tr>
                  <td>
                    <h4> Legend </h4>
                    <ul>
                      <li> E: exoplanet link</li>
                      <li> S: simbad link</li>
                      <li> DD: Diagram of calibrator's displacements</li>
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
                  <td>	
													<h4> Statistics on <xsl:value-of select="substring(date:date(),0,11)"/><br/>
													</h4>
                    <ul>
                      <li><xsl:value-of select="count(//star)"/> analysed stars</li>
                      <li><xsl:value-of select="count($calibrators//calibrator)"/> calibrators (
                        <xsl:value-of select="count($calibrators//calibrator[./calibInfo/accepted])"/> accepted,  
                        <xsl:value-of select="count($calibrators//calibrator[./calibInfo/rejected[not(diamFlagNok)]])"/> flagged on distance or
                        <xsl:value-of select="count($calibrators//calibrator[calibInfo/rejected/diamFlagNok])"/> flagged on diameter quality )</li>
                      <li>Considering accepted only calibrators:</li>
                      <ul>
                        <li><xsl:value-of select="count($calibrators//star[./calibrator/calibInfo/accepted])"/> stars with one or more calibrator</li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator[./calibInfo/accepted])=1])"/> stars with 1 calibrator </li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator[./calibInfo/accepted])=2])"/> stars with 2 calibrators </li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator[./calibInfo/accepted])>2])"/> stars with more than 2 calibrators </li>
                        <li>average <xsl:value-of select="count($calibrators//calibrator[./calibInfo/accepted]) div count(//star)"/> calibrators per star</li>
                      </ul>
                      <li>Considering accepted and rejected calibrators:</li>
                      <ul>
                        <li><xsl:value-of select="count($calibrators//star[./calibrator])"/> stars with one or more calibrator</li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=1])"/> stars with 1 calibrator </li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator)=2])"/> stars with 2 calibrators </li>
                        <li><xsl:value-of select="count($calibrators//star[count(./calibrator)>2])"/> stars with more than 2 calibrators </li>
                        <li>average <xsl:value-of select="count($calibrators//calibrator) div count(//star)"/> calibrators per star</li>
                      </ul>
                    </ul>
                  </td>
                </tr>
              </tbody>
            </table>
          </div>
          <!-- Print table -->
          <br/>
          <div>
            Show flagged calibrators <input id="rejectedFlagCheckBox" type="checkbox" onchange="toggleTRs()" />
          </div>
          <br/>
					<div class="coloredtable">
									<table>
													<tbody>
																	<tr> 
																					<xsl:for-each select="exslt:node-set($columns)/*">
																									<th>
																													<xsl:value-of select="@desc"/><br/>
																													<em><xsl:value-of select="@unit"/></em><br/>
																													<xsl:value-of select="@name"/>
																									</th>
																					</xsl:for-each>
																	</tr>
																	<!--<xsl:for-each select="$calibrators//star[./calibrator]">-->
																					<xsl:for-each select="$calibrators//star">
																									<!--<xsl:sort select="./calibrator[1]/dist" data-type="number" />-->
																									<xsl:sort select="translate(./ra, ' ','')" data-type="number" />
																									<xsl:variable name="simbadName" select="./@simbadName"/>
																									<xsl:variable name="object" select="document($mainFilename)//object[name=$simbadName]"/>
																									<xsl:call-template name="starInTable">
																													<xsl:with-param name="object" select="$object"/>
																									</xsl:call-template>    
																					</xsl:for-each>
													</tbody>
									</table>
					</div>
					<h3> Stars without calibrator:</h3>
					<div class="coloredtable">
									<table>
													<tbody>
																	<tr> 
																					<xsl:for-each select="exslt:node-set($columns)/*">
																									<th>
																													<xsl:value-of select="@desc"/><br/>
																													<em><xsl:value-of select="@unit"/></em><br/>
																													<xsl:value-of select="@name"/>
																									</th>
																					</xsl:for-each>
																	</tr>
																	<xsl:for-each select="//star">
																					<xsl:sort select="translate(./ra, ' ','')" data-type="number" />
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
													</tbody>
									</table>
					</div>
					<hr/>
          <a name="aliases"/>
          <h1> Aliases </h1>
          To achieve data collection some aliases have been defined (most of these are
          used by Exoplanet site):
          <ul>
            <li> E: exoplanet link</li>
            <li> S: simbad link</li>
          </ul>
          <xsl:call-template name="listAliases"/>

          <h1> Rejected stars </h1>
          <h2><a name="microlensing"/>Microlensing </h2>
          <p>
              <xsl:call-template name="listMicrolensing"/>
          </p>

          <hr/>
          Generated on <xsl:value-of select="date:date-time()"/><br/>
        </div>
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
              <xsl:variable name="diagdepFilename">
							<xsl:value-of select="concat('diagdep/',translate($simbadName,' ',''),'.svg')"/>
							</xsl:variable>
              <xsl:variable name="diagdepFilename2">
							<xsl:value-of select="'diagdep/'"/>
							<xsl:for-each select="str:split($simbadName)">
							<xsl:value-of select="."/>
							<xsl:if test="position()!=last()">
							<xsl:value-of select="'%20'"/>
							</xsl:if>
							</xsl:for-each>
							<xsl:value-of select="'.svg'"/>
							</xsl:variable>
              <xsl:if test="document($diagdepFilename)">
                  <a href="{$diagdepFilename}" target="_new">DD</a>
                  <xsl:message><xsl:value-of select="$diagdepFilename"/></xsl:message>
                  </xsl:if>
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
    <!-- place one tr per calibrator -->
    <xsl:for-each select="$calibrators//star[./@simbadName=$simbadName]/calibrator">
      <xsl:variable name="tmpCalib" select="."/>
      <xsl:element name="tr">
        <xsl:if test="not($tmpCalib/calibInfo/accepted)">
          <xsl:attribute name="class">rejectedFlag</xsl:attribute>
        </xsl:if>
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
    <div class="coloredtable">
      <table>
        <tbody>
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
        </tbody>
      </table>
    </div>

  </xsl:template>

  <xsl:template name="listMicrolensing">
    <div class="coloredtable">
      <table>
        <tbody>
            <xsl:for-each select="$microlensings//vot:TD">
            <tr>
              <td>
                <xsl:value-of select="position()"/>
              </td>
              <td>
                <xsl:value-of select="."/>
              </td>
            </tr>
          </xsl:for-each>
        </tbody>
      </table>
    </div>

  </xsl:template>



  <xsl:template name="objectTo2MASSLink">
    <xsl:param name="ident" select="name"/>
    <xsl:param name="content" select="$ident"/>
		<xsl:variable name="encodedIdent"><xsl:call-template name="url-encode"> <xsl:with-param name="str" select="$ident"/> </xsl:call-template></xsl:variable>
    <xsl:element name="a">
      <xsl:attribute name="href">
        <xsl:value-of select="concat('http://vizier.u-strasbg.fr/viz-bin/VizieR-4?-source=II/246/out&amp;-out=2MASS&amp;2MASS=',$encodedIdent,'&amp;-out=Hmag&amp;-out=e_Hmag&amp;-out=Kmag&amp;-out=e_Kmag&amp;-out=Qflg&amp;-out=Rflg&amp;-out=Bflg&amp;-out=Cflg&amp;-out=Xflg&amp;-out=Aflg-meta.ucd=0')"/>
      </xsl:attribute>
      <xsl:value-of select="$content"/>
    </xsl:element>                       
  </xsl:template>


  <xsl:template name="objectToSimbadLink">
    <xsl:param name="ident" select="name"/>
    <xsl:param name="content" select="$ident"/>
		<xsl:variable name="encodedIdent"><xsl:call-template name="url-encode"> <xsl:with-param name="str" select="$ident"/> </xsl:call-template></xsl:variable>
    <xsl:element name="a">
      <xsl:attribute name="href"><xsl:value-of select="concat('http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=',$encodedIdent,'&amp;NbIdent=1&amp;Radius=10&amp;Radius.unit=arcmin&amp;submit=submit+id')"/>
      </xsl:attribute>
      <xsl:value-of select="$content"/>
    </xsl:element>                       
  </xsl:template>

  <xsl:template name="objectToExoplanetLink">
    <xsl:param name="ident" select="name"/>
    <xsl:param name="content" select="$ident"/>
		<xsl:variable name="encodedIdent"><xsl:call-template name="url-encode"> <xsl:with-param name="str" select="$ident"/> </xsl:call-template></xsl:variable>
    <xsl:element name="a">
      <xsl:attribute name="href"><xsl:value-of select="concat('http://exoplanet.eu/star.php?st=',$encodedIdent)"/>
      </xsl:attribute>
      <xsl:value-of select="$content"/>
    </xsl:element>                       

  </xsl:template>


</xsl:stylesheet>

<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

    <!-- xslt that incoludes this file must also check that nfollowiong file as
    been included
    <xsl:include href="mkfSTK_WriteArrayName.xsl"/>
    -->

	<!--            Fonction WriteFunctionParam                -->
	<!-- Cette fonction sert a ecrire le parametre d'une -->
	<!-- fonction C                                      -->
	<xsl:template name="WriteFunctionParam">
		<xsl:param name="noeud"/>
		<!-- Ecriture du type du parametre-->
		<xsl:call-template name="WriteType">
			<xsl:with-param name="Type" select="$noeud/attribute[@name='type']/@value"/>
		</xsl:call-template>
		<xsl:text> </xsl:text>
		<xsl:call-template name="WritePointerType">
			<xsl:with-param name="Type" select="$noeud/attribute[@name='type']/@value"/>
		</xsl:call-template>

		<!-- Ecriture du nom du parametre -->
		<xsl:value-of select="$noeud/attribute[@name='name']/@value"/>

		<!-- Ecriture des dimensions dans le cas de tableau -->
		<xsl:if test="contains($noeud/attribute[@name='type']/@value,'(')">
			<xsl:variable name="nom">
				<xsl:call-template name="WriteArrayName">
					<xsl:with-param name="Tableau" select="$noeud/attribute[@name='type']/@value"/>
				</xsl:call-template>
			</xsl:variable>
			<xsl:variable name="tab1" select="substring-before($noeud/@value,$nom)"/>
			<xsl:variable name="tab2" select="translate($tab1,'a(','[')"/>
			<xsl:value-of select="translate($tab2,').',']')"/>
		</xsl:if>
    </xsl:template>
</xsl:stylesheet>

<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

	<!-- Ce fichier contient les fonctions suivantes : -->
	<!--   * WriteType                                -->
	<!--   * struct                                    -->
	<!--   * typedef                                   -->
	<!--   * Entete_fonction                           -->



	<!--               Fonction WriteType                         -->
	<!-- Cette fonction sert a isoler le nom exact du type         -->
	<!-- Exemple : Dans le fichier XML, un pointeur sur un entier  -->
	<!--           est stocke sous la forme p.int.                 -->
	<!-- Cette fonction remplace donc p.int par int *              -->

	<xsl:template name="WriteType">
		<xsl:param name="Type"/>

		<xsl:choose>
			<xsl:when test="contains($Type,'f(')">
				<xsl:choose>
					<xsl:when test="contains($Type,').')">						
						<xsl:variable name="inter" select="concat('f(',substring-after($Type,').'))"/>
						<xsl:call-template name="WriteType">
							<xsl:with-param name="Type" select="$inter"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="WriteType">
							<xsl:with-param name="Type" select="substring-after($Type,'f(')"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:when test="contains($Type,'...')">
				<xsl:text>...</xsl:text>                
			</xsl:when>               
			<xsl:when test="starts-with($Type,'q(')">
				<xsl:value-of select="substring-after(substring-before($Type,').'),'q(')"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="substring-after($Type,').')"/>          
			</xsl:when>
			<xsl:when test="starts-with($Type,'a(')">
				<xsl:variable name="nom_tab">
					<xsl:call-template name="WriteArrayName">
						<xsl:with-param name="Tableau" select="$Type"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:call-template name="WriteType">
					<xsl:with-param name="Type" select="$nom_tab"/>
				</xsl:call-template>
				
			</xsl:when>
			<xsl:when test="starts-with($Type,'p.')">
				<xsl:call-template name="WriteType">
					<xsl:with-param name="Type" select="substring-after($Type,'p.')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$Type"/>
			</xsl:otherwise>
		</xsl:choose>               
	</xsl:template>
</xsl:stylesheet>


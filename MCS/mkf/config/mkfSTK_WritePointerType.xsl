<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

	<!-- Ce fichier contient les fonctions suivantes : -->
	<!--   * WriteType                                -->
	<!--   * struct                                    -->
	<!--   * typedef                                   -->
	<!--   * Entete_fonction                           -->

	<!--                 Fonction pointeur                          -->
	<!-- Cette fonction sert a gerer les fonctions et les variables -->
	<!-- de type pointeur. Elle utilise la fonction recursive ptr   -->
	
	<xsl:template name="WritePointerType">
		<xsl:param name="Type"/>
		<xsl:if test="contains($Type,'p.')">
			<xsl:variable name="interType">
				<xsl:choose>
					<xsl:when test="contains($Type,'f(')">
						<xsl:value-of select="substring-before($Type,'(')"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$Type"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:call-template name="ptr">
				<xsl:with-param name="point" select="$interType"/>
			</xsl:call-template>
		</xsl:if>

	</xsl:template> 

	<!--                  Fonction ptr                              -->
	<!-- Cette fonction recursive ecrit autant d'etoiles que necessaires -->
	<!-- La recursivite permet de gerer des pointeurs de pointeurs       -->
	
	<xsl:template name="ptr">
		<xsl:param name="point"/>
		<xsl:if test="contains($point,'p.')">
			<xsl:text>*</xsl:text>
			<xsl:call-template name="ptr">
				<xsl:with-param name="point" select="substring-after($point,'p.')"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>

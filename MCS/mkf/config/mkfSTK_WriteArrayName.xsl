<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    
    <!--               Fonction WriteArrayName                                       -->
	<!-- Cette fonction permet d'isoler le nom d'une variable de type tableau -->
	<!-- Elle trouve son utilite dans le cas de paramatre du type suivant     -->
	<!-- char tableau[2][2], qui est stocke dans le fichier XML sous la forme -->
	<!-- a(2).a(2).char. Pour gerer les tableau a plusieurs dimension,        -->
	<!-- elle est recursive                                                   -->
	<xsl:template name="WriteArrayName">
		<xsl:param name="Tableau"/>
		<xsl:choose>
			<xsl:when test="contains($Tableau,'a(')">
				<xsl:variable name="var" select="substring-after($Tableau,').')"/>
				<xsl:call-template name="WriteArrayName">
					<xsl:with-param name="Tableau" select="$var"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$Tableau"/>
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>
</xsl:stylesheet> 

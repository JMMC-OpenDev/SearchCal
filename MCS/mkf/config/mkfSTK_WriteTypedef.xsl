<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<!--              Fonction typedef                               -->
	<!-- Cette fonction sert a ecrire la definition des typedef dans -->
	<!-- le fichier interface.                                       -->
	<!-- Elle support pour l'instant : enum, tableau 1D, 2D, ND      -->
	<!-- Elle ne traite pas les stuctures, qui sont gere plus haut   -->

	<xsl:template name="WriteTypedef">
		<xsl:param name="noeud"/>
		<xsl:variable name="type" select="$noeud/../attribute[@name='name']/@value"/>

		<!-- On ecarte le cas des structures, que l'on traite a part -->
		<xsl:if test="not(contains($noeud/../attribute[@name='type']/@value,'struct'))">
			<xsl:text>typedef </xsl:text>
			<xsl:choose>

				<!-- Premier cas : c'est un type enumerate -->
				<xsl:when test="contains($noeud/../attribute[@name='type']/@value,'enum')">
					<xsl:choose>
						<xsl:when test="contains($noeud/../attribute[@name='type']/@value,'$unname')">
							<xsl:text>enum </xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="$noeud/../attribute[@name='type']/@value"/>
						</xsl:otherwise>
					</xsl:choose>
					<xsl:text> { </xsl:text>

					<xsl:for-each select="$noeud/../../../enum/attributelist/attribute[@name='name']">
						<xsl:if test="@value=$type">
							<xsl:for-each select="../../enumitem">
								<xsl:for-each select="attributelist/attribute[@name='name']" >
									<xsl:value-of select="@value"/>
									<xsl:if test="not(contains(../attribute[@name='enumvalue']/@value,'+1'))">
										<xsl:text> = </xsl:text>
										<xsl:value-of select="../attribute[@name='enumvalue']/@value"/>
									</xsl:if>								
								</xsl:for-each>  
								<xsl:if test="not(position()=last())">
									<xsl:text>,</xsl:text>
								</xsl:if>          

							</xsl:for-each>
							<xsl:text>} </xsl:text>
							<xsl:value-of select="$type"/>
							<xsl:text>;&#xA;</xsl:text>							
						</xsl:if> 
					</xsl:for-each>
				</xsl:when>

				<!-- Deuxieme cas : c'est un tableau -->
				<xsl:when test="contains($noeud/../attribute[@name='decl']/@value,'a(')">
					<xsl:value-of select="$noeud/../attribute[@name='type']/@value"/>
					<xsl:text> </xsl:text>
					<xsl:value-of select="$noeud/../attribute[@name='sym_name']/@value"/>
					<xsl:variable name="inter" select="translate($noeud/../attribute[@name='decl']/@value,'a(','[')"/>
					<xsl:value-of select="translate($inter,').',']')"/>
					<xsl:text>;&#xA;</xsl:text>
				</xsl:when>

				<!-- Sinon, c'est un renommage de type simple -->
				<xsl:otherwise>
					<xsl:value-of select="$noeud/../attribute[@name='type']/@value"/>
					<xsl:text> </xsl:text>
					<xsl:value-of select="$noeud/../attribute[@name='sym_name']/@value"/>
					<xsl:text>;&#xA;</xsl:text>
				</xsl:otherwise>

			</xsl:choose>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>

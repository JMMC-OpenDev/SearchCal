<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    
    <!--                      Fonction struct                                 -->
	<!-- Cette fonction sert a gerer les typedef structures.                  -->
	<!-- Elle recopie chaque champ de la structure, et genere deux fonctions  -->
	<!-- pour en faciliter l'utilisation : structure_lire et structure_print  -->
	
	<xsl:template name="WriteStruct">
		<xsl:param name="noeud"/>
		<xsl:variable name="type" select="$noeud/../attribute[@name='sym_name']/@value"/>
		<xsl:value-of select="$noeud/../attribute[@name='storage']/@value"/>
		<xsl:text> </xsl:text>
		<xsl:choose>
			<xsl:when test="$noeud/../../../cdecl//attributelist/attribute[@name='name']/@value=$type">
				<xsl:text>typedef </xsl:text>
				<xsl:value-of select="../attribute[@name='type']/@value"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>struct </xsl:text>
				<xsl:if test="not($noeud/../attribute[@name='storage']/@value='typedef')">
					<xsl:value-of select="$type"/>
				</xsl:if>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:text>{&#xA;</xsl:text>
		<xsl:for-each select="$noeud/../../cdecl/attributelist">
			<xsl:for-each select="attribute[@name='type']" >
				<xsl:call-template name="WriteType">
					<xsl:with-param name="Type" select="@value"/>
				</xsl:call-template>
				<xsl:text> </xsl:text>
				<xsl:call-template name="WritePointerType">
					<xsl:with-param name="Type" select="../attribute[@name='decl']/@value"/>
				</xsl:call-template>
			</xsl:for-each>

			<xsl:value-of select="attribute[@name='name']/@value"/>

			<xsl:if test="contains(attribute[@name='decl']/@value,'a(')">
				<xsl:variable name="nom">
					<xsl:call-template name="WriteArrayName">
						<xsl:with-param name="Tableau" select="attribute[@name='decl']/@value"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:variable name="tab1">
					<xsl:choose>
						<xsl:when test="string-length($nom)=0">
							<xsl:value-of select="attribute[@name='decl']/@value"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="substring-before(attribute[@name='decl']/@value,$nom)"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:variable name="tab2" select="translate($tab1,'a(','[')"/>
				<xsl:value-of select="translate($tab2,').',']')"/>
			</xsl:if>

			<xsl:text>;&#xA;</xsl:text>                                
		</xsl:for-each>
		<xsl:text>} </xsl:text>
		<xsl:if test="$noeud/../attribute[@name='storage']/@value='typedef' or $noeud/../../../cdecl//attributelist/attribute[@name='name']/@value=$type">	
			<xsl:value-of select="$type"/>
		</xsl:if>
		<xsl:text>;&#xA;</xsl:text>
	
	</xsl:template>	
</xsl:stylesheet>


<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

    <xsl:include href="mkfSTK_WriteType.xsl"/>

	<!-- Fonction XSL ecrivant un script Python   -->
	<!-- facilitant le test de fonction C ou C++ -->

	<xsl:template name="saisie_param">
		<xsl:param name="nom"/>
		<xsl:param name="type"/>
		<xsl:param name="noeud"/>
		<xsl:param name="module"/>
		<xsl:variable name="Type">
			<xsl:call-template name="WriteType">
				<xsl:with-param name="Type" select="$type"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="contains($Type,'unsigned')">
				<xsl:value-of select="$nom"/>
				<xsl:text> = 0&#xA;</xsl:text>
			</xsl:when>
			<xsl:when test="$Type='int' or $Type='long'">
				<xsl:value-of select="$nom"/>
				<xsl:text> = 0&#xA;</xsl:text>
			</xsl:when>
			<xsl:when test="$Type='double' or $Type='float'">
				<xsl:value-of select="$nom"/>
				<xsl:text> = 0.0&#xA;</xsl:text>
			</xsl:when>
			<xsl:when test="$Type='string' or $Type='char'">
				<xsl:value-of select="$nom"/>
				<xsl:text> = " "&#xA;</xsl:text>
			</xsl:when>
			<xsl:when test="$Type='void'">
				<!-- Pas de parametre a saisir -->
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$nom"/>
				<xsl:text> = </xsl:text>
				<xsl:value-of select="$module"/>
				<xsl:text>.</xsl:text>
				<xsl:value-of select="$Type"/>
                <xsl:text>_lire(**</xsl:text><xsl:value-of select="$type"/><xsl:text>**)&#xA;</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="retour_fct">
		<xsl:param name="type"/>
		<xsl:param name="nom"/>
		<xsl:param name="module"/>
		<xsl:variable name="Type">
			<xsl:call-template name="WriteType">
				<xsl:with-param name="Type" select="$type"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$Type='int' or $Type='long' or $Type='double' or $Type='float' or $Type='string' or $Type='char' or contains($Type,'enum') or contains($Type,'unsigned')">
				<xsl:value-of select="$nom"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$module"/>
				<xsl:text>.</xsl:text>
				<xsl:choose>
					<xsl:when test="contains($Type,'struct')">
						<xsl:value-of select="substring-after($Type,'struct ')"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$Type"/>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:text>_print(res)&#xA;</xsl:text>
			</xsl:otherwise>
		</xsl:choose>

		
	</xsl:template>

	<xsl:template match = "/" >                      

		<!-- Importation du module a tester -->
		<xsl:variable name="nom_module">
			<xsl:value-of select="top/attributelist/attribute[@name='module']/@value"/>
		</xsl:variable>
		<xsl:text>import </xsl:text>
		<xsl:value-of select="$nom_module"/>
		<xsl:text>&#xA;&#xA;</xsl:text>

		<!-- Ecriture des appels des fonctions -->

		<xsl:for-each select ="top/include/cdecl/attributelist/attribute[@name ='sym_name']" >  
			<xsl:if test="contains(../attribute[@name='decl']/@value,'f(')">

				<xsl:text># Appel de la fonction </xsl:text>
				<xsl:value-of select="@value"/>
				<xsl:text>&#xA;</xsl:text>

				<!-- Saisie des parametres -->
				
				<xsl:for-each select= "../parmlist/parm/attributelist" >
					<xsl:call-template name="saisie_param">
						<xsl:with-param name="nom"    select="attribute[@name='name']/@value"/>
						<xsl:with-param name="type"   select="attribute[@name='type']/@value"/>
						<xsl:with-param name="noeud"  select="current()"/>
						<xsl:with-param name="module" select="$nom_module"/>
					</xsl:call-template>
				</xsl:for-each>

				<!-- Appel de la fonction -->
				
				<xsl:if test="not(contains(../attribute[@name='type']/@value,'void'))">
					<xsl:text>res = </xsl:text>
				</xsl:if>
				<xsl:value-of select="$nom_module"/>
				<xsl:text>.</xsl:text>
				<xsl:value-of select="@value"/>
				<xsl:text>(</xsl:text>

				<xsl:for-each select= "../parmlist/parm/attributelist" >
					<xsl:value-of select="attribute[@name='name']/@value"/>
					<xsl:if test="not(position()=last())">
						<xsl:text> ,</xsl:text>
					</xsl:if>
				</xsl:for-each>
				<xsl:text>)&#xA;</xsl:text>

				<!-- Affichage de la sortie --> 
				
				<xsl:if test="not(contains(../attribute[@name='type']/@value,'void'))">
					<xsl:text>print "Resultat de </xsl:text>
					<xsl:value-of select="@value"/>
					<xsl:text>"&#xA;</xsl:text>
					<xsl:text>print </xsl:text>
					<xsl:call-template name="retour_fct">
						<xsl:with-param name="type" select="../attribute[@name='type']/@value"/>
						<xsl:with-param name="nom">res</xsl:with-param>
						<xsl:with-param name="module" select="$nom_module"/>
					</xsl:call-template>
					<xsl:text>&#xA;</xsl:text>
				</xsl:if>
			</xsl:if>

		</xsl:for-each>		
		<xsl:text>&#xA;</xsl:text>

	</xsl:template>
</xsl:stylesheet>


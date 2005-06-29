<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

    <xsl:include href="mkfSTK_WriteType.xsl"/>
    <xsl:include href="mkfSTK_WritePointerType.xsl"/>
    <xsl:include href="mkfSTK_WriteFunctionParam.xsl"/>
    
    <!-- TODO : factoriser les differents templates de ce fichier -->
    
    <!--            Fonction Entete_fonction            -->
	<!-- Cette fonction ecrit l'entete d'une fonction C -->
	<xsl:template name="WriteFunctionPrototype">
		<xsl:param name="Noeud"/>
		<xsl:if test="contains(../attribute[@name='decl']/@value,'f(')">

			<!-- Ecriture du type de retour -->

			<xsl:call-template name="WriteType">
				<xsl:with-param name="Type" select="../attribute[@name='type']/@value"/>
			</xsl:call-template>
			<xsl:text> </xsl:text>


			<!-- Ecriture du nom de la fonction -->

			<xsl:text> </xsl:text>
			<xsl:if test="contains(../attribute[@name='decl']/@value,'p.')">
				<xsl:call-template name="WritePointerType">
					<xsl:with-param name="decl" select="../attribute[@name='decl']/@value"/>
				</xsl:call-template>
			</xsl:if>
			<xsl:value-of select="@value"/>
			<xsl:text>(</xsl:text>

			<!-- Ecriture des parametres de la fonction -->

			<xsl:for-each select= "../parmlist/parm/attributelist" >

				<xsl:call-template name="WriteFunctionParam">
					<xsl:with-param name="noeud" select="current()"/>
				</xsl:call-template>
				<xsl:if test="not(position()=last())">
					<xsl:text> ,</xsl:text>
				</xsl:if>

			</xsl:for-each>
			<xsl:text>);</xsl:text>
		</xsl:if>
    </xsl:template>
    
    <!-- Cette fonction ecrit l'entete d'une fonction C -->
    <!-- Mais le noeud passer doit etre une cdecl -->
	<xsl:template name="WriteFunctionPrototype2">
		<xsl:param name="Noeud"/>
        <xsl:if test="contains(.//attribute[@name='decl']/@value,'f(')">

			<!-- Ecriture du type de retour -->

			<xsl:call-template name="WriteType">
                <xsl:with-param name="Type" select="./attributelist/attribute[@name='type']/@value"/>
			</xsl:call-template>
			<xsl:text> </xsl:text>


			<!-- Ecriture du nom de la fonction -->

			<xsl:text> </xsl:text>
            <xsl:if test="contains(.//attribute[@name='decl']/@value,'p.')">
				<xsl:call-template name="WritePointerType">
                    <xsl:with-param name="decl" select=".//attribute[@name='decl']/@value"/>
				</xsl:call-template>
			</xsl:if>
            <xsl:value-of select=".//attribute[@name='sym_name']/@value"/>
			<xsl:text>(</xsl:text>

			<!-- Ecriture des parametres de la fonction -->

            <xsl:for-each select= ".//parmlist/parm/attributelist" >

				<xsl:call-template name="WriteFunctionParam">
					<xsl:with-param name="noeud" select="current()"/>
				</xsl:call-template>
				<xsl:if test="not(position()=last())">
					<xsl:text> ,</xsl:text>
				</xsl:if>

			</xsl:for-each>
			<xsl:text>);</xsl:text>
		</xsl:if>
    </xsl:template>

    <!-- Cette fonction ecrit la liste des parametre d'une fonction -->
    <!-- ex: int a, intb, char *c                                   -->
    <xsl:template name="WriteParametersForPrototype">
		<xsl:param name="Noeud"/>
        <xsl:if test="contains(.//attribute[@name='decl']/@value,'f(')">
			<!-- Ecriture des parametres de la fonction -->

            <xsl:for-each select= ".//parmlist/parm/attributelist" >

				<xsl:call-template name="WriteFunctionParam">
					<xsl:with-param name="noeud" select="current()"/>
				</xsl:call-template>
				<xsl:if test="not(position()=last())">
					<xsl:text> ,</xsl:text>
				</xsl:if>

			</xsl:for-each>
		</xsl:if>
	</xsl:template>
 
    <!-- Cette fonction ecrit la liste des parametre d'une fonction -->
    <!-- ex: a, b, *c                                   -->
    <xsl:template name="WriteParametersForFunctionCall">
		<xsl:param name="Noeud"/>
        <xsl:if test="contains(.//attribute[@name='decl']/@value,'f(')">
			<!-- Ecriture des parametres de la fonction -->

            <xsl:for-each select= ".//parmlist/parm/attributelist" >
                <xsl:value-of select="./attribute[@name='name']/@value"/>
				<xsl:if test="not(position()=last())">
					<xsl:text> ,</xsl:text>
				</xsl:if>

			</xsl:for-each>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>

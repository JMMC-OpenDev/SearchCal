<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

    <xsl:include href="mkfSTK_WriteTypedef.xsl"/>
    <xsl:include href="mkfSTK_WriteStruct.xsl"/>

	<!-- Fontion XSL generant le fichier interface necessaire a SWIG   -->
	<!-- Cette fonction supporte :                                     -->
	<!--	* types simples : int, float, double, char, string         -->
	<!--	* pointeur sur int et double                               -->
	<!--    * typedef enum                                             -->
	<!--	* typedef struct contenant des types simples ou pointeurs  -->
	<!--    * tableaux 1D,2D ou nD                                     -->

	<xsl:template match = "/" >                      

		<!-- Ecriture du nom du module, ainsi que du ou des fichiers a inclure -->

		<xsl:text>&#xA;%module </xsl:text>
		<xsl:value-of select="top/attributelist/attribute[@name='module']/@value"/>
		<xsl:text>&#xA;%{&#xA;</xsl:text>
		<xsl:for-each select="top/attributelist/attribute[@name='infile']">
			<xsl:text>#include "</xsl:text>
			<xsl:value-of select="@value"/>
			<xsl:text>"&#xA;</xsl:text>
		</xsl:for-each>
		<xsl:text>%}&#xA;&#xA;</xsl:text>

		<xsl:for-each select="top/attributelist/attribute[@name='infile']">
			<xsl:text>#include "</xsl:text>
			<xsl:value-of select="@value"/>
			<xsl:text>"&#xA;</xsl:text>
		</xsl:for-each>

		<!-- Ces lignes permettent l'utilisation de pointeurs sur des types standarts  -->
		<xsl:text>%include cpointer.i&#xA;</xsl:text> 
		<xsl:text>%pointer_functions(int,intp);&#xA;</xsl:text> 
		<xsl:text>%pointer_functions(double,doublep);&#xA;</xsl:text>
		<xsl:text>%pointer_functions(short,shortp);&#xA;</xsl:text>
		<xsl:text>%pointer_functions(long,longp);&#xA;</xsl:text>
		<xsl:text>%pointer_functions(unsigned long,unsigned_longp);&#xA;</xsl:text>
		<xsl:text>%pointer_functions(unsigned short,unsigned_shortp);&#xA;</xsl:text>
		<xsl:text>%pointer_functions(float,floatp);&#xA;</xsl:text>

        <!-- Ces lignes permettent l'utilisation de pointeurs sur des typedef excepte -->
        <!-- pour les tableaux                                                        -->
        <xsl:for-each select ="top/include/include//cdecl/attributelist/attribute[@name='storage' and @value='typedef']" >
			<xsl:variable name="Var" select="../attribute[@name='name']/@value"/>
			<xsl:variable name="Decl" select="../attribute[@name='decl']/@value"/>
			<xsl:variable name="courant" select="current()"/>
			<xsl:for-each select="/top/include//attribute[@name='type' and contains(@value,$Var)]">
                <xsl:if test="position()=last()">
                    <xsl:if test="not(starts-with($Decl,'a'))">
                    
                        <xsl:text>%pointer_functions(</xsl:text><xsl:value-of select="$Var"/>,<xsl:value-of select="$Var"/><xsl:text>p);&#xA;</xsl:text> 
                    </xsl:if>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>

        <!-- reecriture des typedef                                                   --> 
		<xsl:for-each select ="top/include/include//cdecl/attributelist/attribute[@name='storage' and @value='typedef']" >
			<xsl:variable name="Var" select="../attribute[@name='name']/@value"/>
			<xsl:variable name="courant" select="current()"/>
			<xsl:for-each select="/top/include//attribute[@name='type' and contains(@value,$Var)]">
				<xsl:if test="position()=last()">
					<xsl:call-template name="WriteTypedef">
						<xsl:with-param name="noeud" select="$courant"/>
					</xsl:call-template>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>
		<xsl:for-each select="top/include/include//class/attributelist/attribute[@name='name']">
			<xsl:variable name="Var" select="@value"/>
			<xsl:variable name="courant" select="current()"/>
			<xsl:for-each select="/top/include//attribute[@name='type' and contains(@value,$Var)]">
				<xsl:if test="position()=last()">
					<xsl:call-template name="WriteStruct">
						<xsl:with-param name="noeud" select="$courant"/>
					</xsl:call-template>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>

	</xsl:template>
</xsl:stylesheet>


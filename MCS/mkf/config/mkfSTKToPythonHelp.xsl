<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" omit-xml-declaration="yes" indent="yes"/>

	<xsl:include href="mkfSTK_WriteFunctionPrototype.xsl"/>
    <xsl:include href="mkfSTK_WriteType.xsl"/>

	<xsl:template name="Lire_structure">           
		<xsl:param name="Type"/>
		<xsl:param name="noeud"/>

		<xsl:text>def </xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>_lire (</xsl:text>
		<xsl:for-each select="$noeud/cdecl/attributelist/attribute[@name='type']" >
			<xsl:value-of select="../attribute[@name='name']/@value"/>
			<xsl:if test="not(position()=last())">
				<xsl:text> ,</xsl:text>
			</xsl:if>                     
		</xsl:for-each>
		<xsl:text> ) :&#xA;</xsl:text>
		<xsl:text>	struct = </xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>()&#xA;	</xsl:text>		
		<xsl:for-each select="$noeud/cdecl/attributelist" >
			<xsl:text>struct.</xsl:text>
			<xsl:value-of select="attribute[@name='name']/@value"/>
			<xsl:text> = </xsl:text>
			<xsl:value-of select="attribute[@name='name']/@value"/>
			<xsl:text>&#xA;	</xsl:text>                                 
		</xsl:for-each>
		<xsl:text>return struct</xsl:text>
		<xsl:text>&#xA;</xsl:text>

	</xsl:template>

	<xsl:template name="Afficher_structure">
		<xsl:param name="Type"/>
		<xsl:param name="noeud"/>

		<xsl:text>def </xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>_print (</xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>):&#xA;</xsl:text>
		<xsl:text>	print </xsl:text>
	      
		<xsl:for-each select="$noeud/cdecl/attributelist" >
			<xsl:value-of select="$Type"/>
			<xsl:text>.</xsl:text>
			<xsl:value-of select="attribute[@name='name']/@value"/>
			<xsl:if test="not(position()=last())">
				<xsl:text>, </xsl:text>
			</xsl:if>
		</xsl:for-each>
		<xsl:text>&#xA;</xsl:text>
	</xsl:template>
	
	<xsl:template name="Ecrire_typedef">
		<xsl:param name="Type"/>
		<xsl:text>def </xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>_print(type):&#xA;	print type&#xA;</xsl:text>
	</xsl:template>

	<xsl:template name="Lire_typedef">
		<xsl:param name="Type"/>
		<xsl:text>def </xsl:text>
		<xsl:value-of select="$Type"/>
		<xsl:text>_lire(valeur):&#xA;	retour=valeur&#xA;	return retour&#xA;</xsl:text>
	</xsl:template>

	
	<xsl:template match="/">
		<xsl:for-each select ="top//cdecl/attributelist/attribute[@name='storage' and @value='typedef']" >
			<xsl:variable name="Var" select="../attribute[@name='name']/@value"/>
			<xsl:variable name="courant" select="current()"/>
			<xsl:for-each select="/top/include/cdecl//attribute[@name='type' and contains(@value,$Var)]">
				<xsl:if test="position()=last()">
					<xsl:call-template name="Lire_typedef">
						<xsl:with-param name="Type" select="$Var"/>
					</xsl:call-template>
					<xsl:call-template name="Ecrire_typedef">
						<xsl:with-param name="Type" select="$Var"/>
					</xsl:call-template>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>

		<xsl:for-each select="top//class/attributelist/attribute[@name='name']">
			<xsl:variable name="Var" select="@value"/>
			<xsl:variable name="courant" select="current()"/>
			<xsl:for-each select="/top/include//attribute[@name='type' and contains(@value,$Var)]">
				<xsl:if test="position()=last()">
					<xsl:call-template name="Lire_structure">
						<xsl:with-param name="Type" select="$Var"/>
						<xsl:with-param name="noeud" select="$courant/../.."/>
					</xsl:call-template>     
					<xsl:call-template name="Afficher_structure">
						<xsl:with-param name="Type" select="$Var"/>
						<xsl:with-param name="noeud" select="$courant/../.."/>
					</xsl:call-template>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>	
		<xsl:text>def help(nom):&#xA;</xsl:text>
		<xsl:text>	if nom == "list":&#xA;</xsl:text>
		<xsl:for-each select="top/include/cdecl/attributelist">
			<xsl:if test="contains(attribute[@name='decl']/@value,'f(')">
				<xsl:text>		print "</xsl:text>
				<xsl:value-of select="attribute[@name='name']/@value"/>
				<xsl:text>"&#xA;</xsl:text>
			</xsl:if>
		</xsl:for-each>
		<xsl:for-each select="top/include/cdecl/attributelist/attribute[@name='sym_name']">
			<xsl:if test="contains(../attribute[@name='decl']/@value,'f(')">
				<xsl:text>	elif nom == "</xsl:text>
				<xsl:value-of select="@value"/>
				<xsl:text>":&#xA;</xsl:text>
				<xsl:text>		print "prototype is"&#xA;</xsl:text>
				<xsl:text>		print "</xsl:text>
				<xsl:call-template name="WriteFunctionPrototype">
					<xsl:with-param name="Noeud" select="current()"/>
				</xsl:call-template>
				<xsl:text>"&#xA;</xsl:text>
			</xsl:if>
		</xsl:for-each>
		<xsl:text>	else:&#xA;</xsl:text>
		<xsl:text>		print "ERROR -- Options not found"&#xA;</xsl:text>


	</xsl:template>
</xsl:stylesheet>



<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>

<xsl:variable name="modName">
     <xsl:value-of select="//@moduleName"/>
</xsl:variable>

<xsl:template match="error">
<xsl:text>#define </xsl:text>
<xsl:value-of select="$modName" />
<xsl:text>ERR_</xsl:text>
<xsl:value-of select="errName" />
<xsl:text> </xsl:text>
<xsl:value-of select="@id" />

</xsl:template>


</xsl:stylesheet>

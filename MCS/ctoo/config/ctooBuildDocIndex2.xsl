<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html" indent="yes"/>
    <xsl:template match="/">
            <xsl:for-each select="/module">
            <tr>
                <td> <xsl:value-of select="package"/> </td>
                <td> <a href="./doc/{./@name}/api/html/index.html" target="module"> <xsl:value-of select="./@name"/> </a> </td>
                <td> <xsl:value-of select="./desc"/> </td>
                <td><xsl:value-of select="./resp"/> </td>
        </tr>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html" indent="yes"/>
    <xsl:template match="/">
            <xsl:for-each select="/module">
            <tr>
                <td> <xsl:value-of select="package"/> </td>
                <td> <xsl:value-of select="./@name"/> </td>
                <td> <xsl:value-of select="./desc"/> </td>
        </tr>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html"/>
    <xsl:template match="/">
        <a href="./doc/{/module/@name}/api/html/index.html" target="module" title="{/module/desc}"> <xsl:value-of select="/module/@name"/> </a>
    </xsl:template>
</xsl:stylesheet>

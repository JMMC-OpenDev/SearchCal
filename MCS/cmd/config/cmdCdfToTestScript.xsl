<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:output method="text" omit-xml-declaration="yes" indent="no"/>

    <!-- ********************************************************* -->
    <!-- This TEMPLATE is the main part                            -->
    <!-- it calls one template for each cmd nodes                  -->
    <!-- INPUT : the module name                                   -->
    <!-- OUTPUT: the generated files  usind <MNEMO>.cfg             -->
    <!-- ********************************************************* -->
    <xsl:template match="/">
 
        <xsl:for-each select="/cmd">
            <xsl:document href="{concat(./mnemonic,'.cfg')}" method="text">
            <xsl:call-template name="cmdTest">
            </xsl:call-template>
        </xsl:document> 

        </xsl:for-each>
    </xsl:template>
        
    <!-- ********************************************************* -->
    <!-- This TEMPLATE is the main part to generate the test cfg   -->
    <!-- file from a given command node.                           -->
    <!-- ********************************************************* -->
    <xsl:template name="cmdTest">
#
# This is a sample test file for the command : <xsl:value-of select="mnemonic"/>
# Please adjust default parameters and go at the bottom of this fill to list
# your sciences objects and specify some parameters if default one does not
# convince.
#

[DEFAULT]
# command : <xsl:value-of select="mnemonic"/>
# desc:  <xsl:value-of select="desc"/>
command=<xsl:value-of select="mnemonic"/>


<xsl:for-each select="./params/param">
    <xsl:choose>    
# parameter: <xsl:value-of select="./name"/>
# desc: <xsl:value-of select="./desc"/>
# type: <xsl:value-of select="./name"/>
<xsl:if test="./unit">
# unit: <xsl:value-of select="./unit"/>
</xsl:if>
<xsl:if test="./minValue">
# minValue: <xsl:value-of select="./minValue"/>
</xsl:if>
<xsl:if test="./maxValue">
# maxValue: <xsl:value-of select="./maxValue"/>
</xsl:if>
<xsl:value-of select="'&#10;'"/>
    <xsl:if test="./@optional"># is optional<xsl:value-of select="'&#10;'"/></xsl:if>
    <xsl:if test="boolean(./defaultValue)"># has a default value:<xsl:value-of select="./defaultValue"/> <xsl:value-of select="'&#10;'"/> </xsl:if>
    <xsl:if test="not(./@optional)"> <xsl:value-of select="./name"/>=<xsl:value-of select="./defaultValue"/> </xsl:if>
    <xsl:if test="./@optional">#<xsl:value-of select="./name"/>=<xsl:value-of select="./defaultValue"/> <xsl:value-of select="'&#10;'"/>
    </xsl:if>
    <xsl:value-of select="'&#10;'"/>
</xsl:for-each>

# You fullfill the tail according your request.
# There must be one section per object
# If the section name does not correspond to an objectName then the objectName
# option must be specified
# ra and dec options must also be specified.
#
[test_1_ETA_TAU]
objectName = ETA TAU
ra=03:47:29.07
dec=+24:06:18.4



</xsl:template>
    
</xsl:stylesheet>

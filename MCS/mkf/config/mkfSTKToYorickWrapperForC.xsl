<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:exslt="http://exslt.org/common"
 xmlns:math="http://exslt.org/math"
 xmlns:date="http://exslt.org/dates-and-times"
 xmlns:func="http://exslt.org/functions"
 xmlns:set="http://exslt.org/sets"
 xmlns:str="http://exslt.org/strings"
 xmlns:dyn="http://exslt.org/dynamic"
 xmlns:saxon="http://icl.com/saxon"
 xmlns:xalanredirect="org.apache.xalan.xslt.extensions.Redirect"
 xmlns:xt="http://www.jclark.com/xt"
 xmlns:libxslt="http://xmlsoft.org/XSLT/namespace"
 xmlns:test="http://xmlsoft.org/XSLT/"
 extension-element-prefixes="exslt math date func set str dyn saxon xalanredirect xt libxslt test"
 exclude-result-prefixes="math str">
<xsl:output omit-xml-declaration="yes" indent="no"/>
<xsl:param name="inputFile">-</xsl:param>
<!--
********************************************************************************
* JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
********************************************************************************
 NAME
 mkfSTKToYorickWrapperForCpp

 DESCRIPTION
 Produce the yorick .i code to be able to use it using Yorick
 The given xml file should be obtained using swig onto the right_wrap.cpp file 
 which does not include any class description but only cdecl.
 after special preprocess:
 cpp -DSWIG oidata-wrap.cpp tmpoidata-wrap.cpp
 swig -xml -c++ -module oidata tmpoidata-wrap.cpp
 $ xsltproc mkfSTKToYorickWrapperForCpp.xsl ~/sw/oidata/src/tmpoidata-wrap_wrap.xml > ~/sw/ymcs/src/oidata.i
 
 One user mapping file is used to solve type conversion: userMapping.xml
 
-->

<xsl:include href="mkfSTK_WriteFunctionPrototype.xsl"/>
<xsl:include href="mkfEXSLT_StrReplace.xsl"/>
<xsl:variable name="moduleName" select="top/attributelist/attribute[@name='module']/@value"/>


<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template match="/">
/**** MODULE   '<xsl:value-of select="$moduleName"/>' ****/
/****                 contains :                         ****/
/****    o DEFINE CONSTANTS                                ****/
/****    o ENUM CONSTANTS                                ****/
/****    o STRUCTURES                                    ****/
/****    o FUNCTIONS                                     ****/


if (!is_void(plug_in)) plug_in, "<xsl:value-of select="$moduleName"/>";
write,"<xsl:value-of select="$moduleName"/> plugin loaded";

<!-- For each Define -->
/****** DEFINE CONSTANTS (numerical ones only) ******/
<xsl:for-each select="//constant">
    <xsl:call-template name="wrapForConstantValues"/>
</xsl:for-each>


<!-- For each Enum -->
/****** ENUM CONSTANTS ******/
<xsl:for-each select="//enum">
    <xsl:call-template name="wrapForEnumValues"/>
</xsl:for-each>

<!-- For each Structures -->
/****** STRUCTURES ******/
<xsl:for-each select="//class[./attributelist/attribute[./@name='name']]">
        <xsl:call-template name="wrapForStructure"/>
</xsl:for-each>

<!-- For each functions -->
/****** FUNCTIONS ******/
<xsl:for-each select=".//cdecl[.//attribute[./@name='decl' and starts-with(./@value,'f')]]">
    <xsl:if test="not(.//attribute[@name='access']) and
        .//attribute[@name='sym_name']">
        <xsl:call-template name="wrapForClassMethod"/>
    </xsl:if>
</xsl:for-each>
</xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template name="wrapForConstantValues">
    <xsl:variable name="name" select="./attributelist/attribute[@name='name']/@value"/>
    <xsl:choose>
        <xsl:when test="./attributelist/attribute[@name='type']/@value='int'">
/* Wrapping of '<xsl:value-of select="$name"/>' define */
<xsl:value-of select="$name"/> = int (<xsl:value-of select="./attributelist/attribute[@name='value']/@value"/>);
        </xsl:when>
    </xsl:choose>
</xsl:template>
 
<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template name="wrapForEnumValues">
    <xsl:variable name="name" select="./attributelist/attribute[@name='name']/@value"/>
/* Wrapping of '<xsl:value-of select="$name"/>' enum */
<xsl:for-each select="./enumitem">
    <xsl:variable name="itemname" select="./attributelist/attribute[@name='name']/@value"/>
    <!-- get value using enum value or enumvalue ex -->
    <xsl:value-of select="$itemname"/> = int (<xsl:choose>
        <!-- transform enums into 'int' type -->
        <xsl:when test="./attributelist/attribute[@name='enumvalue']"><xsl:value-of select="./attributelist/attribute[@name='enumvalue']/@value"/></xsl:when>
        <xsl:otherwise><xsl:value-of select="./attributelist/attribute[@name='enumvalueex']/@value"/></xsl:otherwise>
    </xsl:choose>);
</xsl:for-each>
</xsl:template>
 
<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template name="wrapForStructure">
    <xsl:variable name="name" select="./attributelist/attribute[@name='name']/@value"/>
/* Wrapping of '<xsl:value-of select="$name"/>' structure */
struct <xsl:value-of select="$name"/>  
{
<xsl:for-each select=".//cdecl">
    <xsl:call-template name="WriteYorickVariableDeclaration">
        <xsl:with-param name="variable" select="."/>
    </xsl:call-template>
</xsl:for-each>};
</xsl:template>
 
<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template name="wrapForClassMethod">
    <xsl:variable name="methName" select="./attributelist/attribute[@name='name']/@value"/>
    <xsl:variable name="methNameIndex">
        <xsl:call-template name="appendMethodIndex">
            <xsl:with-param name="methodList" select="ancestor::class/cdecl[attributelist/attribute/@value=$methName and attributelist/attribute/@name='name']"/>
            <xsl:with-param name="method" select="."/>
        </xsl:call-template>
    </xsl:variable>
    <!--  Next part will ouptut the codger PROTOTYPE only if it is possible -->
    <xsl:choose>
        <!-- Reject variables arguments -->
        <xsl:when test="contains(./attributelist/attribute[@name='decl']/@value,'...')">
            <xsl:message>Warning: '<xsl:value-of select="$methName"/>' which has variable arguments is not wrapped</xsl:message>
/* '<xsl:value-of select="$methName"/>' function skipped :  
 * this function contains variable arguments 
 */
    </xsl:when>        
    <xsl:otherwise>
/* 
 * Wrapping of '<xsl:value-of select="$methName"/>' function */   
 <xsl:if test="contains(./attributelist/attribute[@name='decl']/@value,').p.')">/*
 * WARNING : this function returns one pointer
 */
 <xsl:message>Warning: '<xsl:value-of select="$methName"/>' function returns one pointer</xsl:message>    </xsl:if> 
extern __<xsl:value-of select="$methName"/>;
/* PROTOTYPE
    <xsl:choose>
        <xsl:when test="contains(./attributelist/attribute[@name='decl']/@value,').p.')">int</xsl:when>        
    <xsl:otherwise>
        <xsl:call-template name="WriteYorickType">
        <xsl:with-param name="type" select="./attributelist/attribute[@name='type']/@value"/>
        </xsl:call-template>
    </xsl:otherwise>
    </xsl:choose>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>( <xsl:if test=".//parmlist"><xsl:call-template name="WriteParametersTypeForYorickPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
*/
/* DOCUMENT  <xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>( <xsl:if test=".//parmlist"><xsl:call-template name="WriteParametersTypeForYorickPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
  * C-prototype:
    ------------
    <xsl:value-of select="./attributelist/attribute[@name='type']/@value"/> <xsl:if test="contains(./attributelist/attribute[@name='decl']/@value,').p.')"><xsl:value-of select="' *'"/></xsl:if> <xsl:value-of select="' '"/><xsl:value-of select="$methName"/><xsl:value-of select="$methNameIndex"/>  (<xsl:if test=".//parmlist"> <xsl:call-template name="WriteParametersForPrototype">
        <xsl:with-param name="Noeud" select="./attributelist/attribute[@name='name']"/>
</xsl:call-template></xsl:if>)
*/
</xsl:otherwise>
</xsl:choose>
</xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
<!-- Append on method index to identify polymorphic C++ method   -->
<!--  -->
<xsl:template name="appendMethodIndex">
    <!-- Used to place index in C for C++ polymorphic methods -->
    <xsl:param name="methodlist"/>
    <xsl:param name="method"/>
    <xsl:if test="count($methodList)>1">
        <xsl:for-each select="$methodList">
            <xsl:if test="./@id=$method/@id">_<xsl:value-of select="position()"/></xsl:if>
        </xsl:for-each>
    </xsl:if>
    
</xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
<!-- Write List of paramaeters for given C function  -->
<!--  -->
<xsl:template name="WriteParametersTypeForYorickPrototype">
    <xsl:param name="Noeud"/>
    <xsl:if test="contains(.//attribute[@name='decl']/@value,'f(')">

        <!-- Ecriture ddu type des parametres de la fonction -->

        <xsl:for-each select=".//parmlist/parm/attributelist/attribute[@name='type']" >
            <xsl:variable name="type" select="./@value"/>
            <xsl:call-template name="WriteYorickType">
                <xsl:with-param name="type" select="$type"/>
            </xsl:call-template>
            <xsl:if test="not(position()=last())">
                <xsl:text>, </xsl:text>
            </xsl:if>
        </xsl:for-each>
        <xsl:text></xsl:text>
    </xsl:if>
</xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
	<xsl:template name="WriteArrayDimension">
		<xsl:param name="Tableau"/>
		<xsl:choose>
			<xsl:when test="contains($Tableau,'a(')">
				<xsl:variable name="var" select="substring-after($Tableau,').')"/>
                				<xsl:call-template name="WriteArrayName">
					<xsl:with-param name="Tableau" select="$var"/>
                </xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$Tableau"/>
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
<!-- Write array dimensions -->
<!-- output is blank, (x) or (x,y,...,z) -->
<xsl:template name="WriteYorickVariableDimensions">
    <xsl:param name="swigDim"/>
    <xsl:if test="contains($swigDim,'a(')">
    <xsl:text>(</xsl:text>
    <xsl:variable name="a">
        <xsl:call-template name="str:replace">
        <xsl:with-param name="string" select="$swigDim" />
        <xsl:with-param name="search" select="'a('" />
        <xsl:with-param name="replace" select="','" />
    </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="b">
    <xsl:call-template name="str:replace">
        <xsl:with-param name="string" select="$a" />
        <xsl:with-param name="search" select="').'" />
        <xsl:with-param name="replace" select="''" />
    </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="c">
    <xsl:call-template name="str:replace">
        <xsl:with-param name="string" select="$b" />
        <xsl:with-param name="search" select="'p.'" />
        <xsl:with-param name="replace" select="''" />
    </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="substring($c,2)"/> 
    <xsl:text>)</xsl:text>
    </xsl:if>
</xsl:template>

<!-- TEMPLATE:                               -->
<!-- *********                               -->
<!--
 Print yorick declaration
 ************************
 output is something like 
 int a;
 char b(20);
 where 20 comes from swig decl a(80+1).
-->
<xsl:template name="WriteYorickVariableDeclaration">
    <xsl:param name="variable"/>
    <xsl:value-of select="'    '"/>
    <xsl:call-template name="WriteYorickType">
        <xsl:with-param name="type" select="./attributelist/attribute[@name='type']/@value"/>
    </xsl:call-template>
    <xsl:if test="contains(./attributelist/attribute[@name='decl']/@value,'p.')">
        <xsl:value-of select="'*'" />
        <xsl:for-each select="str:tokenize(./attributelist/attribute[@name='decl']/@value,'p.')">
            <xsl:value-of select="'*'" />
        </xsl:for-each>
    </xsl:if>
    <xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>
    <xsl:call-template name="WriteYorickVariableDimensions">
        <xsl:with-param name="swigDim" select="./attributelist/attribute[@name='decl']/@value"/>
    </xsl:call-template>;
</xsl:template>
 
<!-- TEMPLATE:                               -->
<!-- *********                               -->
<xsl:template name="WriteYorickType">
    <xsl:param name="type"/>
    <xsl:variable name="typeMod"> 
        <xsl:call-template name="str:replace">
            <xsl:with-param name="string" select="$type" />
            <xsl:with-param name="search" select="'q(const).'" />
            <xsl:with-param name="replace" select="''" />
        </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
        <!-- transform enums into 'int' type -->
        <xsl:when test="//enum//attribute[./@name='name' and ./@value=$typeMod]">int </xsl:when>
        <!-- transform into same type -->
        <xsl:when test="$typeMod='char'">char </xsl:when>
        <xsl:when test="$typeMod='int'">int </xsl:when>
        <xsl:when test="$typeMod='short'">short </xsl:when>
        <xsl:when test="$typeMod='void'">void </xsl:when>
        <xsl:when test="$typeMod='double'">double </xsl:when>
        <xsl:when test="$typeMod='float'">float </xsl:when>
        <!-- transform typedef struct into same type -->
        <xsl:when test="//class[./attributelist/attribute[./@name='name' and @value=$typeMod]]"><xsl:value-of select="$typeMod"/><xsl:value-of select="' '"/></xsl:when>
        <!-- transform unsigned xxxx into xxxx type -->
        <xsl:when test="starts-with($typeMod,'unsigned')"><xsl:value-of select="substring-after($typeMod,'unsigned')"/><xsl:value-of select="' '"/></xsl:when>

        <!-- transform array into pointer type -->
        <xsl:when test="starts-with($type,'a(')">pointer </xsl:when>
        <!-- transform arrays into pointer type -->
        <xsl:when test="starts-with($type,$moduleName)">pointer </xsl:when>
        <!-- transform char array into pointer type -->
        <xsl:when test="$typeMod='p.char'">string </xsl:when>
        <!-- transform pointer into pointer type -->
        <xsl:when test="starts-with($typeMod,'p.')">pointer </xsl:when>

        
        <!-- used to identify errors -->
        <xsl:otherwise>
            <xsl:choose>
                <!-- transform according userMapping if any -->
                <xsl:when test="document('userMapping.xml')//type[c=$typeMod]"><xsl:value-of select="document('userMapping.xml')//type[c=$typeMod]/yorick"/></xsl:when>
                <xsl:otherwise>
                    <xsl:message>Type '<xsl:value-of select="$typeMod"/>' not supported (please fill userMapping.xml file) </xsl:message> _TYPE_<xsl:value-of select="$typeMod"/>_NOT_SUPPORTED_in_mkfSTKToYorickWrapperForCpp_xsl_file </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>

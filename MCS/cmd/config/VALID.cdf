<?xml version="1.0" encoding="UTF-8"?>
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <!--
    This cdf describes a simple command (VALID) that accepts 12 parameters:
    4 of them are required 
    4 others have a default value
    4 others are optional

    For this demo file, the name of each parameter is the name of the type.
    Parameters with default value have d prefix and optional parameters have 
    a o prefix.
    -->
    <mnemonic>VALID</mnemonic>
    <desc>Simple command with 4 mandatory, 4 default and 4 optional parameters</desc>
    <params>
        <param>
            <name>integer</name>
            <desc> this parameter is mandatory and must be of integer type </desc>
            <type>integer</type>
            <unit>BogoMIPS</unit>
        </param>
        <param>
            <name>double</name>
            <type>double</type>
        </param>
        <param>
            <name>boolean</name>
            <type>boolean</type>
        </param>
        <param>
            <name>string</name>
            <type>string</type>
        </param>
        <param>
            <name>dinteger</name>
            <type>integer</type>
            <desc> this parameter is not mandatory but must be of integer type </desc>
            <defaultValue><integer>12</integer></defaultValue>
        </param>
        <param>
            <name>ddouble</name>
            <type>double</type>
            <defaultValue><double>12.5</double></defaultValue>
        </param>
        <param>
            <name>dboolean</name>
            <type>boolean</type>
            <defaultValue><boolean>true</boolean></defaultValue>
        </param>
        <param>
            <name>dstring</name>
            <type>string</type>
            <defaultValue><string>dvalue</string></defaultValue>
        </param>
        <param optional="true">
            <name>ointeger</name>
            <type>integer</type>
        </param>
        <param optional="true">
            <name>odouble</name>
            <type>double</type>
        </param>
        <param optional="true">
            <name>oboolean</name>
            <type>boolean</type>
        </param>
        <param optional="true">
            <name>ostring</name>
            <type>string</type>
        </param>
    </params>
</cmd>

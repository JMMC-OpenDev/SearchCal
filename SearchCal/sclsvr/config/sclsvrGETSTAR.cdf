<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
* JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
********************************************************************************
-->
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETSTAR</mnemonic>
    <desc>Get informations about the given star. It returns all properties found
        in CDS star catalogs, or computed by SW.</desc>
    <params>
        <param>
            <name>objectName</name>
            <type>string</type>
            <desc>star name</desc>
        </param>
        <param>
            <name>file</name>
            <type>string</type>
            <desc>file name to write votable</desc>
        </param>
        <param>
            <name>baseline</name>
            <type>double</type>
            <defaultValue><double>100.00</double></defaultValue>
            <unit>m</unit>
            <desc>baseline used to compute visibility</desc>
        </param>
        <param>
            <name>wlen</name>
            <type>double</type>
            <desc>observing wavelength used to compute visibility</desc>
            <defaultValue><double>1.00</double></defaultValue>
            <unit>us</unit>
        </param>
    </params>        
</cmd>

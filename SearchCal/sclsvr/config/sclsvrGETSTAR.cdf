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
        <param optional="true">
            <name>file</name>
            <type>string</type>
            <desc>file name to write votable</desc>
        </param>
        <param>
            <name>baseMax</name>
            <type>double</type>
            <desc>maximum baseline length used to compute visibility</desc>
            <defaultValue>
                <double>0.0</double>
            </defaultValue>
            <minValue>
                <double>0.0</double>
            </minValue>
            <unit>m</unit>
        </param>
        <param>
            <name>wlen</name>
            <type>double</type>
            <desc>observing wavelength used to compute visibility</desc>
            <defaultValue>
                <double>0.0</double>
            </defaultValue>
            <unit>um</unit>
        </param>
        <param optional="true">
            <name>format</name>
            <type>string</type>
            <desc>output format ('vot' or 'tsv')</desc>
        </param>
        <param optional="true">
            <name>diagnose</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>true</boolean>
            </defaultValue>
            <desc>specify whether the diagnostic mode is enabled (add request log in VOTABLE)</desc>
        </param>
    </params>
</cmd>

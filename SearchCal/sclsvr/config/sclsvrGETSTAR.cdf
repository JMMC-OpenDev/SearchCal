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
            <desc>star name(s) separated by '|' or ',' character</desc>
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
            <defaultValue>
                <string>vot</string>
            </defaultValue>
        </param>
        <param optional="true">
            <name>diagnose</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>true</boolean>
            </defaultValue>
            <desc>specify whether the diagnostic mode is enabled (add request log in VOTABLE)</desc>
        </param>
        <param optional="true">
            <name>forceUpdate</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>false</boolean>
            </defaultValue>
            <desc>specify whether the force (update) is enabled (force query CDS again... slow the first time)</desc>
        </param>
        <!-- user custom photometries and spectral type -->
        <param optional="true">
            <name>V</name>
            <type>double</type>
            <desc>science object magnitude in V band</desc>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>e_V</name>
            <type>double</type>
            <desc>science object magnitude error in V band</desc>
            <minValue>
                <double>0.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>J</name>
            <type>double</type>
            <desc>science object magnitude in J band</desc>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>e_J</name>
            <type>double</type>
            <desc>science object magnitude error in J band</desc>
            <minValue>
                <double>0.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>H</name>
            <type>double</type>
            <desc>science object magnitude in H band</desc>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>e_H</name>
            <type>double</type>
            <desc>science object magnitude error in H band</desc>
            <minValue>
                <double>0.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>K</name>
            <type>double</type>
            <desc>science object magnitude in K band</desc>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>e_K</name>
            <type>double</type>
            <desc>science object magnitude error in K band</desc>
            <minValue>
                <double>0.0</double>
            </minValue>
            <maxValue>
                <double>20.0</double>
            </maxValue>
        </param>
        <param optional="true">
            <name>SP_TYPE</name>
            <type>string</type>
            <desc>science object spectral type (made of a temperature class, eventually a luminosity class (roman number) and/or spectral peculiarities)</desc>
        </param>
    </params>
</cmd>

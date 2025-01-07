<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
* JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
********************************************************************************
-->
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETCAL</mnemonic>
    <desc>Get a list of interferometric calibrators for a given science object. It returns any star found in CDS catalogs around the science object, which could be used as calibrator during an observation. It computes diameters and the expected visibility for each found calibrator.</desc>
    <params>
        <param>
            <name>objectName</name>
            <type>string</type>
            <desc>science object name</desc>
        </param>
        <param>
            <name>mag</name>
            <type>double</type>
            <desc>science object magnitude</desc>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20</double>
            </maxValue>
        </param>
        <param>
            <name>diffRa</name>
            <type>double</type>
            <defaultValue>
                <double>1800</double>
            </defaultValue>
            <desc>right ascension value of the rectangular search box size</desc>
            <minValue>
                <double>0</double>
            </minValue>
            <maxValue>
                <double>5400</double>
            </maxValue>
            <unit>arcmin</unit>
        </param>
        <param>
            <name>diffDec</name>
            <type>double</type>
            <defaultValue>
                <double>600</double>
            </defaultValue>
            <desc>declination value of the rectangular search box size</desc>
            <minValue>
                <double>0</double>
            </minValue>
            <maxValue>
                <double>1800</double>
            </maxValue>
            <unit>arcmin</unit>
        </param>
        <param optional="true">
            <name>radius</name>
            <type>double</type>
            <desc>radius value of the circular search box size</desc>
            <minValue>
                <double>0</double>
            </minValue>
            <maxValue>
                <double>3600</double>
            </maxValue>
            <unit>arcmin</unit>
        </param>
        <param>
            <name>band</name>
            <type>string</type>
            <defaultValue>
                <string>K</string>
            </defaultValue>
            <desc>observation band</desc>
        </param>
        <param optional="true">
            <name>minMagRange</name>
            <type>double</type>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20</double>
            </maxValue>
            <desc>minimum of the magnitude range</desc>
        </param>
        <param optional="true">
            <name>maxMagRange</name>
            <type>double</type>
            <minValue>
                <double>-10.0</double>
            </minValue>
            <maxValue>
                <double>20</double>
            </maxValue>
            <desc>maximum of the magnitude range</desc>
        </param>
        <param>
            <name>ra</name>
            <type>string</type>
            <desc>right ascension coordinate of the science object</desc>
            <unit>HH:MM:SS.TT</unit>
        </param>
        <param>
            <name>dec</name>
            <type>string</type>
            <desc>declination coordinate of the science object</desc>
            <unit>DD:MM:SS.TT</unit>
        </param>
        <param>
            <name>baseMax</name>
            <type>double</type>
            <desc>maximum baseline length used to compute visibility</desc>
            <defaultValue>
                <double>100</double>
            </defaultValue>
            <minValue>
                <double>0.1</double>
            </minValue>
            <unit>m</unit>
        </param>
        <param>
            <name>wlen</name>
            <type>double</type>
            <desc>observing wavelength</desc>
            <minValue>
                <double>0.3</double>
            </minValue>
            <maxValue>
                <double>20</double>
            </maxValue>
            <unit>um</unit>
        </param>
        <param optional="true">
            <name>file</name>
            <type>string</type>
            <desc>name of file in which results should be saved</desc>
        </param>
        <param optional="true">
            <name>bright</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>true</boolean>
            </defaultValue>
            <desc>specify whether the query should return bright (by default) or faint stars</desc>
        </param>
        <param optional="true">
            <name>noScienceStar</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>true</boolean>
            </defaultValue>
            <desc>specify whether the science object should be removed from the list or not</desc>
        </param>
        <param optional="true">
            <name>outputFormat</name>
            <type>double</type>
            <defaultValue>
                <double>0.0</double>
            </defaultValue>
            <desc>expected output format (votable meta data)</desc>
        </param>
        <param optional="true">
            <name>diagnose</name>
            <type>boolean</type>
            <defaultValue>
                <boolean>false</boolean>
            </defaultValue>
            <desc>specify whether the diagnostic mode is enabled (do not filter on diamFlag and add request log in VOTABLE)</desc>
        </param>
    </params>
</cmd>

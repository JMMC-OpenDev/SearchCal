<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETCAL.cdf,v 1.2 2005-02-03 10:27:01 gzins Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $ 

 Definition of GETCAL command.
-->
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETCAL</mnemonic>
    <desc>options definition of the GETCAL command</desc>
    <params>
        <param>
            <name>objectName</name>
            <type>string</type>
            <desc>the science object name</desc>
        </param>
        <param>
            <name>mag</name>
            <type>double</type>
            <desc>the calibrators expected magnitude</desc>
            <unit></unit>
        </param>
        <param>
            <name>maxReturn</name>
            <type>integer</type>
            <defaultValue><integer>50</integer></defaultValue>
            <desc>the number of maximum result</desc>
        </param>
        <param>
            <name>diffRa</name>
            <type>integer</type>
            <defaultValue><integer>1800</integer></defaultValue>
            <desc>the right ascension value of the box size</desc>
            <unit></unit>                
        </param>
        <param>
            <name>diffDec</name>
            <type>integer</type>
            <defaultValue><integer>600</integer></defaultValue>
            <desc>the declinaison value of the box size</desc>
            <unit></unit>
        </param>
        <param>
            <name>band</name>
            <type>string</type>
            <defaultValue><string>K</string></defaultValue>
        </param>
        <param>
            <name>minMagRange</name>
            <type>string</type>
            <desc>the minimum value of the expected magnitude</desc>
        </param>
        <param>
            <name>maxMagRange</name>
            <type>string</type>
            <desc>the maximum value of the expected magnitude</desc>
        </param>
        <param>
            <name>ra</name>
            <type>string</type>
            <defaultValue><string>03:47:29.79</string></defaultValue>
            <desc>the right ascension coordinate of the science object</desc>
            <unit>hms</unit>
        </param>
        <param>
            <name>dec</name>
            <type>string</type>
            <defaultValue><string>+24:06:18.50</string></defaultValue>
            <desc>the declinaison coordinate of the science object</desc>
            <unit>dms</unit>                
        </param>
        <param>
            <name>baseMin</name>
            <type>double</type>
            <defaultValue><double>46.64</double></defaultValue>
        </param>
        <param>
            <name>baseMax</name>
            <type>double</type>
            <defaultValue><double>102.45</double></defaultValue>
        </param>
        <param>
            <name>lambda</name>
            <type>double</type>
            <desc>the observing wavelength</desc>
            <unit>micron</unit>
        </param>
        <param>
            <name>vis</name>
            <type>double</type>
            <defaultValue><double>1.0</double></defaultValue>
            <desc>the object expected squared visibility</desc>
        </param>
        <param>
            <name>visErr</name>
            <type>double</type>
            <defaultValue><double>0.1</double></defaultValue>
            <desc>the request absolute error on visibility</desc>
        </param>
    </params>        
</cmd>

<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETCAL.cdf,v 1.3 2005-02-07 14:41:46 gzins Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $
 Revision 1.2  2005/02/03 10:27:01  gzins
 Added file header.

 Definition of GETCAL command.
-->
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETCAL</mnemonic>
    <desc>Get a list of interferometric calibrators for a given science
        object. It returns all stars, found in CDS catalogs around the science
        object, which can be used as calibrator during the observation. It
        computes for each calibrator, its diameter and the expected visibility.  </desc>
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
            <unit></unit>
        </param>
        <param>
            <name>maxReturn</name>
            <type>integer</type>
            <defaultValue><integer>50</integer></defaultValue>
            <desc>maximum number of calibrators</desc>
        </param>
        <param>
            <name>diffRa</name>
            <type>integer</type>
            <defaultValue><integer>1800</integer></defaultValue>
            <desc>right ascension value of the search box size</desc>
            <unit>arcmin</unit>                
        </param>
        <param>
            <name>diffDec</name>
            <type>integer</type>
            <defaultValue><integer>600</integer></defaultValue>
            <desc>declinaison value of the search box size</desc>
            <unit>arcmin</unit>
        </param>
        <param>
            <name>band</name>
            <type>string</type>
            <defaultValue><string>K</string></defaultValue>
            <desc>observation band</desc>
        </param>
        <param>
            <name>minDeltaMag</name>
            <type>string</type>
            <desc>min delta magnitude; i.e. maximum difference between the
                minimum magnitude of calibrator and the science object magnitude
            </desc>
        </param>
        <param>
            <name>maxDeltaMag</name>
            <type>string</type>
            <desc>max delta magnitude; i.e. maximum difference between the
                maximum magnitude of calibrator and the science object magnitude
            </desc>
        </param>
        <param>
            <name>ra</name>
            <type>string</type>
            <defaultValue><string>03:47:29.79</string></defaultValue>
            <desc>right ascension coordinate of the science object</desc>
            <unit>HH:MM:SS.TT</unit>
        </param>
        <param>
            <name>dec</name>
            <type>string</type>
            <defaultValue><string>+24:06:18.50</string></defaultValue>
            <desc>declinaison coordinate of the science object</desc>
            <unit>DD:MM:SS.TT</unit>                
        </param>
        <param>
            <name>baseMin</name>
            <type>double</type>
            <desc>minimum baseline length</desc>
            <defaultValue><double>46.64</double></defaultValue>
            <unit>m</unit>                
        </param>
        <param>
            <name>baseMax</name>
            <type>double</type>
            <desc>maximum baseline length</desc>
            <defaultValue><double>102.45</double></defaultValue>
            <unit>m</unit>                
        </param>
        <param>
            <name>wlen</name>
            <type>double</type>
            <desc>observing wavelength</desc>
            <unit>um</unit>
        </param>
        <param>
            <name>vis</name>
            <type>double</type>
            <defaultValue><double>1.0</double></defaultValue>
            <desc>calibrator expected visibility</desc>
        </param>
        <param>
            <name>visErr</name>
            <type>double</type>
            <defaultValue><double>0.1</double></defaultValue>
            <desc>maximum error on visibility</desc>
        </param>
    </params>        
</cmd>

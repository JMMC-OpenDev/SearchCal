<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETCAL.cdf,v 1.12 2005-03-07 16:06:38 gzins Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $
 Revision 1.11  2005/03/07 13:41:04  gzins
 Removed min baseline length
 Defined magnitude range option as optional (not used in N band)

 Revision 1.10  2005/03/04 09:51:23  gzins
 Updated parameter ranges

 Revision 1.9  2005/03/04 09:17:49  scetre
 *** empty log message ***

 Revision 1.7  2005/02/28 13:49:03  scetre
 Added save file option in the request

 Revision 1.6  2005/02/14 15:10:17  scetre
 changed m..RangeMag to m..MagRange

 Revision 1.5  2005/02/13 16:07:42  gzins
 Changed Min/MaxDeltaMag to Min/MaxRangeMag

 Revision 1.4  2005/02/08 04:54:20  gzins
 Changed min/max magnitude type

 Revision 1.3  2005/02/07 14:41:46  gzins
 Updated documentation; added description and unit.
 Renamed lambda to wlen.
 Renamed minMagRange to minDeltaMag, and maxMagRange to maxDeltaMag.

 Revision 1.2  2005/02/03 10:27:01  gzins
 Added file header.

 Definition of GETCAL command.
-->
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETCAL</mnemonic>
    <desc>Get a list of interferometric calibrators for a given science object. It returns all stars, found in CDS catalogs around the science object, which can be used as calibrator during the observation. It computes for each calibrator, its diameter and the expected visibility.</desc>
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
            <minValue><double>-5.0</double></minValue>
            <maxValue><double>20</double></maxValue>
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
            <minValue><integer>0</integer></minValue>
            <maxValue><integer>3600</integer></maxValue>
            <unit>arcmin</unit>                
        </param>
        <param>
            <name>diffDec</name>
            <type>integer</type>
            <defaultValue><integer>600</integer></defaultValue>
            <desc>declinaison value of the search box size</desc>
            <minValue><integer>0</integer></minValue>
            <maxValue><integer>1800</integer></maxValue>
            <unit>arcmin</unit>
        </param>
        <param>
            <name>band</name>
            <type>string</type>
            <defaultValue><string>K</string></defaultValue>
            <desc>observation band</desc>
        </param>
        <param optional="true">
            <name>minMagRange</name>
            <type>double</type>
            <minValue><double>-5.0</double></minValue>
            <maxValue><double>20</double></maxValue>
            <desc>minimum of the range magnitude</desc>
        </param>
        <param optional="true">
            <name>maxMagRange</name>
            <type>double</type>
            <minValue><double>-5.0</double></minValue>
            <maxValue><double>20</double></maxValue>
            <desc>maximum of the range magnitude</desc>
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
            <desc>declinaison coordinate of the science object</desc>
            <unit>DD:MM:SS.TT</unit>                
        </param>
        <param>
            <name>baseMax</name>
            <type>double</type>
            <desc>maximum baseline length</desc>
            <defaultValue><double>100</double></defaultValue>
            <minValue><double>0.1</double></minValue>
            <unit>m</unit>                
        </param>
        <param>
            <name>wlen</name>
            <type>double</type>
            <desc>observing wavelength</desc>
            <minValue><double>0.5</double></minValue>
            <maxValue><double>20</double></maxValue>
            <unit>um</unit>
        </param>
        <param optional="true">
            <name>file</name>
            <type>string</type>
            <desc>name of file in which results will be saved</desc>
        </param>
    </params>        
</cmd>

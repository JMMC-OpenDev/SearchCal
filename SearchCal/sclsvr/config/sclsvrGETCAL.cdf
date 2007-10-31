<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETCAL.cdf,v 1.23 2007-10-31 11:25:30 gzins Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $
 Revision 1.22  2007/06/27 14:25:42  scetre
 Added noScienceStar parameter default is true

 Revision 1.21  2007/04/27 09:02:36  gzins
 Change diffRa, diffDec and radius parameter type; integer -> double

 Revision 1.20  2006/07/17 09:10:36  scetre
 Added old scenario option

 Revision 1.19  2006/03/07 15:33:39  scetre
 Removed old scenario in band K

 Revision 1.18  2006/03/06 17:09:47  lafrasse
 Added diamVK parameter

 Revision 1.17  2005/12/16 13:26:24  scetre
 Added test when computing visibility Added visibility error as parameter

 Revision 1.16  2005/12/12 14:08:41  scetre
 Added -oldScenario option to the GETCAL command

 Revision 1.15  2005/11/24 09:00:10  lafrasse
 Added 'radius' parameter to the GETCAL command

 Revision 1.14  2005/11/23 14:35:33  lafrasse
 Added fileName proper management (strncpy() calls instead of strcpy())
 Removed unused 'MaxReturn' command parmater
 Added 'bright' command parameter

 Revision 1.13  2005/10/05 12:35:12  scetre
 Update parameter and command description

 Revision 1.12  2005/03/07 16:06:38  gzins
 Removed vis and visErr options

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
    <desc>Get a list of interferometric calibrators for a given science object. It returns any stars found in CDS catalogs around the science object, which can be used as calibrator during an observation. It computes diameters and the expected visibility for each found calibrator.</desc>
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
            <name>diffRa</name>
            <type>double</type>
            <defaultValue><double>1800</double></defaultValue>
            <desc>right ascension value of the rectangular search box size</desc>
            <minValue><double>0</double></minValue>
            <maxValue><double>3600</double></maxValue>
            <unit>arcmin</unit>                
        </param>
        <param>
            <name>diffDec</name>
            <type>double</type>
            <defaultValue><double>600</double></defaultValue>
            <desc>declinaison value of the rectangular search box size</desc>
            <minValue><double>0</double></minValue>
            <maxValue><double>1800</double></maxValue>
            <unit>arcmin</unit>
        </param>
        <param optional="true">
            <name>radius</name>
            <type>double</type>
            <desc>radius value of the circular search box size</desc>
            <minValue><double>0</double></minValue>
            <maxValue><double>3600</double></maxValue>
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
            <desc>minimum of the magnitude range</desc>
        </param>
        <param optional="true">
            <name>maxMagRange</name>
            <type>double</type>
            <minValue><double>-5.0</double></minValue>
            <maxValue><double>20</double></maxValue>
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
            <desc>name of file in which results should be saved</desc>
        </param>
        <param optional="true">
            <name>diamVK</name>
            <type>double</type>
            <desc>VK diameter</desc>
            <minValue><double>0.0</double></minValue>
            <unit>mas</unit>
        </param>
        <param optional="true">
            <name>bright</name>
            <type>boolean</type>
            <defaultValue><boolean>true</boolean></defaultValue>
            <desc>specify whether the query should return bright (by default) or faint stars</desc>
        </param>
        <param optional="true">
            <name>visErr</name>
            <type>double</type>
            <defaultValue><double>0.5</double></defaultValue>
            <minValue><double>0.0</double></minValue>
            <maxValue><double>1.0</double></maxValue>
            <desc>expected visibility error</desc>
        </param>
        <param optional="true">
            <name>noScienceStar</name>
            <type>boolean</type>
            <defaultValue><boolean>true</boolean></defaultValue>
            <desc>specify whether the science has to be removed from the list or not</desc>
        </param>
    </params>        
</cmd>

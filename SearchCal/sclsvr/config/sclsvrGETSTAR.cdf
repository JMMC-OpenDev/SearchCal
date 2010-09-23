<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETSTAR.cdf,v 1.6 2010-09-23 19:18:41 mella Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $
 Revision 1.5  2005/02/07 14:42:11  gzins
 Renamed lambda to wlen.

 Revision 1.4  2005/02/03 10:38:32  gzins
 Fixed conflict

 Definition of GETSTAR command.
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

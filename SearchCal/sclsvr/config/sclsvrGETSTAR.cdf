<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
 JMMC project

 "@(#) $Id: sclsvrGETSTAR.cdf,v 1.4 2005-02-03 10:38:32 gzins Exp $"

 History 
 ~~~~~~~
 $Log: not supported by cvs2svn $ 

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
            <desc>the star name</desc>
        </param>
        <param>
            <name>baseline</name>
            <type>double</type>
            <defaultValue><double>100.00</double></defaultValue>
            <unit>m</unit>
            <desc>the baseline used to compute visibility</desc>
        </param>
        <param>
            <name>lambda</name>
            <type>double</type>
            <desc>the observing wavelength used to compute visibility</desc>
            <defaultValue><double>1.00</double></defaultValue>
            <unit>us</unit>
        </param>
    </params>        
</cmd>

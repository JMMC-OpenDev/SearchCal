<?xml version="1.0" encoding="UTF-8"?>

<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <mnemonic>GETSTAR</mnemonic>
    <desc>Command to ask the server some information about a specific star.
        the output result is write in this order :

        name of the star
        right ascension
        declinaison
        position date
        hipparcos catalog number
        spectral type
        galactic lattitude
        galactic longitude
        paralax
        Magnitude in band B V R I J H K L M
        Corrected Magnitude in band B V R I J H K L M
        angular diameter of the star
        angular diameter error

        if a propery is not found or can't be computed, '-' is returned
    </desc>
    <params>
        <param>
            <name>objectName</name>
            <type>string</type>
            <desc>the star name</desc>
        </param>
    </params>        
</cmd>

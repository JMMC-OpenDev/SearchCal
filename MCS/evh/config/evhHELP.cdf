<?xml version="1.0" encoding="UTF-8"?>
<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">
    <!--
    This cdf describes the HELP command.
    -->
    <mnemonic>HELP</mnemonic>
    <desc>Get a short description of the commands supported by the application, or a description of a given command.</desc>
    <params>
        <param optional="true">
            <name>command</name>
            <desc>get detailed description of the command</desc>
            <type>string</type>
        </param>
    </params>
</cmd>

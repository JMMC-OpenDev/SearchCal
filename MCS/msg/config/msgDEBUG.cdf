<?xml version="1.0" encoding="UTF-8"?>
<!--
********************************************************************************
* JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
********************************************************************************

This cdf describes the DEBUG command.

-->

<cmd
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="cmdDefinitionFile.xsd">

    <mnemonic>DEBUG</mnemonic>
    <desc>Changes logging levels on-line. Levels are defined from 1 to 5 whereby
level 1 produces only limited number of logs, and level 5 produces logs
at a very detailed level.</desc>
    <params>
        <param optional="true">
            <name>stdoutLevel</name>
            <desc>level for logs which are printed on stdout</desc>
            <type>integer</type>
        </param>
        <param optional="true">
            <name>logfileLevel</name>
            <desc>level for logs which are stored into the log file</desc>
            <type>integer</type>
        </param>
        <param optional="true">
            <name>printDate</name>
            <desc>switch on/off printing of date</desc>
            <type>boolean</type>
        </param>
        <param optional="true">
            <name>printFileLine</name>
            <desc>switch on/off printing of file name and line number</desc>
            <type>boolean</type>
        </param>
    </params>
</cmd>

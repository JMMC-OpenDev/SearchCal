#! /bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: errXmlToH.sh,v 1.7 2005-02-12 14:01:07 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2005/01/31 14:48:59  mella
# Give new doxygen documentation style
#
# Revision 1.5  2005/01/31 14:26:51  mella
# Check if xml error file does exist
#
# Revision 1.4  2005/01/31 13:01:22  mella
# Add log tag for history
#
#*******************************************************************************

#/**
# \file 
# Generate the error defines from the XML definition file. 
#
# \synopsis
# errXmlToH \<input.xml\> \<output.h\>   
#
# \details
# errXmlToH generates a C header file. The output file defines each error's
# constant. Errors are defined into the XML input file 
#
# This script exits with 0 on success or 1 on failure.
#
# \usedfiles
# errXmlToH.xsl: XSLT stylesheet file to generate header files
#
# */

# signal trap (if any)


#___oOo___#Parameters Integrity Check

#This function returns the complete pathname of the file given in parameter
function getCfgFile
{
	#Path list
	path="../config $INTROOT/config $MCSROOT/config"
    for dir in $path
    do
        if [ -f "$dir/$1" ]
        then
            fullPath="$dir/$1"
            return
        fi
    done
    fullPath="NULL"
}

#Main Script
if [ $# != 2 ];
then
	echo "USAGE : $0 ../errors/<mod>Errors.xml ../include/<mod>Errors.h" >&2
else
    # Check if error file does exist
    if [ ! -f $1 ]
    then
        echo "ERROR: XML error file not found" >&2
        exit 1;
    fi
    
    #XSD & XSL declaration files
    getCfgFile "errXmlToH.xsd"
    if [ "$fullPath" = "NULL" ]
    then
        echo "ERROR: XSD Schema File Not Found" >&2
        exit 1;
    fi
    schema="$fullPath"

    getCfgFile "errXmlToH.xsl"
    if [ "$fullPath" = "NULL" ]
    then
        echo "ERROR: XSL Transformation File Not Found" >&2
        exit 1;
    fi
    xslt="$fullPath"

    # Check Xml file validity
    xmllint --noout --schema $schema $1 &> $1.tmpres 

    # Check if validation was ok or failed
    grep "fail" $1.tmpres >&2
    res=$?    
    if [ $res -eq 1 ];
    then
        # if output file exists then exit
        if [ -a $2 ];
        then 
            echo "ERROR: File $2 already exists." >&2
            echo "Please change filename and run it again." >&2
            exit 1;
        fi

        xsltproc $xslt $1 > $2
        echo "Header file $2 created successfully."
    else
        echo "ERROR: Sorry, validation error. You need to modify $1." >&2
        exit 1;
    fi
    rm $1.tmpres
fi

exit 0;






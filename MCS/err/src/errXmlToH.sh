#! /bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: errXmlToH.sh,v 1.2 2004-09-29 08:37:53 mella Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# mella     24-Jun-2004  Created
#
#
#*******************************************************************************
#   NAME
#   errXmlToH - Generate the error defines from the XML definition file. 
#
#   SYNOPSIS
#   errXmlToH input.xml output.h   
#
#   DESCRIPTION
#   errXmlToH generates a C header file. The output file defines each error's
#   constant. Errors are defined into the XML input file 
#
#   FILES
#
#   ENVIRONMENT
#
#   RETURN VALUES
#   errXmlToH exits with a value of 0 on success or 1 on failure.
#
#   CAUTIONS
#
#   EXAMPLES
#   errXmlToH ../errors/modErrors.xml ../include/modErrors.h   
#   
#
#   SEE ALSO
#
#   BUGS     
#
#-------------------------------------------------------------------------------
#

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
	echo "USAGE : $0 ../errors/<mod>Errors.xml ../include/<mod>Errors.h" > /dev/stderr
else
	#XSD & XSL declaration files
    getCfgFile "errXmlToH.xsd"
    if [ "$fullPath" = "NULL" ]
    then
        echo "XSD Schema File Not Found" > /dev/stderr
        exit 1;
    fi
    schema="$fullPath"

    getCfgFile "errXmlToH.xsl"
    if [ "$fullPath" = "NULL" ]
    then
        echo "XSL Transformation File Not Found" > /dev/stderr
        exit 1;
    fi
    xslt="$fullPath"

    # Check Xml file validity
    xmllint --noout --schema $schema $1 &> $1.tmpres 

    grep "fails" $1.tmpres &> /dev/stderr
    res=$?    
    if [ $res -eq 1 ];
    then
        echo "$val"

        # if output file exists then exit
        if [ -a $2 ];
        then 
            echo "File $2 already exists." > /dev/stderr
            echo "Please change filename and run it again." > /dev/stderr
            exit 1;
        fi

        xsltproc $xslt $1 > $2
        echo "Header file $2 created successfully."
    else
        cat $1.tmpres > /dev/stderr
        echo "Sorry, validation error. You need to modify $1." > /dev/stderr
    fi
#    rm $1.tmpres
fi

exit 0;






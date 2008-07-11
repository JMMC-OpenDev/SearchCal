#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOParseResult.sh,v 1.1 2008-07-11 12:56:56 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

#/**
# @file
# Create synthesis files for the ESO calibrator catalog
#
# @synopsis
# sclcatESOParseResult \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @usedfiles
# @filename ../config/sclcatESO.cfg : list of stars of interest for PRIMA
# @filename ../config/sclcatVOTable2html.xsl : xsl style sheet to convert
# VOTable to html
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatESOParseResult [eso-ref] or [eso-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t<eso-run>\tParse results in eso-run directory"
    exit 1;
}

while getopts "h" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

XSLT_VOT2HTML="$PWD/../config/sclcatVOTable2html.xsl"

# define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# Parse command-line parameters
dir=$1
if [ ! -d "$dir" ]
then
    echo "Please give one run directory"
    exit 1
fi

cd $dir

HTMLDIR="html/"
mkdir $HTMLDIR &> /dev/null

# copy every votable and append its calibrators to result VOTable
for i in *.vot
do
    if [ ! -e "$HTMLDIR/$i" ]
    then
        cp "$i" $HTMLDIR
    fi
done


cd $HTMLDIR

# Generating VOTable header
tmp=( $(ls *.vot) )
RESULTFILE=../catalog.vot
cat $tmp | awk '{if ($1=="<TR>")end=1;if(end!=1)print;}' &> $RESULTFILE

# loop on every calibrators of every stars
# and build calibrator file
for i in *.vot
do
    if [ -f "$i" ]
    then
        xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -c "//VOT:TR" $i >> $RESULTFILE
    fi
done

# Generating VOTable footer
cat $tmp | awk '{if ($1=="</TABLEDATA>")start=1;if(start==1)print;}' >> $RESULTFILE

echo -e "Finished at: "
date

echo -e "Number of calibrators retrieved: "
xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -v "count(//VOT:TR)" $RESULTFILE

#Now CALIBRATORS file can be presented by next stylesheet
echo "Html resume generated"
xsltproc --path .:.. -o "catalog.html" $XSLT_VOT2HTML "$RESULTFILE"

#___oOo___

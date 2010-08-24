#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatPrimaParseResult.sh,v 1.27 2010-08-24 15:23:07 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.26  2010/08/24 14:05:01  mella
# copy vot catalog with scvot extension
#
# Revision 1.25  2010/06/16 08:40:40  mella
# use sclcatConcatenateVotables instead of old sclcatESOParseResult
#
# Revision 1.24  2009/12/02 10:39:40  mella
# add computation of stars without kmag
#
# Revision 1.23  2009/11/17 21:18:32  mella
# add two new files for stars and their galactical coords
#
# Revision 1.22  2009/11/16 17:32:01  mella
# fix accepted filter
#
# Revision 1.21  2009/11/16 16:54:25  mella
# compute dist1 dist2 even for bad calibrators
#
# Revision 1.20  2009/11/16 09:08:53  mella
# add kmag and galactical coordinates in calibrators files
#
# Revision 1.19  2009/10/23 19:13:40  mella
# Fix parsing to include diamFlag=Nok into calibrator file
#
# Revision 1.18  2009/10/09 11:40:06  mella
# fix generation of results into html/result dir
#
# Revision 1.17  2009/10/08 20:45:18  mella
# fix ok_number/cal/count
#
# Revision 1.16  2009/10/08 14:31:44  mella
# generate html view of compiled catalog
#
# Revision 1.15  2009/10/08 13:17:59  mella
# change varaible name
#
# Revision 1.14  2009/10/07 08:18:27  mella
# add concatenation to build main catalog
#
# Revision 1.13  2009/09/16 12:41:02  mella
# add transformation to get param of table as new fields
#
# Revision 1.12  2009/08/24 12:38:45  mella
# do not compute twice orbits
#
# Revision 1.11  2009/08/24 11:31:19  mella
# remove unused code
#
# Revision 1.10  2009/05/18 15:48:57  mella
# output table.dat
#
# Revision 1.9  2009/04/29 10:44:03  mella
# add first implementation to compute orbit
#
# Revision 1.8  2009/04/27 11:57:51  mella
# fix bug
#
# Revision 1.7  2009/04/27 08:58:39  mella
# fix hardcoded values with dynamic accessor
#
# Revision 1.6  2009/01/22 14:03:22  mella
# copy input xml file at end of generation
#
# Revision 1.5  2007/03/27 14:55:56  scetre
# Updated documentation
#
# Revision 1.4  2007/02/07 14:57:23  mella
# Set calPmRa or calPmDec to 0 for filter process call
#
# Revision 1.3  2007/01/31 12:39:54  mella
# Use pmra and pmDec to 0 if star has not yet for filter program
#
# Revision 1.2  2007/01/31 10:25:31  mella
# Change bad test condition
#
# Revision 1.1  2007/01/31 10:01:00  mella
# First revision
#
#
#*******************************************************************************

#/**
# @file
# Create synthesis files for the PRIMA calibrator catalog
#
# @synopsis
# sclcatPrimaParseResult \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @usedfiles
# @filename ../config/sclcatPRIMA.cfg : list of stars of interest for PRIMA
# @filename ../config/sclcatVOTable2html.xsl : xsl style sheet to convert VOTable to html
# @filename ../config/sclcatObjectsToHtml.xsl :  xsl style sheet to convert object file to html
# @filename ../config/sclcatSimbadList.xml : Simbad return properties for each stars (ra, dec, pmra, pmdec, parallax).
# @filename ../config/sclcatPrimaStars.xml : Stars list
#
# */

# Print usage 
function printUsage () {
echo -e "Usage: sclcatPrimaParseResult [prima-ref] or [prima-run-<YYYY-MM-DDTHH-MM-SS>]" 
echo -e "\t-h\t\tprint this help."
echo -e "\t<prima-run>\tParse results in prima-run directory"
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
XSLT_OBJECT2HTML="$PWD/../config/sclcatObjectsToHtml.xsl"
XSLT_OBJECT2LATEX="$PWD/../config/sclcatObjectsToLatex.xsl"
XSLT_OBJECT2CSV="$PWD/../config/sclcatObjectsToCSV.xsl"
XSLT_OBJECT2DAT="$PWD/../config/sclcatObjectsToDat.xsl"
XSLT_OBJECT2MISSINGKMAG="$PWD/../config/sclcatObjectsWithoutKmag.xsl"
# Next file contains all grabed data
SIMBAD_FILE="$PWD/../config/sclcatSimbadList.xml"
ALIAS_FILE="$PWD/../config/sclcatAliases.xml"
PRIMA_STAR_LIST="$PWD/../config/sclcatPrimaStars.xml"
EXOPLANET_VOTABLE="$PWD/../config/sclcatPrimaExoplanetData.xml"
MICROLENSING_STAR_LIST="$PWD/../config/sclcatMicrolensingStars.xml"

# define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# define timespan to use 
timespan=20
echo "Timespan is $timespan [yr]"

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

# copy every votable and transform it into html
# the copy ethod also copy the param of the votable as fields
let idx=0
for i in *.vot
do
    if [ ! -e "$HTMLDIR/$i" ]
    then
        vobsCopyVotParamsAsFields "$i" > "$HTMLDIR/$i"
        xsltproc --path .:.. -o "$HTMLDIR/$i.html" $XSLT_VOT2HTML "$HTMLDIR/$i"
				echo -ne "\r$idx $i                        \r$idx"
				let idx=$idx+1
    fi
done

XMLFILE=$HTMLDIR/result/catalog.vot
if [ ! -e $XMLFILE ]
then
	sclcatConcatenateVotables $HTMLDIR
fi
HTMLFILE=$XMLFILE.html
if [ $HTMLFILE -ot $XMLFILE ]
then
	xsltproc --path .:.. -o "$HTMLFILE" $XSLT_VOT2HTML "$XMLFILE"
fi
cp $XMLFILE ${XMLFILE/.vot/.scvot}

cd $HTMLDIR

getColumnIndex()
{
    VOTABLE_FILENAME="$1"
    COLNAME="$2"
    VAL="$(xml sel -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t  -m "//VOT:FIELD" -i "@name='$COLNAME'" -v "position()" "$VOTABLE_FILENAME")"
    if [ -n "$VAL" ]
    then
        echo "$VAL"
        return
    fi
    VAL="$(xml sel -N VOT=http://exoplanet.eu -t  -m "//VOT:FIELD" -i "@name='$COLNAME'" -v "position()" "$VOTABLE_FILENAME")"
    if [ -n "$VAL" ]
    then
        echo "$VAL"
        return
    fi
    return 1

}

getCellValue()
{
    VOTABLE_FILENAME="$1"
    COLNAME="$2"
    ROWINDEX="$3"

    COLINDEX=$(getColumnIndex "$VOTABLE_FILENAME" "$COLNAME")
    VAL=$(xml sel -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -v "//VOT:TR[$ROWINDEX]/VOT:TD[$COLINDEX]" "$VOTABLE_FILENAME")
    if [ -n "$VAL" ]
    then
        echo "$VAL"
        return
    fi
    VAL=$(xml sel -N VOT=http://exoplanet.eu -t -v "//VOT:TR[$ROWINDEX]/VOT:TD[$COLINDEX]" "$VOTABLE_FILENAME")
    if [ -n "$VAL" ]
    then
        echo "$VAL"
        return
    fi
    return 1
}

CALIBRATORS=calibrators.xml
# loop on every calibrators of every stars
# and build calibrator file
genCalibratorList()
{
    echo "<calibrators>" > $CALIBRATORS

    # loop on every calibrators of every stars
    # and build calibrator file
    for i in *.vot
    do
        if [ -f "$i" ]
        then
            echo
            # Search indice of diamFlag column
            DIAM_FLAG_INDEX=$( getColumnIndex "$i" "diamFlag" )

            # Search indexes into exoplanet votable
            ST_NAME_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "St_Name")
            ST_MASS_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "St_Mass")
            ST_DIST_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "St_Dist")
            PL_MASS_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "Pl_Mass")
            PL_SEMIAXIS_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "Pl_SemiAxis")
            PL_ECC_INDEX=$(getColumnIndex "$EXOPLANET_VOTABLE" "Pl_Ecc")

            starName=${i%.vot}
            OUT="$(xml sel -N VOT=http://exoplanet.eu -t -v "count(//VOT:TR[./VOT:TD[$ST_NAME_INDEX]='$starName'])" "$EXOPLANET_VOTABLE" )"
            if [ "$OUT" -ge 1 ]
            then
                exoplanetStarName="$starName"
            else
                exoplanetStarName=$(xml sel -t -v "//object[@alias='$starName']/@name" $ALIAS_FILE)
                OUT="$(xml sel -N VOT=http://exoplanet.eu -t -v "count(//VOT:TR[./VOT:TD[$ST_NAME_INDEX]='$exoplanetStarName'])" "$EXOPLANET_VOTABLE" )"
                if [ "$OUT" -lt 1 ]
                then
                    echo
                    echo "Can't find star into exoplanet votable using following names $starName $exoplanetStarName"
                    echo
                fi
            fi


            starRa=$(xml sel -t -v "//object[name='$starName']/ra" $SIMBAD_FILE)
            starPmRa=$(xml sel -t -v "//object[name='$starName']/pmra" $SIMBAD_FILE)
            starDec=$(xml sel -t -v "//object[name='$starName']/dec" $SIMBAD_FILE)
            starPmDec=$(xml sel -t -v "//object[name='$starName']/pmdec" $SIMBAD_FILE)

            # if pmra or pmdec not found set it to 0
            if [ -z "$starPmRa" ]
            then
                starPmRa=0
            fi
            if [ -z "$starPmDec" ]
            then
                starPmDec=0
            fi

            NBCAL=$(xml sel   -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -v "count(//VOT:TR)" "$i")
            echo "<star simbadName=\"$starName\" scCount=\"$NBCAL\">" >>  $CALIBRATORS
            xml sel -t -m "//object[name='$starName']" -c "*[not(name()='name')]" -b $SIMBAD_FILE >> $CALIBRATORS

            # compute orbit size
            ST_ARG="$(xml sel -N VOT=http://exoplanet.eu -t -m "//VOT:TR[./VOT:TD[$ST_NAME_INDEX]='$exoplanetStarName']" -i "position()=1" -o "&quot;" -v "./VOT:TD[$ST_MASS_INDEX]" -o "&quot; &quot;" -v "./VOT:TD[$ST_DIST_INDEX]" -o "&quot; " "$EXOPLANET_VOTABLE" )"
            PL_ARGS="$(xml sel -N VOT=http://exoplanet.eu -t -m "//VOT:TR" -i "./VOT:TD[$ST_NAME_INDEX]='$exoplanetStarName'" -o "&quot;" -v "./VOT:TD[$PL_MASS_INDEX]" -o "&quot; &quot;" -v "./VOT:TD[$PL_SEMIAXIS_INDEX]" -o "&quot; &quot;" -v "./VOT:TD[$PL_ECC_INDEX]" -o "&quot; " -n "$EXOPLANET_VOTABLE" )"
            ORBIT=$(eval sclcatComputeOrbit $ST_ARG $PL_ARGS)
            echo "<!-- sclcatComputeOrbit $ST_ARG $PL_ARGS -->">> $CALIBRATORS
            echo $ORBIT >> $CALIBRATORS
            echo "sclcatComputeOrbit $ST_ARG $PL_ARGS "
            echo $ORBIT 


            echo -n "$starName  "
            xml sel   -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t \
            -v "count(//VOT:TR[./VOT:TD[$DIAM_FLAG_INDEX]='OK'])" -o "/" \
            -v "count(//VOT:TR)" "$i" 



            # output copy of calibrator
#            -m "//VOT:TR" -i "./VOT:TD[$DIAM_FLAG_INDEX]='OK'" \
            INDICES=$(xml sel -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t \
            -m "//VOT:TR" \
            -v "position()" -o " " "$i")

            for index in $INDICES
            do
		echo -n "$index "
                calName=$(getCellValue "$i" 2MASS $index )
                calMagK=$(getCellValue "$i" K $index )
                calRa=$(getCellValue "$i" RAJ2000 $index )
                calDec=$(getCellValue "$i" DEJ2000 $index )
                calPmRa=$(getCellValue "$i" pmRa $index )
                calPmDec=$(getCellValue "$i" pmDec $index )
                calDist=$(getCellValue "$i" dist $index )
                calDiamFlag=$(getCellValue "$i" diamFlag $index )
                calGLat=$(getCellValue "$i" GLAT $index )
                calGLon=$(getCellValue "$i" GLON $index )
                echo "  <calibrator index=\"$index\">" >>  $CALIBRATORS
                echo "    <name>$calName</name>" >>  $CALIBRATORS
                echo "    <kmag>$calMagK</kmag>" >>  $CALIBRATORS
                echo "    <ra>$calRa</ra><dec>$calDec</dec>" >>  $CALIBRATORS
                echo "    <pmra>$calPmRa</pmra><pmdec>$calPmDec</pmdec>" >>  $CALIBRATORS
                echo "    <dist>$calDist</dist>" >>  $CALIBRATORS
                echo "    <glat>$calGLat</glat>" >>  $CALIBRATORS
                echo "    <glon>$calGLon</glon>" >>  $CALIBRATORS
                # if pmra or pmdec not found set it to 0
                if [ -z "$calPmRa" ]
                then
                    calPmRa=0
                fi
                if [ -z "$calPmDec" ]
                then
                    calPmDec=0
                fi

								# prep command
								FILTERINFO=$(sclcatPrimaFilter "$starRa" "$starDec" "$starPmRa" \
								"$starPmDec" "$calRa" "$calDec" "$calPmRa" "$calPmDec" "$timespan")
								if [ $? -ne 0 ]
								then
												echo "ERROR occured for calib n° $index"
												echo "<calibInfo><error/></calibInfo>" >> $CALIBRATORS
								else
												echo "<!-- \ 
												sclcatPrimaFilter \"$starRa\" \"$starDec\" \"$starPmRa\" \
												\"$starPmDec\" \"$calRa\" \"$calDec\" \"$calPmRa\" \"$calPmDec\" \"$timespan\" \
												-->" >> $CALIBRATORS
												if [ "$calDiamFlag" == "OK" ]
												then
																echo $FILTERINFO >> $CALIBRATORS
												else
																# copy all information excepted the accepted flag
																echo $FILTERINFO | xml sel -t -e calibInfo -m "calibInfo/*[not(contains(name(),'accepted'))]" -c "." -b -e rejected -e diamFlagNok -b >> $CALIBRATORS
												fi
												echo "  </calibrator>" >>  $CALIBRATORS
								fi
            done
						echo 
            echo "</star>" >>  $CALIBRATORS  
        fi
    done
    echo "</calibrators>" >> $CALIBRATORS
}

if [ -e "$CALIBRATORS" ]
then
    echo " $CALIBRATORS file already computed skip build "
else
    genCalibratorList
fi

OUTPUT_FILE=starsWithoutKmag.txt
echo "List of star without kmag $PWD/$OUTPUT_FILE"
xsltproc  --path ./html:.:.. -o "$OUTPUT_FILE" --stringparam calibratorsFilename \
$CALIBRATORS --stringparam mainFilename $SIMBAD_FILE \
$XSLT_OBJECT2MISSINGKMAG $PRIMA_STAR_LIST
mv -v $OUTPUT_FILE result


OUTPUT_FILE=stars.csv
echo "Stars votable generated into $PWD/$OUTPUT_FILE"
xsltproc  --path ./html:.:.. -o "$OUTPUT_FILE" --stringparam calibratorsFilename \
$CALIBRATORS --stringparam mainFilename $SIMBAD_FILE \
$XSLT_OBJECT2CSV $PRIMA_STAR_LIST
stilts tpipe ifmt=csv istream=false in=$OUTPUT_FILE cmd='addskycoords -inunit sex fk5 gal $2 $3 GAL_LONG GAL_LAT' omode=out ofmt=csv out=starsWithGal.csv
mv -v starsWithGal.csv result
mv -v $OUTPUT_FILE result


#Now CALIBRATORS file can be presented by next stylesheet
OUTPUT_FILE=table.dat
echo "Dat ( ascii file ) generated into $PWD/$OUTPUT_FILE"
xsltproc  --path ./html:.:.. -o "$OUTPUT_FILE" --stringparam calibratorsFilename \
$CALIBRATORS --stringparam mainFilename $SIMBAD_FILE \
$XSLT_OBJECT2DAT $PRIMA_STAR_LIST
mv -v $OUTPUT_FILE result

OUTPUT_FILE=index.html
echo "Html resume generated into $PWD/$OUTPUT_FILE"
xsltproc  --path ./html:.:.. -o "$OUTPUT_FILE" --stringparam calibratorsFilename \
$CALIBRATORS --stringparam mainFilename $SIMBAD_FILE \
--stringparam microlensingFilename $MICROLENSING_STAR_LIST \
$XSLT_OBJECT2HTML $PRIMA_STAR_LIST

OUTPUT_FILE=table.tex
echo "Latex table generated into $PWD/$OUTPUT_FILE"
xsltproc  --path ./html:.:.. -o "$OUTPUT_FILE" --stringparam calibratorsFilename \
$CALIBRATORS --stringparam mainFilename $SIMBAD_FILE \
$XSLT_OBJECT2LATEX $PRIMA_STAR_LIST
mv -v $OUTPUT_FILE result



# copy xml files
cp -v $PRIMA_STAR_LIST  result
cp -v $SIMBAD_FILE result
cp -v $CALIBRATORS result

cd result

# plot 2d repartition diagram
stilts plot2d cmd='addcol -units radians deltaRA "hmsToRadians(ra)-hmsToRadians(RAJ2000)"; addcol -units radians deltaDEC "dmsToRadians(dec)-dmsToRadians(DEJ2000)"; addcol starDEC "dmsToRadians(DEJ2000)"' subset2='equals(diamFlag,"OK")' name2='diam OK' subset1='equals(diamFlag,"NOK")' name1='diam NOK' xdata='deltaRA*cos(starDEC)' ydata='deltaDEC' in=catalog.vot omode=out out=calibsRelativePositions.png

# plot data for herve's greg scripts
stilts tpipe cmd='clearparams * ; keepcols "ra dec RAJ2000 DEJ2000 diamFlag"' in=catalog.vot out=Star_RaDec_Cal_RaDecDiamFlag.csv 
sed -i~ "s/,/ /g" Star_RaDec_Cal_RaDecDiamFlag.csv
stilts tpipe cmd='clearparams * ; keepcols "ra dec RAJ2000 DEJ2000 diamFlag"' in=catalog.vot out=Star_RaDec_Cal_RaDecDiamFlag.ascii ofmt=ascii


#___oOo___

#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Compare two catalogs with the same name of columns.
#
# @synopsis
# \<sclcatCompareCat\> <-h|-c|-e>
#
# @opt
# @optname h : show usage help;
#
# @details
# Tables must have RAJ2000 and DEJ2000 columns to allow hard coded cross matches
# */


SCRIPTNAME="$(basename $0)"

usage(){
  echo "Usage: $SCRIPTNAME <cat1> <cat2>"
}
echoError(){
  echo "ERROR: $*" > /dev/stderr
}
toHtml()
{
  MYHTML="$MYHTML \n$*"
}

catToTd(){
  c="$1"
  toHtml "<td><a href='./$c'>$c</a><ul><li>$(head -1 ${c}.stats.txt)</li><li><a href='./$c.meta.txt'>META</a> / <a href='./$c.stats.txt'>STATS</a></li></ul></td>"
}

if [ $# -ne 2 ]
then
  echoError "Two catalog required as input parameters"
  usage
  exit 1
fi

CAT1_FN=$(readlink -f $1)
ls $CAT1_FN > /dev/null || exit 1 
CAT1=$(basename $CAT1_FN)

CAT2_FN=$(readlink -f $2)
ls $CAT2_FN > /dev/null || exit 1
CAT2=$(basename $CAT2_FN)

OUTPUTDIR="DIFF_${CAT1/%./_}_${CAT2/%./_}"

echo "Comparing $CAT1 and $CAT2 ..."


mv -v $OUTPUTDIR $OUTPUTDIR.$(date +"%s") 2> /dev/null 
mkdir $OUTPUTDIR
cp $CAT1 $CAT2 $OUTPUTDIR
cp $0 $OUTPUTDIR/$SCRIPTNAME.backup
cd $OUTPUTDIR

doXmatch(){
    for join in 1and2 1not2 2not1
    do
        stilts tskymatch2 ra1='radiansToDegrees(hmsToRadians(RAJ2000))' ra2='radiansToDegrees(hmsToRadians(RAJ2000))' dec1='radiansToDegrees(dmsToRadians(DEJ2000))' dec2='radiansToDegrees(dmsToRadians(DEJ2000))' error=1 join="$join" find="best" out="${join}.fits" "$CAT1" "$CAT2"
    done
}

doStats(){
for c in *.fits 
do
stilts tpipe omode=stats $c > ${c}.stats.txt 
stilts tpipe omode=meta $c > ${c}.meta.txt 
done
}

doHisto(){
toHtml "<h1>Normalized histogramms on each numerical columns</h1>"

# NF==13 is true for numerical values and NR=3 must be discarded because it is the table column header name
meta1=$(cat ${CAT1}.stats.txt|awk '{if (NF==13 && NR!=3) print $2}')
meta2=$(cat ${CAT2}.stats.txt|awk '{if (NF==13 && NR!=3) print $2}')

metas=$(echo "$meta1" "$meta2") 

for m in $metas
do 
PNG=histo_${m}.png
if [ ! -e $PNG ] 
then
echo $m
stilts plothist xpix=600 out=$PNG norm=true xdata1=$m xdata2=$m xdata3=$m xdata4=$m ofmt=png in1=$CAT1 in2=$CAT2 in3=1not2.fits in4=2not1.fits name1=CAT1 name2=CAT2 name3=1not2 name4=2not1
fi
toHtml "<image src='$PNG' alt='meta $m'/>"
done
}


doXmatch
doStats

toHtml "<h1>Compared catalogs</h1>"
toHtml "<table border='1'>"
toHtml "<tr><th>CAT1</th><th>Common part</th><th>CAT2</th></tr>"
toHtml "<tr>"
catToTd $CAT1
toHtml "<td></td>"
catToTd $CAT2
toHtml "</tr>"
toHtml "<tr>"
toHtml "<td></td>"
catToTd 1and2.fits
toHtml "<td></td>"
toHtml "</tr>"
toHtml "<tr>"
catToTd 1not2.fits
toHtml "<td></td>"
catToTd 2not1.fits
toHtml "</tr>"
toHtml "<tr>"
toHtml "</table>"

doHisto

toHtml "<pre>Generated on $(date +'%c') by </pre> <a href='${SCRIPTNAME}.backup'>$SCRIPTNAME</a>"

echo -e "$MYHTML" > index.html

echo "Produced files in '$OUTPUTDIR':"
ls 

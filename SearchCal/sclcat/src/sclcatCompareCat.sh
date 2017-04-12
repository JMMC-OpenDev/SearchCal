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

# use headless mode
STILTS_JAVA_OPTIONS=" -Xms512m -Xmx1024m -Djava.awt.headless=true"
echo "Stilts options: $STILTS_JAVA_OPTIONS"

echo "Comparing $CAT1 and $CAT2 ..."


mv -v $OUTPUTDIR $OUTPUTDIR.$(date +"%s") 2> /dev/null 
mkdir $OUTPUTDIR
ln -v $CAT1 $CAT2 $OUTPUTDIR
cp $0 $OUTPUTDIR/$SCRIPTNAME.backup
cd $OUTPUTDIR


_getColumnMeta(){
    # NF==13 is true for numerical values and NR=3 must be discarded because it is the table column header name
    cat ${CAT2}.stats.txt ${CAT1}.stats.txt |awk '{if (NF==13 && NR!=3) print $2}' |sort |uniq -d
}

doXmatch(){
    for join in 1and2 1not2 2not1
    do
        stilts ${STILTS_JAVA_OPTIONS} tskymatch2 ra1='radiansToDegrees(hmsToRadians(RAJ2000))' ra2='radiansToDegrees(hmsToRadians(RAJ2000))' dec1='radiansToDegrees(dmsToRadians(DEJ2000))' dec2='radiansToDegrees(dmsToRadians(DEJ2000))' error=1 join="$join" find="best" out="${join}.fits" "$CAT1" "$CAT2"
    done
}

doStats(){
for c in *.fits 
do
stilts ${STILTS_JAVA_OPTIONS} tpipe omode=stats $c > ${c}.stats.txt 
stilts ${STILTS_JAVA_OPTIONS} tpipe omode=meta $c > ${c}.meta.txt 
done
}

doHisto(){
toHtml "<h1>Normalized histogramms on each numerical columns</h1>"

# NF==13 is true for numerical values and NR=3 must be discarded because it is the table column header name
meta1=$(cat ${CAT1}.stats.txt|awk '{if (NF==13 && NR!=3) print $2}')
meta2=$(cat ${CAT2}.stats.txt|awk '{if (NF==13 && NR!=3) print $2}')

metas=$(echo "$meta1" "$meta2") 

if echo $metas | grep pmRA &> /dev/null
then
    if echo $metas | grep pmDEC &> /dev/null
    then
    PMDIST="sqrt(pmRA*pmRA+pmDEC*pmDEC)"
    fi
fi

if echo $metas | grep plx | grep e_plx &> /dev/null; then EPLX="e_plx/plx" ;fi

for m in "radiansToDegrees(hmsToRadians(RAJ2000))" "radiansToDegrees(dmsToRadians(DEJ2000))" $metas $EPLX
do 
    if [ "$m" == "pmDEC" -o "$m" == "pmRA" ]
        then
            if [ -n "$PMDIST" ]
                then
                    m=$PMDIST
            fi
    fi

    PNG=$(echo histo_${m}.png |tr "/" "_")
    if [ ! -e $PNG ] 
    then
        echo $m
        stilts ${STILTS_JAVA_OPTIONS} plothist xpix=800 ypix=550 out="$PNG" ylog=true norm=false xdata1="$m" xdata2="$m" xdata3="$m" xdata4="$m" ofmt=png in1=$CAT1 in2=$CAT2 in3=1not2.fits in4=2not1.fits name1=CAT1 name2=CAT2 name3=1not2 name4=2not1
        toHtml "<image src='$PNG' alt='meta $m'/>"
    fi
done

if echo $metas | grep LDD | grep e_LDD &> /dev/null; then EDIAM="e_LDD/LDD" ;fi
for m in $EDIAM
do 
    PNG=$(echo histo_${m}.png |tr "/" "_")
    if [ ! -e $PNG ] 
    then
        echo $m
        stilts ${STILTS_JAVA_OPTIONS} plothist xpix=800 ypix=550 out="$PNG" xlog=true ylog=true norm=false xdata1="$m" xdata2="$m" xdata3="$m" xdata4="$m" ofmt=png in1=$CAT1 in2=$CAT2 in3=1not2.fits in4=2not1.fits name1=CAT1 name2=CAT2 name3=1not2 name4=2not1
        toHtml "<image src='$PNG' alt='meta $m'/>"
    fi
done

}

doHR(){
    BAND1=$1
    BAND2=$2

    common_metas=$(_getColumnMeta)
    if echo $common_metas | grep ${BAND1}mag 
    then 
        if echo $common_metas | grep ${BAND2}mag 
        then
            PNG=HR_${BAND1}_${BAND2}.png
            x="${BAND1}mag-${BAND2}mag"
            y="-(${BAND2}mag+5+5*log10(0.001*plx))"
            stilts ${STILTS_JAVA_OPTIONS} plot2d xpix=800 ypix=550 out="old_$PNG" size1="0" size2="0" xdata1="$x" xdata2="$x" ydata1="$y" ydata2="$y" ofmt=png in1=$CAT1 in2=1not2.fits name1=CAT1 name2=1not2 
            stilts ${STILTS_JAVA_OPTIONS} plot2d xpix=800 ypix=550 out="new_$PNG" size1="0" size2="0" xdata1="$x" xdata2="$x" ydata1="$y" ydata2="$y" ofmt=png in1=$CAT2 in2=2not1.fits name1=CAT2 name2=2not1
            toHtml "<tr><td>"
            toHtml "<image src='old_$PNG' alt='HR_diagram on old catalog entries'/>"
            toHtml "</td><td>"
            toHtml "<image src='new_$PNG' alt='HR_diagram on new catalog entries'/>"
            toHtml "</td></tr>"
            return
        fi
    fi
    echo "WARNING : Missing columns ${BAND1}mag / ${BAND2}mag : HR diagrams skipped"
}

doDiams(){
# Ne marche pas sur le jsdc 
# (diam_a - diam_vk) / diam_vk    en fonction de   V - K
# avec a = [bv, vr, ik, jk, hk, sed] 
for i in bv vr ik jk hk sed 
do
    x="(diam_$i-diam_vk)/diam_vk"
    y="V-K"

    stilts ${STILTS_JAVA_OPTIONS} plot2d xpix=800 ypix=550 out="$PNG" xdata1="$x" xdata2="$x" xdata3="$x" xdata4="$x" ydata1="$y" ydata2="$y" ydata3="$y" ydata4="$y" ofmt=png in1=$CAT1 in2=$CAT2 in3=1not2.fits in4=2not1.fits name1=CAT1 name2=CAT2 name3=1not2 name4=2not1
        toHtml "<image src='$PNG' alt='meta $m'/>"
done
}

doUtils(){
    toHtml "<h1>HR diagrams</h1>"
    toHtml "<table>"
    doHR B V
    doHR V K
    doHR H K
    doHR R K
    doHR I K
    toHtml "</table>"
#    toHtml "<h1>Diameters diagrams</h1>"
#    doDiams
}


doDiff(){
    common_metas=$(_getColumnMeta)
DIFF_CMD=""
for m in $common_metas
do 
    diff_col="${m}_diff"
    DIFF_CMD="$DIFF_CMD addcol \"${diff_col}\" \"abs(${m}_1-${m}_2)\"; "
done

# add relative errors on LDD:
DIFF_CMD="$DIFF_CMD addcol \"re_LDD_1\" \"e_LDD_1/(LDD_1*ln(10))\"; "
DIFF_CMD="$DIFF_CMD addcol \"re_LDD_2\" \"e_LDD_2/(LDD_2*ln(10))\"; "
# add residuals on LDD:
DIFF_CMD="$DIFF_CMD addcol \"res_LDD_diff\" \"(log10(LDD_2)-log10(LDD_1))/sqrt(re_LDD_1*re_LDD_1+re_LDD_2*re_LDD_2)\"; "

echo stilts ${STILTS_JAVA_OPTIONS} tpipe in=1and2.fits out=tmp1and2.fits cmd="$DIFF_CMD; badval 0 \"*_diff\"" 
stilts ${STILTS_JAVA_OPTIONS} tpipe in=1and2.fits out=tmp1and2.fits cmd="$DIFF_CMD; badval 0 \"*_diff\"" 
mv tmp1and2.fits 1and2.fits

for m in $common_metas "(1./LDD_1)*LDD" "(1./e_LDD_1)*LDD" "res_LDD"
do 
    diff_col="${m}_diff"
    PNG=$(echo histo_${diff_col}.png |tr "/" "_")
    echo ${diff_col}
    stilts ${STILTS_JAVA_OPTIONS} plothist xpix=800 ypix=550 out="$PNG" ylog=true norm=false xdata1="${diff_col}" ofmt=png in1=1and2.fits name1=1and2
    toHtml "<image src='$PNG' alt='meta $diff_col'/>"
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
toHtml "<p>1and2.fits catalog contains a new diff (absolute value) column per column present in both compared catalogs. </p>"

doUtils
doHisto
doDiff
# perform stats again since 1and2.fits has changed in doDiff
doStats

toHtml "<pre>Generated on $(date +'%c') by </pre> <a href='${SCRIPTNAME}.backup'>$SCRIPTNAME</a>"

echo -e "$MYHTML" > index.html

echo "Produced files in '$OUTPUTDIR':"
ls 

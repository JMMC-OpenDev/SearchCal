#/bin/bash
# This file parse some account execution log for SearchCal webservice and
# produce an histogram
# 
# execution of this scripts loop on some account logs (produced by sclwsProxy.php) and produce the "sclwsStatUsage.png" file in the execution directory
#
# TODO accept options to define accounts to loop onto and output filename
#
logins="sclws betaswmgr"
pngFilename="sclwsStatUsage.png"

# temporary variable 
let idx=0
# buffer to build a cmd for later execution
stiltsCmd="stilts plothist binwidth=0.08333"

# loop on every accounts
for l in $logins
do
 let idx=$idx+1
 outfile=sclws_$l.csv
 
 # parse and produce csv file
 ( (echo "<a>"; eval cat ~$l/sclwsProxy.log ; echo "</a>") | xml sel -t -m "//date" -v "substring-before(.,'T')" -o ", $l" -n ) > $outfile 

 # build stilts cmd
 stiltsCmd="$stiltsCmd xdata${idx}='mjdToDecYear(isoToMjd(\$1))' name${idx}='$l'  in${idx}=$outfile ifmt${idx}=csv "

done

# perform stilts cmd to produce histogram
stiltsCmd="$stiltsCmd out=$pngFilename xpix=800 ypix=500"
#echo $stiltsCmd
eval $stiltsCmd
echo "$pngFilename generated for following logins: '$logins'"

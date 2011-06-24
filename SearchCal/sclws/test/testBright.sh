#!/bin/bash
source env.sh

for BAND in N V K
do
	sclwsTestServer "-objectName ETA_TAU -mag NaN -diffRa 1800.0 -diffDec 600.0 -band $BAND -minMagRange 2.0 -maxMagRange 4.0 -ra 03:47:29.0765 -dec 24:06:18.494 -baseMax 102.45 -wlen 10.0 -bright true -noScienceStar false" > sclwsTestBand_$BAND.log
done

#faint:
sclwsTestServer "-objectName hip 32617 -mag 5.01 -diffRa 3600.0 -diffDec 1200.0 -band K -minMagRange 3.01 -maxMagRange 7.01 -ra 06 48 19.08795 -dec -01 19 07.8207 -baseMax 102.45 -wlen 2.2 -radius 0.0 -bright false -noScienceStar false" > sclwsTestBand_FAINT.log

sclwsTestServer "-minMagRange 10 -mag 12.9 -objectName AzV_266 -wlen 2.2 -band K -bright false -baseMax 102.45 -ra 01:01:09.42 -dec -72:27:28.40 -maxMagRange 13" > sclwsTestBand_FAINT_AzV266.log


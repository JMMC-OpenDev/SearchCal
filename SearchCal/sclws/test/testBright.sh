#!/bin/bash
source env.sh

for BAND in N V K
do
	sclwsTestServer "-objectName ETA_TAU -mag NaN -diffRa 1800.0 -diffDec 600.0 -band $BAND -minMagRange 2.0 -maxMagRange 4.0 -ra 03:47:29.0765 -dec 24:06:18.494 -baseMax 102.45 -wlen 10.0 -bright true -noScienceStar false" > sclwsTestBand_$BAND.log
done


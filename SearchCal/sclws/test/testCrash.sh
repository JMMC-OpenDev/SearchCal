#!/bin/bash
source env.sh

#bright:
for BAND in V I J H K N
do
sclwsTestServer "-objectName ETA_TAU -mag NaN -diffRa 3600.0 -diffDec 1200.0 -band ${BAND} -minMagRange 2.0 -maxMagRange 4.0 -ra 03:47:29.0765 -dec 24:06:18.494 -baseMax 102.45 -wlen 10.0 -bright true -noScienceStar false -outputFormat 2013.7" > sclwsTestBand_${BAND}_${1}.log
done

#faint:
for BAND in I J H K
do
sclwsTestServer "-objectName hip 32617_FAINT -mag 5.01 -diffRa 3600.0 -diffDec 1200.0 -band ${BAND} -minMagRange 3.01 -maxMagRange 7.01 -ra 06 48 19.08795 -dec -01 19 07.8207 -baseMax 102.45 -wlen 2.2 -radius 0.0 -bright false -noScienceStar false -outputFormat 2013.7" > sclwsTestBand_${BAND}_FAINT_${1}.log
done

sclwsTestServer "-minMagRange 10 -mag 12.9 -objectName AzV_266 -wlen 2.2 -band K -bright false -baseMax 102.45 -ra 01:01:09.42 -dec -72:27:28.40 -maxMagRange 13 -outputFormat 2013.7" > sclwsTestBand_K_FAINT_AzV266_${1}.log

#BIG (perfs):
sclwsTestServer "-objectName HD 32617 -mag 8.9 -diffRa 3600.0 -diffDec 1200.0 -band V -minMagRange 6.9 -maxMagRange 10.9 -ra 05 01 51.8518 -dec -50 16 34.052 -baseMax 102.45 -wlen 0.55 -bright true -noScienceStar false -outputFormat 2013.7" > sclwsTestBand_V_BIG_${1}.log


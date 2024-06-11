#!/bin/bash
source env.sh

sclwsTestServer "-objectName HD 32617 -mag 8.9 -diffRa 3600.0 -diffDec 1200.0 -band V -minMagRange 6.9 -maxMagRange 10.9 -ra 05 01 51.8518 -dec -50 16 34.052 -baseMax 102.45 -wlen 0.55 -bright true -noScienceStar false -outputFormat 2013.7" > sclwsTestBand_V_BIG_$1.log


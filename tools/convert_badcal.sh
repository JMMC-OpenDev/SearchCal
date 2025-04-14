#!/bin/bash

set -e;

IN=Badcal_SubmitCone.csv
OUT=badcal_list.tmp

curl -o $IN "http://apps.jmmc.fr/badcal-dsa/SubmitCone?DSACATTAB=badcal.valid_stars&RA=0&DEC=0&SR=180&Format=Comma-Separated"

# UCD then IDS
echo "ID_MAIN,POS_EQ_RA_MAIN_DEG,POS_EQ_DEC_MAIN_DEG,POS_EQ_RA_MAIN,POS_EQ_DEC_MAIN,NAME" > $OUT
echo "id,ra,dec,ra_sexa,dec_sexa,name" >> $OUT

tail +6 $IN >> $OUT

sed -i "s/,/\t/g" $OUT

# atomic update:
mv $OUT badcal_list.dat

rm $IN

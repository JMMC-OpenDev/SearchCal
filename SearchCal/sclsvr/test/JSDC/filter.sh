#!/bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

CURRENT=`pwd`

cd ../../../sclcat/bin/
./sclcatFilterCatalog $CURRENT/catalog/ $1

cp $CURRENT/catalog/result/fi* $CURRENT/
cp $CURRENT/catalog/result/shortDesc.txt $CURRENT/

rm $CURRENT/catalog/result/*.fits
rm $CURRENT/catalog/result/badcal.vot
rm $CURRENT/catalog/result/fi*
rm $CURRENT/catalog/result/shortDesc.txt


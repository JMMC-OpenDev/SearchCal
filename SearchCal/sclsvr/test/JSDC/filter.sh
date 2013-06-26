#!/bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

CURRENT=`pwd`

cd ../../../sclcat/bin/
./sclcatFilterCatalog $CURRENT/catalog/ $1

rm $CURRENT/catalog/result/catalog*.fits

cp $CURRENT/catalog/result/final.fits $CURRENT/final.fits


#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# bright or faint JSDC scenario:
export BRIGHT=true

export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"
#export VOBS_VIZIER_URI="http://viz-beta.u-strasbg.fr"

# define dev flag for development features (noticeably: use of internal database in MCSDATA):
export VOBS_DEV_FLAG="true"
# disable dev flag
export VOBS_LOW_MEM_FLAG="false"

# Where is MCSDATA:
#export MCSDATA=$HOME/MCSDATA

export http_proxy=
export https_proxy=

# Accept an optionnal argument to change output filename : jsdc${suffix}.vot
if [ -n "$1" ]
then
    suffix="$1"
fi

JSDC_FILE=jsdc${suffix}.vot

#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1

#./monitor.sh sclsvrServer &> monitor${suffix}.log &
# Remember monitor PID for later kill
#MON_PID=$!
#echo "monitor started: $MON_PID"

echo "Running sclsvr to produce the catalog '$JSDC_FILE' ..."
# use dec=+90:00 to get catalog sorted by distance to north pole
sclsvrServer -v 3 GETCAL "-wlen 2.2 -minMagRange -5.0 -file $JSDC_FILE -objectName JSDC_V3 -ra +00:00:00.000 -dec +90:00:00.000 -noScienceStar false -band 0 -bright $BRIGHT -diffRa 3600 -diffDec 1200 -baseMax 999.0 -maxMagRange 20.0 -mag 5 -outputFormat 2013.7" &> runJSDC${suffix}.log

# kill monitor
#echo -n "monitor stopping ..."
#kill $MON_PID

echo "JSDC done."
echo "to compare outputs, remove version dependent parts with, e.g:"
echo "cat runJSDC.log |grep -v Info |sed -e 's%- "`date +%F`"T[0123456789\:\.]*%%g' |sed -e 's%\.c.*\:[0-9]*%%g' > file_to_be_compared"

# --- EoF ---

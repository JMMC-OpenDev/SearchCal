#!/bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# bright or faint JSDC scenario:
export BRIGHT=false

export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"
#export VOBS_VIZIER_URI="http://viz-beta.u-strasbg.fr"
# define dev flag for development features (noticeably: use of internal database in MCSDATA):
export VOBS_DEV_FLAG="true"
#where is MCSDATA:
export MCSDATA=$HOME/MCSDATA

export http_proxy=
export https_proxy=

# Accept an optionnal argument to change output filename : jsdc${suffix}.vot
if [ -n "$1" ]
then
    suffix="$1"
fi
#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1

./monitor.sh sclsvrServer &> monitor${suffix}.log &

# Remember monitor PID for later kill
MON_PID=$!
echo "monitor started: $MON_PID"

# use dec=+90:00 to get catalog sorted by distance to north pole
sclsvrServer -v 3 GETCAL "-wlen 2.2 -minMagRange -5.0 -file jsdc${suffix}.vot -objectName JSDC_V2 -diffRa 3600 -ra +00:00:00.000 -noScienceStar false -band 0 -bright $BRIGHT -diffDec 1200 -baseMax 102.45 -maxMagRange 20.0 -mag 6 -dec +90:00:00.000 -outputFormat 2013.7" &> runJSDC${suffix}.log

# kill monitor
echo -n "monitor stopping ..."
kill $MON_PID 
echo "JSDC done."
echo "to compare outputs, remove version dependent parts with, e.g:"
echo "cat runJSDC.log |grep -v Info |sed -e 's%- "`date +%F`"T[0123456789\:\.]*%%g' |sed -e 's%\.c.*\:[0-9]*%%g' > file_to_be_compared"

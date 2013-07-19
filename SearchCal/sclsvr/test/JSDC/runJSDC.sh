#!/bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"
# define dev flag:
export VOBS_DEV_FLAG="true"

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
sclsvrServer -v 3 GETCAL "-wlen 2.2 -minMagRange -5.0 -file jsdc${suffix}.vot -objectName toto -diffRa 3600 -ra +00:00:00.000 -noScienceStar false -band 0 -bright true -diffDec 1200 -baseMax 102.45 -maxMagRange 20.0 -mag 6 -dec +90:00:00.000 -outputFormat 2013.7" &> runJSDC${suffix}.log

# kill monitor
echo -n "monitor stopping ..."
kill $MON_PID 
echo "JSDC done."

#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"

export http_proxy=
export https_proxy=

#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1

./monitor.sh sclsvrServer &> monitor.log &

sclsvrServer -v 3 GETCAL "-wlen 2.2 -minMagRange -5.0 -file jsdc.vot -objectName toto -diffRa 3600 -ra +00:00:00.000 -noScienceStar false -band 0 -bright true -diffDec 1200 -baseMax 102.45 -maxMagRange 20.0 -mag 6 -dec +00:00:00.000 -visErr 0.5" &> runJSDC.log &
#-diffDec 1200 -minMagRange 0.5 -mag 0.83 -objectName alf_PsA -diffRa 3600 -wlen 2.2 -band 0 -baseMax 102.45 -maxMagRange 2 -file jsdc.vot -dec -29:37:20.050 -ra 22:57:39.047 -bright true -noScienceStar false" &> runJSDC.log &


#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"

export http_proxy=
export https_proxy=

export G_SLICE=always-malloc
export GLIBCXX_FORCE_NEW=1

./monitor.sh sclsvrServer &> monitor.log &

sclsvrServer -v 3 GETCAL "-diffDec 1200 -minMagRange 0.5 -mag 0.83 -objectName alf_PsA -diffRa 3600 -wlen 2.2 -band 0 -baseMax 102.45 -maxMagRange 2 -file jsdc.vot -dec -29:37:20.050 -ra 22:57:39.047" &> runJSDC.log &


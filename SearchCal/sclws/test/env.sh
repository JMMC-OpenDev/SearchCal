#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
export SCLWS_PORT_NB=6666
export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"
export VERBOSITY=3

export http_proxy=
export https_proxy=

export SCLWS_CMD="sclwsServer -v $VERBOSITY"

echo "SCLWS CMD  = $SCLWS_CMD"
echo "SCLWS PORT = $SCLWS_PORT_NB"

export VG_PATH=/usr/bin/
#export VG_PATH=/home/users/bourgesl/apps/valgrind/bin
export TSAN_PATH=/home/users/bourgesl/apps/tsan/

echo "valgrind path = $VG_PATH"
echo "tsan path     = $TSAN_PATH"

#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1


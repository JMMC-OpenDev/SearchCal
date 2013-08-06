#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
export SCLWS_PORT_NB=6666
export VOBS_VIZIER_URI="http://vizier.u-strasbg.fr"
export VERBOSITY=3

# define dev flag:
export VOBS_DEV_FLAG="false"

export http_proxy=
export https_proxy=

export SCLWS_CMD="sclwsServer -l 0 -v $VERBOSITY"

echo "SCLWS CMD  = $SCLWS_CMD"
echo "SCLWS PORT = $SCLWS_PORT_NB"
echo "VOBS_DEV_FLAG = $VOBS_DEV_FLAG"

export VG_PATH=/usr/bin/
#export VG_PATH=/home/users/bourgesl/apps/valgrind/bin
export TSAN_PATH=/home/users/bourgesl/apps/tsan/

echo "valgrind path = $VG_PATH"
echo "tsan path     = $TSAN_PATH"

#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1


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
echo "verbosity  = $VERBOSITY"

#valgrind software installation directory:
export VG_PATH=/usr/bin/
#export VG_PATH=/home/users/bourgesl/apps/valgrind/bin

echo "valgrind path = $VG_PATH"

export G_SLICE=always-malloc
export GLIBCPP_FORCE_NEW=1
export GLIBCXX_FORCE_NEW=1


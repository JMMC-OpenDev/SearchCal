#!/bin/bash
#export GDL_PATH="/home/bourgesl/apps/astron/pro/:/usr/share/gnudatalanguage/lib/"

#export GDL_PATH="/home/bourgesl/apps/astron/pro/:/home/bourgesl/dev/gdl/src/pro/:/home/bourgesl/dev/gdl/src/pro/CMprocedures/"

DIR=`pwd`

export GDL_PATH="$DIR:$DIR/pro/"
echo "GDL_PATH: $GDL_PATH"

#export PATH="/home/bourgesl/dev/gdl/build/src/:$PATH"
which gdl


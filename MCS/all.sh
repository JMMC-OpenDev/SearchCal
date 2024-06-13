#!/bin/bash

echo "BUILDING MODULES ..."
for i in mcs log err misc misco env cmd thrd evh msg sdb timlog; do (cd $i/src; make clean all install); done
echo "BUILD DONE"


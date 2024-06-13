#!/bin/bash

echo "BUILDING MODULES ..."
for i in alx vobs simcli sclsvr sclws; do (cd $i/src; make clean all install); done
echo "BUILD DONE"

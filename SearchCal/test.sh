#!/bin/bash

echo "BUILDING TESTS ..."
for i in alx vobs simcli sclsvr sclws; do (cd $i/test; make clean all install); done
echo "TESTS DONE"


#!/bin/bash

echo "BUILDING TESTS ..."
for i in mcs log err misc misco env cmd thrd evh msg sdb timlog; do (cd $i/test; make clean all install); done
echo "TESTS DONE"


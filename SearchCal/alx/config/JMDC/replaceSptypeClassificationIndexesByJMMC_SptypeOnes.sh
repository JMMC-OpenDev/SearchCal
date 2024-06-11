#!/bin/bash
if test $# -eq 0
then
        echo usage: $0 fits_in fits_out
        exit 1
fi

stilts tpipe ifmt=fits in=$1 \
 cmd='keepcols "SPTYPE_JMMC"' \
 omode=out ofmt=ascii out="tmp.ascii"

alxDecodeSpectralType -i tmp.ascii -o tmp.tst

stilts tjoin nin=2 ifmt1=fits in1=$1 ifmt2=tst in2=tmp.tst \
ofmt=fits out=$2 \
icmd1='delcols "color_table_index color_table_delta lum_class lum_class_delta"' \
fixcols="dups" 

\rm tmp.ascii tmp.tst 


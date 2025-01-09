#!/bin/bash

DIR=/home/bourgesl/MCS/data/tmp

# candidates = _1 (primary query):
# Search_JSDC_BRIGHT_1_K_I_280B_1.log
# Search_JSDC_FAINT_1_K_I_280B_1.log

rm $DIR/Search_JSDC_BRIGHT_1_K_I_280B_1.log
rm $DIR/Search_JSDC_FAINT_1_K_I_280B_1.log


# Merge BRIGHT+FAINT catalogs:
BR=$DIR/BRIGHT
FA=$DIR/FAINT

# common catalogs: I_280 II_246_out I_355_gaiadr3 I_355_paramp II_328_allwise B_sb9_main B_wds_wds

# I_280:
B=Search_JSDC_BRIGHT_2_K_I_280_2.log
F=Search_JSDC_FAINT_2_K_I_280_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F


# II_246_out:
B=Search_JSDC_BRIGHT_5_K_II_246_out_2.log
F=Search_JSDC_FAINT_3_K_II_246_out_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F


# I_355_gaiadr3:
B=Search_JSDC_BRIGHT_6_K_I_355_gaiadr3_2.log
F=Search_JSDC_FAINT_4_K_I_355_gaiadr3_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F

# I_355_paramp:
B=Search_JSDC_BRIGHT_7_K_I_355_paramp_2.log
F=Search_JSDC_FAINT_5_K_I_355_paramp_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F


# II_328_allwise:
B=Search_JSDC_BRIGHT_8_K_II_328_allwise_2.log
F=Search_JSDC_FAINT_6_K_II_328_allwise_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F


# B_sb9_main:
B=Search_JSDC_BRIGHT_13_K_B_sb9_main_2.log
F=Search_JSDC_FAINT_7_K_B_sb9_main_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F


# B_wds_wds:
B=Search_JSDC_BRIGHT_14_K_B_wds_wds_2.log
F=Search_JSDC_FAINT_8_K_B_wds_wds_2.log
cp  $BR/$B           $DIR/$B
echo '\n'         >> $DIR/$B
tail -n +3 $FA/$F >> $DIR/$B
rm $DIR/$F
ln -s $DIR/$B        $DIR/$F

echo "That's All,folks !!!"


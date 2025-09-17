


Get last JMDC:
CSV_FILE=JMDC-$(date +"%F").csv
$ curl https://jmdc.jmmc.fr/export_csv -o ${CSV_FILE}

Run scriptHandleLastVersionOf_JMDC_dot_xls_ToProduceJMDC_dot_fits.sh
$ scriptHandleLastVersionOf_JMDC_dot_xls_ToProduceJMDC_dot_fits ${CSV_FILE}

OpenOffice conventions:
- use Language=EN for numerical columns (format cells - Number format)
- trim: replace all '^[ ]+' by ''

Run script:
source env.sh (astro lib + gdl runtime with mpfit)
    export GDL_PATH="/home/bourgesl/apps/astron/pro/:/home/bourgesl/dev/gdl/src/pro/:/home/bourgesl/dev/gdl/src/pro/CMprocedures/"

 gdl -e "make_jsdc_script_simple,\"JMDC_full_20200730_final_lddUpdated.fits\",verbose=1,nocatalog=1"

 gdl -e "make_jsdc_script_simple,\"JMDC_full_20200730_final_lddUpdated.fits\",\"catalog2020.fits\",verbose=1"


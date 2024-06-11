# How to test a new beta version of ViezieR ?

* install SearchCal server
* cd in ../sclsvr/test/
* run prod and beta test files
    * TS=$(date +"%d%b%Y")
    * cmdBatch -f Prod$TS sclsvrTestBatch.config
    * VOBS_VIZIER_URI="http://viz-beta.u-strasbg.fr" cmdBatch -f Beta$TS sclsvrTestBatch.config
* compare results 
    * meld Prod$TS Beta$TS

Lot of urls change in log files but content should be very similar.
Give positive and negative feedback to cds team.




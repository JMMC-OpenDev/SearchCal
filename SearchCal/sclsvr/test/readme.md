# How to test a new beta version of ViezieR ?

* install SearchCal server
* cd in ../sclsvr/test/
* run prod and beta test files
    * cmdBatch -f Prod15Sept sclsvrTestBatch.config
    * VOBS_VIZIER_URI="http://viz-beta.u-strasbg.fr" cmdBatch -f Beta15Sept sclsvrTestBatch.config
* compare results 
    * meld Prod15Sept Beta15Sept

Lot of urls change in log files but content should be very similar.
Give positive and negative feedback to cds team.




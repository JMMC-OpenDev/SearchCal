# SearchCal
This public repository gathers the source code for JMMC SearchCal, JMDC &amp; JSDC processing &amp; MCS env

License
=======
Public code and Open data under GPL3 license:
"
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007
"
See LICENSE


How-to
======

Please have a look to the Makefile for using the SearchCal docker image.

If you want your SearchCal GUI to query the server running on your machine :
- edit you `~/.fr.jmmc.searchcal.properties` preference file
- define the `server.url.address` entry to `http://localhost:8079`

Get a VOTABLE using GETSTAR:
- `touch out.vot; docker run -v $PWD/out.vot:/out.vot searchcal:6.0.0 sclsvrServer -noDate -noFileLine -v 3  GETSTAR "-objectName HD1234 -file /out.vot"`

SearchCal server starts looking at `$MCSDATA` to find cached data. 
You can speedup the SearchCal server saving VizieR queries with `make run_vol`. 

You must get next directory structure and files:
```
/data/VOL_SCL/
└── GetCal
    ├── SearchListBackup_JSDC_BRIGHT.dat
    └── SearchListBackup_JSDC_FAINT.dat
└── GetStar
└── metadata
```

=EoF=


[![JMMC logo](http://www.jmmc.fr/images/logo.png)](http://www.jmmc.fr)


## JSDC3 Change log & release notes


Date: July 2025

Authors:
- Laurent Bourgès — JMMC / OSUG

Contributors:
- Guillaume Mella — JMMC / OSUG
- Sylvain Lafrasse — CNRS / LAPP (until 2017)
- Alain Chelli — JMMC / Lagrange (OCA)
- Gilles Duvert — JMMC / IPAG (until 2022)
- Jean-Philippe Berger — JMMC / IPAG
- Pierre Cruzalèbes — JMMC / Lagrange (OCA)


Reference:
- [TBD]


> [!NOTE]
> **Updates in progress**



### Description
This document will give release notes and change logs on the JSDC-3 catalog, SearchCal & GetStar 6.0 releases, compared to the previous JSDC-2 & SearchCal/GetStar 5.0 releases.

Expliquer BRIGHT / FAINT au plus tôt.


### Building steps for the JSDC catalog

Several steps:
- preparing candidates (SIMBAD x ASCC = TYCHO2)
	=> candidates for BRIGHT (with SPTYPE) and FAINT (no SPTYPE)

- querying vizier catalogs (large compilation)
	BRIGHT & FAINT scenarios

- computation steps:
	- parse SPTYPE (O to M) => indices: color_table_index / delta + lum_class / delta
	- Teff / logG (from SPTYPE)
	- angular diameters
		- using polynoms (V-J V-H V-K) vs (color_table_index) + correlation matrix => diam_vj/vh/vk + error
		- weighted mean diameter => LDD / e_LDD and chi2_diam
		- LDD to UD conversion (tables neilson)
	- define flags (CalFlag)
	
Note: la méthode n'utilise pas extinction, teff, logg, ni plx / distance = approche statistique basée sur photométries
	

### Updated JSDC candidates
explain: (SIMBAD x ASCC x MDFC) for 2.5e6 stars
= using CDS xmatch files + stilts script

Prepare candidate lists for both BRIGHT (with SPTYPE) and FAINT (no SPTYPE)

keep GroupSize (3as) to mark stars with other stars in the field (3as ~ 2MASS resolution)

note: GroupSize = 1 means Simbad xmatch inconsitency (1991.25 vs 2000) ie proper motions inconsistent

### Scenarios / Used catalogs:

make JSDC3 diagrams

SIMBAD: (V_SIMBAD, sptype, object type)
ASCC: tycho2 compilation (V)
(HIP)
2MASS: IR (J H Ks)
allWise: mid IR (LMN)
SBC9 / WDS: non-standard stars + ASCC variability / multiplicity flags

TODO: describe MDFC / GAIA DR3 selected columns

BADCAL: only available in full catalog (live badcal catalog) for SearchCal & GetStar 6

TODO: generate JSDC3 stable row identifier (based on ASCC_ID ?)


### Updated xmatch algorithm
describe proper-motion handling (epoch 1991.25 vs 2000 vs 2016)

New:
- best match among ref (symetric) not AllFrom1
	=> XMATCH_LOG + XMATCH_FLAGS + columns (sep, n_mates, sep_2nd) for (ASCC, HIP, 2MASS, WISE, GAIA)

=> symetric (best) no more duplicates in SIMBAD + ASCC

- detailler critères (distances, HD, V mag) sous la forme d'une table

- fixed xmatch radius (effective resolution)
- for GAIA: use radius (max 1.5 as) + distance (V - V_gaia from G) to ensure GAIA candidates are compatible in V range (5 sigma ~ 2.5 mag max si e_V = 0.5 max) 
	V_gaia from (G, Bp Rp) ou (G, H, J, K) => V_gaia + e_V
	
	distance criteria = (V_GAIA - V) / 5 * max (0.1, e_V, e_V_gaia)
		=> adjust max distance < 1.0 !!
		
		sqrt(e_v*e_v+e_V_GAIA*e_V_GAIA)
		
		(V- V_gaia)/max(0.05,min(max(e_v, e_v_gaia),0.3))
		
	problem: 2025.07: XM_n_mates_GAIA counts only matching entries (distance + magnitude matching) !
			-> use only distance (but potentially many faint candidates in 3 as)
			=> use relaxed magnitude criteria to get brighter and fainter but not all targets:
				ie V_gaia < V_ref + 3 (signed) ?
		
TODO: GroupSize = max (groupSize(ASCC, SIMBAD) + xmatch n_mates) 
	=> flag targets if GroupSize > 2 (3as) to report potential bad IR flux (2MASS)
	reuse CalFlag (new XMATCH flag = 8)  or new flag ?


### Method update on stellar diameter computation:
TODO: describe variants:
  - bright (VJHK + SPTYPE)
  - faint (VJHK + NO SPTYPE) => best(chi2) => use min_chi2 + 1/2 to adjust diam (middle) + error (distance to mid) 
  	=> adjust SPTYPE_JMMC (color_table_index_fix / table_delta_color_fix)
  	expliquer l'approche 'grossière'

    => pas terrible: chi2 is flat (below 1) => use mid diameter (min + max) / 2 and err=(max - min) / 2  in log10 !
    
    - exemple: illustrer le cas = limite de l'approche (ex comparaison bright vs force-faint)

  - future: new fainter (G =>V + JHK +/- SPTYPE) ? : lower confidence => diameter rough estimation (used by getstar6 if no V tycho)
  	-> dicussion point

Reminder: JSDC V2 / V3 use the same polynoms (V-J V-H V-K) from JMDC fit (2017) valid in range [42;272] ie [B0.5-M8]
	

### Updated flags:
new flag in CalFlag (new XMATCH flag = 8) + added IR flag (from MDFC)

CalFlag bit0: set if 0 < color_table_delta < 20
_9	gg	454613	85% 	CalFlag == 0	

GroupSize = Simbad xmatch inconsitency (1991.25 vs 2000) = 1, then max(n_mates (ASCC,HIP,2MASS,WISE,GAIA))
_9	gg	23989	4% 	GroupSize >= 1	

=> TODO: new flag in CalFlag or reuse XMATCH flag ?

reminder IRFlag (MDFC)
describe BadCal (update)


## Other changes:
### Teff / logg:
	-> bright: from sptype (direct), not from guessed color_table_index_fix
	faint: based on guessed (center) SP-TYPE (color_table_index_fix)

- do not compute Av => do not compute missing mags R & I

### SearchCal & GetStar 6 releases

TODO


### Data summary & statistics

#### Summary

~ 500k bright
~ 2m faint

couverture du ciel + histogramme separation min

#### Flavors

- small (few columns)
	add few GAIA, MDFC columns ?
	
- full (all columns + origin / confidence columns)


#### Statistics

BRIGHT:

?? gaia entries
170 badcal entries

- crossmatch avec JMDC (new) ? 2183 rows

### JSDC comparison (v2 vs v3)
TODO: Lots of nice (stilts) Plots

- xmatch differences:
  - 10000 types spectraux différents (V2 / V3)
  ~ 1000 (V-J-H-K) mag différentes (crossmatch)


- LDD_v3 vs v2: bosse à -2 sigma => changement sur gestion color_table_delta
	? error max = 0.15 vs 0.25 (faint) or chi2 threshold = 1 + 1 / (nbdiam - 1) or nbdiam ?

- note: V mag from ASCC (tycho2) not HIP2 => small difference on LDD

### JSDC3 vs GAIA DR3
comparer diametres radius(Rsun) -> angular diam (mas)
- gsp_phot
- flame (TODO)

? DR4 ?

### Conclusion


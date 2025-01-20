[![JMMC logo](http://www.jmmc.fr/images/logo.png)](http://www.jmmc.fr)


## JSDC3 Change log & release notes


Date: Mon 20th January 2025

Authors:
- Laurent Bourgès — JMMC / OSUG

Contributors:
- Guillaume Mella — JMMC / OSUG
- Sylvain Lafrasse — CNRS / LAPP (until 2017)
- Gilles Duvert — JMMC / IPAG (until 2022)
- Alain Chelli — JMMC / Lagrange (OCA)
- Jean-Philippe Berger — JMMC / IPAG
- Pierre Cruzalèbes — JMMC / Lagrange (OCA)


Reference:
- [TBD]


> [!NOTE]
> **Documentation updates in progress**



### Description
This document will give release notes and change logs on the JSDC-3 catalog, SearchCal & GetStar 6.0 releases, compared to the previous JSDC-2 & SearchCal/GetStar 5.0 releases.

### Used catalogs

TODO: describe MDFC / GAIA DR3 selected columns


### JSDC Scenarios
TODO: with JSDC2 & 3 diagrams

### Updated JSDC candidates
TODO: SIMBAD x ASCC x MDFC) for 2.5e6 stars

### Updated xmatch algorithm
TODO: pm + gaia V

### Method update on stellar diameter computation:
TODO: describe variants:
  - bright (VJHK + SPTYPE)
  - faint (VJHK + NO SPTYPE) => best(chi2) + guess SPTYPE_JMMC
  - new fainter (G->V JHK +/- SPTYPE): lower confidence => diameter rough estimation

### Updated flags:
TODO: CalFlag (new XM flag) + IR flag + scaled chi2 by scale(jmdc / scl_chi2) = 5 (only good if < 5)

### Data summary & statistics

#### Summary & statistics

#### Statistics

### JSDC comparison (2 vs 3)
TODO: Lots of nice (stilts) Plots + compare with GAIA radius vs LDD_jsdc

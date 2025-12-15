II/346      JMMC Stellar Diameters Catalogue - JSDC. Version 2  (Bourges+, 2017)
================================================================================
An all-sky catalogue of computed star diameters.
    Bourges L., Mella G., Lafrasse S., Duvert G., Chelli A., Le Bouquin J.-B.,
    Delfosse X., Chesneau O.
   <ASP Conference Series, Vol. 485, p.223 (2014)>
   =2014ASPC..485..223B
   =2017yCat.2346....0B
================================================================================
ADC_Keywords: Stars, standard ; Interferometry ; Fundamental catalog ;
              Photometry, infrared ; Parallaxes, trigonometric; Stars, diameters
Keywords: stars: fundamental parameters - techniques: data analysis -
          techniques: interferometric - astronomical database: miscellaneous -
          catalogs

Abstract:
    This catalogue contains stellar angular diameter estimates for nearly
    all the stars of the Hipparcos and Tycho catalogue that have an
    associated spectral type in Simbad/CDS. The median error on the
    diameters is around 1.5%, with possible biases of around ~2%. For each
    object, the limb-darkened diameter retained is the mean value of
    several estimates performed using different couples of photometries.
    The chi-square representing the dispersion between these values is
    also given (it is below 2 for ~400000 stars). An additional flag
    signals stars that could represent a risk if chosen as calibrators for
    Optical Long-Baseline Interferometry, independently of the correctness
    of their apparent diameter estimate.

    This catalog replaces the catalog II/300/jsdc .

Description:
    The JMMC (Jean-Marie Mariotti Center) Calibrator Workgroup has long
    developed methods to estimate the angular diameter of stars, and
    provides this expertise in the SearchCal tool
    (http://www.jmmc.fr/searchcal). SearchCal creates a dynamical
    catalogue of stars suitable to calibrate Optical Long-Baseline
    Interferometry (OLBI) observations from on-line queries of CDS
    catalogues, according to observational parameters. In essence,
    SearchCal is limited only by the completeness of the stellar
    catalogues it uses, and in particular is not limited in magnitude.
    SearchCal being an application centered on the somewhat restricted
    OLBI observational purposes, it appeared useful to make our angular
    diameter estimates available for other purposes through a CDS-based
    catalog, the JMMC Stellar Diameters Catalogue (JSDC, II/300).

    This second version of the catalog represents a tenfold improvement
    both in terms of the number of objects and on the precision of the
    estimates. This is due to a new algorithm using reddening-free
    quantities -- the pseudomagnitudes, allied to a new database of all the
    measured stellar angular diameters -- the JMDC (II/345/jmdc), and a
    rigorous error propagation at all steps of the processing. All this is
    described in the associated publication by Chelli et al.
    (2016A&A...589A.112C).

    The catalog reports the Limb-Darkened Diameter (LDD) and error for
    465877 stars, as well as their BVRIJHKLMN magnitudes, Uniform Disk
    Diameters (UDD) in these same photometric bands, Spectral Type, and
    two supplementary quality indicators:

    - the mean-diameter chi-square (see Appendix A.2 of Chelli et al.,
    2016A&A...589A.112C).

    - a flag indicating some degree of caution in choosing this star as an
    OLBI calibrator: known spectroscopic binaries, Algol type stars, etc,
    see Note (1).

    The conversion from LDD to UDD in each spectral band is made using
    mainly the coefficients from J/A+A/556/A86/table16 and
    J/A+A/554/A98/table16 when possible (compatible spectral types) and
    following the prescriptions of the JMMC report at
    http://www.mariotti.fr/doc/approved/JMMC-MEM-2610-0001.pdf in all
    other cases. The errors on UDD values are omitted as they are similar
    to the LDD error.

    Instead of using this catalog to find a suitable OLBI calibrator, the
    reader is invited to use the SearchCal tool at JMMC
    (http://www.jmmc.fr/searchcal) which permits a refined search, give
    access to other possible calibrators (faint stars not in the Tycho
    catalog) and benefits from the maintainance of JMMC and CDS databases.

    This catalog replaces the previous JSDC (II/300/jsdc). Almost all
    stars in II/300/jsdc are found in II/346 with a consistent diameter,
    with the exception of 1935 stars whose estimated diameter differs from
    more than 2 sigmas between the two catalogs. The associated file
    JSDC_v2_v1_dis.vot (jsdc_dis.dat) summarizes this
    difference.

File Summary:
--------------------------------------------------------------------------------
 FileName         Lrecl  Records   Explanations
--------------------------------------------------------------------------------
ReadMe               80        .   This file
jsdc_v2.dat         367   465877   Stellar diameters catalogue, version 2
jsdc_v2.fits       2880    33329   Fits version of the catalog
jsdc_dis.dat         93     1917   Data for stars having more than 2 sigma
                                    difference between JSDC 1 and 2
jsdc_v2_v1_dis.vot  166    21142   VOTable version of jsdc_dis.dat
figs/*               .         4   Figures of 3 diagrams of Chelli et al.,
                                    2016A&A...589A.112C, Cat. II/345 and 
                                    comparison of JSDC diameters with those of 
                                    Swihart et al., 2017,  J/AJ/153/16/table3
--------------------------------------------------------------------------------

See also:
   II/224 : Catalogue of Stellar Diameters (CADARS) (Pasinetti-Fracassini+ 2001)
   II/300 : JMMC Stellar Diameters Catalogue - JSDC (Lafrasse+, 2010)

   http://www.jmmc.fr/jsdc : JSDC home page
    http://www.jmmc.fr/getstar : GetStar utility page

Byte-by-byte Description of file: jsdc_v2.dat
--------------------------------------------------------------------------------
   Bytes Format Units   Label     Explanations
--------------------------------------------------------------------------------
   1- 33  A33   ---     Name      Normalised star name (preferably HD)
  35- 36  I2    h       RAh       Right ascension (J2000)
  38- 39  I2    min     RAm       Right ascension (J2000)
  41- 48  F8.5  s       RAs       Right ascension (J2000)
      50  A1    ---     DE-       Declination sign (J2000)
  51- 52  I2    deg     DEd       Declination (J2000)
  54- 55  I2    arcmin  DEm       Declination (J2000)
  57- 63  F7.4  arcsec  DEs       Declination (J2000)
  65- 84  A20   ---     SpType    Spectral Type from Simbad
  86- 92  F7.3  mag     Bmag      ?=- B magnitude (Cat. I/259)
  94-101  F8.6  mag   e_Bmag      ?=- Error on Bmag (Cat. I/259)
 103-109  F7.3  mag     Vmag      ?=- V magnitude (Cat. I/259)
 111-115  F5.3  mag   e_Vmag      ?=- Error on Vmag (Cat. I/259)
 117-126  F10.6 mag     Rmag      ?=- R magnitude (estimated)
 128-137  F10.6 mag     Imag      ?=- I magnitude (estimated)
 139-145  F7.3  mag     Jmag      ?=- J magnitude (Cat. II/246)
 147-151  F5.3  mag   e_Jmag      ?=- Error on Jmag (Cat. II/246)
 153-159  F7.3  mag     Hmag      ?=- H magnitude (Cat. II/246)
 161-165  F5.3  mag   e_Hmag      ?=- Error on Hmag (Cat. II/246)
 167-173  F7.3  mag     Kmag      ?=- K magnitude (Cat. II/246)
 175-179  F5.3  mag   e_Kmag      ?=- Error on Kmag (Cat. II/246)
 181-187  F7.3  mag     Lmag      ?=- L magnitude (Cat. II/311/wise W1mag)
 189-193  F5.3  mag   e_Lmag      ?=- Error on Lmag (Cat. II/311/wise e_W1mag)
 195-200  F6.3  mag     Mmag      ?=- M magnitude (Cat. II/311/wise W2mag)
 202-206  F5.3  mag   e_Mmag      ?=- Error on Mmag (Cat. II/311/wise e_W2mag)
 208-217  F10.6 mag     Nmag      ?=- N magnitude (Cat. II/311/wise W3mag)
 219-223  F5.3  mag   e_Nmag      ?=- Error on Nmag (Cat. II/311/wise e_W3mag)
 225-233  F9.6  mas     LDD       ?=-1 Limb-Darkened Disk diameter
 235-242  F8.6  mas   e_LDD       ?=-1 Error on LDD (and all UDDs) (1)
 244-255  E12.6 ---     LDDCHI    Chi2 of LDD fitting (2)
     257  I1    ---     CalFlag   [0/7] Confidence flags for using this star as
                                   a calibrator in Optical Interferometry
                                   experiments (3)
 259-268  F10.6 mas     UDDB      ?=-1 Uniform Disk Diameter in band B (4)
 270-279  F10.6 mas     UDDV      ?=-1 Uniform Disk Diameter in band V (4)
 281-290  F10.6 mas     UDDR      ?=-1 Uniform Disk Diameter in band R (4)
 292-301  F10.6 mas     UDDI      ?=-1 Uniform Disk Diameter in band I (4)
 303-312  F10.6 mas     UDDJ      ?=-1 Uniform Disk Diameter in band J (4)
 314-323  F10.6 mas     UDDH      ?=-1 Uniform Disk Diameter in band H (4)
 325-334  F10.6 mas     UDDK      ?=-1 Uniform Disk Diameter in band K (4)
 336-345  F10.6 mas     UDDL      ?=-1 Uniform Disk Diameter in band L (4)
 347-356  F10.6 mas     UDDM      ?=-1 Uniform Disk Diameter in band M (4)
 358-367  F10.6 mas     UDDN      ?=-1 Uniform Disk Diameter in band N (4
--------------------------------------------------------------------------------
Note (1): This error takes into account a 2% error (added quadratically)
  added as a provision for the estimated biases.
Note (2): value of Mean-diameter chi-square as in Appendix A.2 of Chelli
 et al., 2016A&A...589A.112C.
Note (3): CalFlag is a bit field:
  bit zero is set if LDD_CHI is above 5;
  bit 1 is set if the star is a known double in WDS (Cat. B/wds/wds) with
  separation inferior to 1 arcsec;
  bit 2 is set if the star is, according to Simbad's OTYPEs,
  one of the following codes: "C*", "S*", "Pu*", "RR*", "Ce*", "dS*", "RV*",
  "WV*", "bC*", "cC*", "gD*", "SX*", "Mi*", "SB*", "Al*", which signals a
  possible binarity or pulsating stars.
  Although none of these bit flags prevent the LDD estimate to be accurate,
  they imply some caution in choosing this star as a calibrator star for
  Optical Long-Baseline Interferometry.
Note (4): The error on this value is equal to e_LDD. See Description for
  information about the conversion LDD to UDD.
--------------------------------------------------------------------------------

Byte-by-byte Description of file: jsdc_dis.dat
--------------------------------------------------------------------------------
   Bytes Format Units   Label     Explanations
--------------------------------------------------------------------------------
   1- 16  A16   ---     Name      HIP or Tycho name
  18- 19  I2    h       RAh       Right ascension (J2000)
  21- 22  I2    min     RAm       Right ascension (J2000)
  24- 29  F6.3  s       RAs       Right ascension (J2000)
      31  A1    ---     DE-       Declination sign (J2000)
  32- 33  I2    deg     DEd       Declination (J2000)
  35- 36  I2    arcmin  DEm       Declination (J2000)
  38- 42  F5.2  arcsec  DEs       Declination (J2000)
  44- 48  F5.3  ---     LDD1      Limb-Darkened Disk diameter of JSDC1
  50- 54  F5.3  ---   e_LDD1      rms uncertainty on LDD1
  57- 64  F8.6  ---     LDD2      Limb-Darkened Disk diameter of JSDC v2
  66- 73  F8.6  ---   e_LDD2      rms uncertainty on LDD2
  75- 82  F8.5  ---     LDDchi2   Reduced chi-square of the LDD diameter
                                    estimation
  84- 93  F10.7 ---     dLDD      Difference in sigma (LDD1-LDD2)/e_LDD1)
--------------------------------------------------------------------------------

Acknowledgements:
   Please cite the reference publication 2016A&A...589A.112C and use
   the following acknowledgement: "This research has made use of the
   Jean-Marie Mariotti Center JSDC catalog, available at
   http://www.jmmc.fr/jsdc"

   Gilles Duvert, gilles.duvert(at)univ-grenoble-alpes.fr

History:
   * 05-Jan-2017: First on-line version with "only" 467585 stars
   * 20-Jan-2017: Complete catalog with 471255 stars
   * 06-Mar-2017: New vesion with 465877 stars
   * 11-May-2017: JSDC_v2_v1_ discrepancies file added
   * 05-Jun-2018: figure swihart_vs_jmmc_data.png added

================================================================================
(End)   Gilles Duvert [IPAG, OSUG, France], Patricia Vannier [CDS]   02-Jan-2017

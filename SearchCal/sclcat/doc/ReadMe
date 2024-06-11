II/300              JMMC Stellar Diameters Catalogue - JSDC  (Lafrasse+, 2010)
================================================================================
An all-sky catalogue of computed star diameters using the SearchCal webservice.
    Lafrasse S., Mella G., Bonneau D., Duvert G., Delfosse X., Chelli A.
   <SPIE Conf. on Astronomical Instrumentation 77344E (2010)>
   =2010SPIE.7734E.140L
================================================================================
ADC_Keywords: Stars, standard ; Interferometry ; Fundamental catalog ;
              Photometry, infrared ; Parallaxes, trigonometric; Stars, diameters
Keywords: catalogs - stars: fundamental parameters -
          instrumentation: interferometers - techniques: interferometric

Abstract:
    This catalogue contains stellar angular diameter estimate for bright
    stars, complete for all stars with Hipparcos parallaxes.

    The JMMC Calibrator Workgroup has long developed methods to estimate
    the angular diameter of stars, and provides this expertise in the
    SearchCal software (http://www.jmmc.fr/searchcal). "SearchCal" creates
    a dynamical catalogue of stars suitable to calibrate Optical
    Long-Baseline Interferometry (OLBI) observations from on-line queries
    of CDS catalogues, according to observational parameters. In essence,
    SearchCal is limited only by the completeness of the stellar
    catalogues it uses, and in particular is not limited in magnitude.
    SearchCal being an application centered on OLBI peculiar purposes, it
    appeared useful to publish the estimated angular diameters of all
    stars with known parallaxes in a static catalogue.

    The present catalogue of stellar angular diameters has been obtained
    from an automated SearchCal results aggregation on the whole celestial
    sphere. For each star, the value of the limb-darkened angular
    diameters are computed using a surface brightness method and
    calibrations for (B-V), (V-R) and (V-K) color indexes.

    Stars whose angular diameters estimated from the various color indexes
    are not comparable, are rejected, and a reliable error on the
    estimated diameter is computed (1). For details of the method see
    Bonneau et al. (2006A&A...456..789B). To avoid specific confusion
    problems, spectroscopic binaries in the 9th Catalogue of Spectroscopic
    Binary Orbits (Pourbaix et al., 2009, Cat. B/sb9) or close visual
    binaries with a separation of less than 2 arc seconds in the
    Washington Visual Double Star Catalog (Mason et al., 2001, Cat. B/wds)
    have been filtered out.

Description:
    Limb Darkened and Uniform Angular diameters are given for 38472 stars,
    extracted from the All-sky Compiled Catalogue of 2.5 million stars
    (I/280, Kharchenko et al. 2009). The limb darkened diameters are
    computed using the V magnitude and the (V-K) color index.

    The uniform disk diameters are computed for the photometric bands (B,
    V, R, I, J, H, K) using the linear limb darkened coefficients from
    (Claret & Diaz-Cordoves, 1995, Cat. J/A+AS/114/247).

    For each star, HD and Hipparcos catalog numbers, equatorial
    coordinates, parallax, spectral type, photometric data (B,V,R,J,H,K)
    and angular diameters are given.

    Additionnally, the absence of a particular star in this catalogue can
    also be due to a missing photometry in one of the base catalogues.
    Conversely, the rejection of binaries depends on their availability as
    such in the relevant catalogues.

File Summary:
--------------------------------------------------------------------------------
 FileName   Lrecl  Records   Explanations
--------------------------------------------------------------------------------
ReadMe         80        .   This file
jsdc.dat      189    38472   Stellar diameters catalogue
--------------------------------------------------------------------------------

See also:
          B/wds : The Washington Visual Double Star Catalog (Mason+ 2001-2010)
          B/sb9 : 9th Cat. of Spectroscopic Binary Orbits (Pourbaix+ 2004-2009)
 II/224 : Catalogue of Stellar Diameters (CADARS) (Pasinetti-Fracassini+ 2001)
 J/A+AS/114/247 : Limb-darkening coefficients for R I J H K (Claret+, 1995)
 J/A+A/393/183  : Catalogue of calibrator stars for LBSI (Borde+, 2002)
 J/A+A/433/1155 : 948 bright calibrator stars for interferometry (Merand+, 2005)
 http://www.jmmc.fr/jsdc : JSDC home page

Byte-by-byte Description of file:jsdc.dat
--------------------------------------------------------------------------------
   Bytes Format Units   Label     Explanations
--------------------------------------------------------------------------------
   1- 16  A16   ---     Name      Name: "HIP" HIP number (Cat. I/311)
                                         or "TYC" TYCHO number (Cat. I/259/tyc2)
  17- 18  I2    h       RAh       Right Ascension J2000 (hours)
  20- 21  I2    min     RAm       Right Ascension J2000 (minutes)
  23- 28  F6.3  s       RAs       Right Ascension J2000 (seconds)
      29  A1    ---     DE-       Declination J2000 (sign)
  30- 31  I2    deg     DEd       Declination J2000 (degrees)
  33- 34  I2    arcmin  DEm       Declination J2000 (minutes)
  36- 40  F5.2  arcsec  DEs       Declination J2000 (seconds)
  41- 47  F7.2  mas/yr  pmRA      Proper motion in Right Ascension
  48- 54  F7.2  mas/yr  pmDE      Proper motion in Declination
  55- 60  F6.3  ---     Bmag      Johnson B magnitude (from ASCC-2.5, I/280)
  61- 66  F6.3  ---     Vmag      Johnson V magnitude (from ASCC-2.5, I/280)
  67- 72  F6.3  ---     Rmag      Johnson R magnitude (Cat. II/7A)
      73  I1    ---   f_Rmag      [0/1] Rmag is a measurement or estimation (1)
  74- 79  F6.3  ---     Imag      Johnson I magnitude (Cat. II/7)
      80  I1    ---   f_Imag      [0/1] Imag is a measurement or estimation (1)
  81- 86  F6.3  ---     Jmag      Johnson J magnitude (Cat. II/246, II/7)
  87- 92  F6.3  ---     Hmag      Johnson H magnitude (Cat. II/246, II/7)
  93- 98  F6.3  ---     Kmag      Johnson K magnitude (Cat. II/246, II/7)
  99-104  F6.3  mas     LDD       Limb-Darkened Diameter
 105-109  F5.3  mas   e_LDD       Error on Limb-Darkened Diameter (2)
 110-115  F6.3  mas     UDDB      Uniform diameter at B band
 116-121  F6.3  mas     UDDV      Uniform diameter at V band
 122-127  F6.3  mas     UDDR      Uniform diameter at R band
 128-133  F6.3  mas     UDDI      Uniform diameter at I band
 134-139  F6.3  mas     UDDJ      Uniform diameter at J band
 140-145  F6.3  mas     UDDH      Uniform diameter at H band
 146-151  F6.3  mas     UDDK      Uniform diameter at K band
 152-157  F6.2  mas     plx       Trigonometric parallax (from ASCC-2.5, I/280)
 158-162  F5.2  mas   e_plx       Error on Parallax (from ASCC-2.5, I/280)
 163-178  A16   ---     SpType    MK Spectral type (from ASCC-2.5, I/280)
 179-183  I5    K       Teff      Effective temperature adopted from SpType
 184-189  F6.3 [cm/s2]  logg      Surface gravity adopted from SpType
--------------------------------------------------------------------------------
Note (1): Magnitude flag as follows:
      1 = the corresponding magnitude is a measurement
      0 = the corresponding magnitude is an estimate based on the colors
          deduced from the spectral type. In this case, due to the
          discrete nature of the tables (SpType - color) used, a
          discretization appears in the diagrams using the colors (V-I)
          and (V-R)
Note (2): We provide only one error for all angular diameters, since the linear
          corrections between Uniform Disk diameters and the Limb-Darkened
          diameter lead to negligible corrections on the UDdiam errors.
--------------------------------------------------------------------------------

History:
    From Sylvain Lafrasse, Sylvain.Lafrasse(at)obs.ujf-grenoble.fr

Acknowledgements:
    This research has made use of the JSDC Jean-Marie Mariotti Center database,
    available at http://www.jmmc.fr/jsdc

    This research has made use of the VizieR catalogue access tool, CDS,
    Strasbourg, France.
    This research has made use of the SIMBAD database, operated at CDS,
    Strasbourg, France.

    This research has made use of the TOPCAT and STILTS software, provided by
    Mark Taylor of Bristol University, England available at
    http://www.starlink.ac.uk/topcat and http://www.starlink.ac.uk/stilts
================================================================================

II/345         JMDC : JMMC Measured Stellar Diameters Catalogue   (Duvert, 2016)
================================================================================
Compendium of Direct Measurement of Apparent Stellar Diameters.
(Initially introduced as part of Chelli et al., 2016A&A...589A.112C).
   Duvert G.
   <JMMC center (2016)>
   =2016yCat.2345....0D
================================================================================
ADC_Keywords: Stars, diameters
Keywords: stars: fundamental parameters - techniques: data analysis -
          techniques: interferometric - astronomical database: miscellaneous -
          catalogs

Abstract:
    This catalog lists all measurements of stellar apparent diameters made
    with "direct" techniques: optical interferometry, intensity
    interferometry and lunar occultations that have been published since
    the first experiments by Michelson.

Description:
    Several star diameter compilations exist that contain a fair amount of
    stellar angular diameter measurements. The CADARS (2011, Cat. II/224)
    has entries for 6888 stars and claims completeness up to 1997. CHARM2
    (2005, Cat. J/A+A/431/773) lists 8231 measurements of 3243 stars, up
    to 2005. However all these catalogs mix results from very direct
    methods, such as intensity interferometry, with indirect methods, or
    spectrophotometric estimates of various kind (always including some
    model of the star), or linear diameters from eclipsing binaries (1600
    entries in CADARS), which need some modelling of the two stars, as
    well as a good estimate of the distance to be converted into an
    angular diameter.

    In contrary, the present catalogue, called JMDC (for JMMC Measured
    stellar Diameters Catalog) is focussed on direct methods only, and
    selects only one value of the uniform-disk diameter (UDD) and/or
    limb-darkened diameter (LDD) for each historical measurement.

    Until 2020, this catalog has been updated manually. Now a web
    interface has been setup at JMMC (jmdc.jmmc.fr) to submit new
    measurements and browse the catalog. Please submit your new
    measurements! The CDS copy will be updated yearly.

    The current version (Q3 2021) gathers 2013 measurements that have been
    published since the first experiments by Michelson. Prior to 1997, our
    bibliography relies only on the reference list of CADARS, carefully
    reviewed. After this date we used NASA's ADS hosted at CDS. We
    retained only the measurements obtained from visible/IR
    interferometry, intensity interferometry and lunar occultation in the
    database. We always retrieved the values in the original text and used
    SIMBAD to properly and uniquely identify the stars.

    The three techniques retained share the same method of converting the
    measurements (squared visibilities for optical interferometry,
    correlation of photon-counts for intensity interferometry, fast
    photometry for lunar occultations) into an angular diameter: fitting a
    geometrical function into the values, in many cases a uniform disk,
    which provides a uniform disk diameter (UDD) value. This UDD is
    wavelength-dependent owing to the limb-darkening effect of the upper
    layers of a star's photosphere, and JMDC retains the wavelength or
    photometric band at which the observation was made. To measure a
    star's apparent diameter consistently, i.e., with the same meaning as
    our Sun's well-resolved apparent diameter, it was necessary for the
    authors of these measurements to take into account the star's
    limb-darkening, for which only theoretical estimates exist as yet.
    They chose one of the various limb-darkening parameters available in
    the literature, either by multiplying the UDD by a coefficient
    function of the wavelength and the star's adopted effective
    temperature, or directly fitting a limb-darkened disk model in the
    data. Of course this adds some amount of theoretical bias in the
    published measurements, which however diminishes as the wavelength
    increases. An additional difficulty for the lunar occultations is that
    the result depends on the exact geometry of the occulting portion of
    the lunar limb, which can, more or less, be correctly estimated.

    To deal with the limb-darkening problem as efficiently as possible, in
    the publications where reported diameters are measured in several
    optical/IR bands, we retain the measurement with the best accuracy and
    favor the measurement at the longest wavelength to minimize the effect
    of limb-darkening correction. When the publication include both LDD
    and UDD values, we report both, and, if available, the conversion
    coefficient used.

    We provide in the Notes additional information, such as the eventual
    binarity of the star, possible erroneous measurements, origin the of
    limb-darkening factor used, duplication with other publications etc...
    as weel as more "in-house" comments related to the proper use of this
    database, initially in the companion publication 2016A&A...589A.112C
    and now for the maintainance and improvement of the JMMC SearchCal
    tool.

    In the paper 2016A&A...589A.112C, we further use the published UDD
    measurement, or retrieve the original, unpublished UDD measurement
    from the LDD value and the limb-darkening coefficient used by the
    authors. We then convert these UDD values into limb-darkened angular
    diameters using mainly the coefficients from J/A+A/556/A86/table16 and
    J/A+A/554/A98/table16 when possible (compatible spectral types) and
    following the prescriptions of the JMMC report JMMC-MEM-2610-001
    (http://www.mariotti.fr/doc/approved/JMMC-MEM-2610-0001.pdf) in all
    other cases. As the limb-darkening coefficients depend on the
    effective temperature and surface gravity as well as some model of the
    stellar photosphere, these "revised" LDDs are not part of the present
    catalog.

File Summary:
--------------------------------------------------------------------------------
 FileName      Lrecl  Records   Explanations
--------------------------------------------------------------------------------
ReadMe            80        .   This file
jmdc.dat         335     2013   JMMC Measured stellar Diameters Catalog
                                 (as part of Chelli et al., 2016A&A...589A.112C)
jmdc.fits       2880      209   FITS version of the catalog
--------------------------------------------------------------------------------

See also:
        II/224 : Catalog of Stellar Diameters (CADARS)
                                                 (Pasinetti-Fracassini+ 2001)
 J/A+A/431/773 : CHARM2, an updated of CHARM catalog (Richichi+, 2005)
 J/A+A/556/A86 : FGK dwarf stars limb darkening coefficients (Neilson+, 2013)

Byte-by-byte Description of file: jmdc.dat
--------------------------------------------------------------------------------
   Bytes Format Units   Label     Explanations
--------------------------------------------------------------------------------
   1- 28  A28   ---     ID1       Normalised star name (preferably HD) (ID1)
  30- 57  A28   ---     ID2       Name used in the original publication (ID2)(1)
  59- 67  F9.5  mas     UDdiam    ?=- Uniform Disk Diameter (UD_MEAS)
  69- 75  F7.4  mas     LDdiam    ?=- Limb-Darkened Disk diameter (LD_MEAS)
  77- 84  F8.4  mas   e_LDdiam    ?=- Error on UDdiam and LDdiam (E_LD_MEAS) (2)
  86- 95  A10   ---     Band      Text describing the wavelength or band
                                   of the measurement (BAND) (3)
  97-103  F7.5  ---     mu-lambda ?=- When possible, value of the conversion
                                  UDD-to-LDD user by the author (MU_LAMBDA)
     105  I1    ---     Method    [1/3] Integer code of the method used
                                   (METHOD) (4)
 107-108  I2    ---     BandCode  ?=- Integer code of the band used
                                   (BANDCODE) (5)
 110-313  A204  ---     Notes     Note about the star and/or measurement (NOTES)
 315-333  A19   ---     BibCode   BibCode
     335  I1    ---     SINPE     ? boolean flag (6)
--------------------------------------------------------------------------------
Note (1): Provided the publication name is recognized by the Sesame name
  resolver.
Note (2): In general, quotes the published error on LDD or UDD, which are
 essentially equivalent. The blanking value of "---" can be due to an absence of
 published error, or has been fixed thus when the measurement is
 (retrospectively) in doubt, in which case an explanation lies in the Notes.
Note (3): A loosely defined string representing the band (UBVRIJHKLMNQ) or the
 central wavelength, in microns if not otherwise precised, of the observation.
 A stands for Angstroem, nm for nanometer.
Note (4): Code for the observational method used as follows:
    1 = optical interferometry
    2 = Lunar occultation
    3 = intensity interferometry
Note (5): Index from 1 (band U) and up through bands B,V,R,I,J,H,K,L,M,N,Q.
Note (6): Simbad identifiant is not precise enough for this object, usually a
    component of a close double star.
--------------------------------------------------------------------------------

History:
  * 28-Nov-2016: First version with 1239 measurements
  * 29-Mar-2018:   New version with 1478 measurements
  * 16-Jan-2019:   New version with 1554 measurements
  * 03-Feb-2020:   New version with 1672 measurements
  * 10-Sep-2021:   New version with 2013 measurements
  * 13-Sep-2021:   New version with 2013 measurements
  * Next release  :  
    - Fix HD187076J target id with HD187076, 
    - Fix 9 references with 1998A&A...331..619P instead of 1998AJ....116..981D
    

Acknowledgements:
    
    Gilles Duvert, gilles.duvert(at)univ-grenoble-alpes.fr

================================================================================
(End)  Gilles Duvert [IPAG, OSUG, France], Patricia Vannier [CDS]    17-Nov-2016

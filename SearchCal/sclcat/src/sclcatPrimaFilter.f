
      PROGRAM SCLCATPRIMAFILTER

      IMPLICIT NONE

      INTEGER DEG,MIN
      REAL*8 PMRA,PMDEC,DKAL,VITSKY,TIMESPAN,SEC,STARRA,STARDEC,DMLIM
      REAL*8 CALIBDEC,CALIBRA,STARPMRA,STARPMDEC,CALIBPMRA,CALIBPMDEC
      REAL*8 PMRAREL,PMDECREL,RAREL,DECREL,CD,D0,W,PSCAL,TMIN,DMIN,D1
      REAL*8, PARAMETER :: PI = 3.14159265358979323846d0
      REAL*8, PARAMETER :: TREF = 2000.d0
      REAL*8, PARAMETER :: DLIM = 1.d0
      CHARACTER*18 S
      LOGICAL OK
 
 1    FORMAT('<dist><year>',F9.4,'</year><value>',F7.5,
     &'</value></dist><dist><year>',F9.4,'</year><value>',
     & F7.5, '</value></dist><minDist><value>',F7.5,'</value><year>',
     & F11.4, '</year></minDist>')

      CALL GETARG(1,S)
      READ(S,*)DEG,MIN,SEC      
      STARRA = (DEG+MIN/60.d0+SEC/3600.d0)
      CALL GETARG(2,S)
      READ(S,*)DEG,MIN,SEC      
      STARDEC = (ABS(DEG)+MIN/60.d0+SEC/3600.d0)
      IF (S(1:1).EQ."-") STARDEC = -STARDEC
      CALL GETARG(3,S)
      READ(S,*)STARPMRA
      CALL GETARG(4,S)
      READ(S,*)STARPMDEC
      CALL GETARG(5,S)
      READ(S,*)DEG,MIN,SEC      
      CALIBRA = (DEG+MIN/60.d0+SEC/3600.d0)
      CALL GETARG(6,S)
      READ(S,*)DEG,MIN,SEC      
      CALIBDEC = (ABS(DEG)+MIN/60.d0+SEC/3600.d0)
      IF (S(1:1).EQ."-") CALIBDEC = -CALIBDEC
      CALL GETARG(7,S)
      READ(S,*)CALIBPMRA
      CALL GETARG(8,S)
      READ(S,*)CALIBPMDEC
      CALL GETARG(9,S)
      READ(S,*)TIMESPAN

      CD = COS(STARDEC*PI/180.d0)

      RAREL = (CALIBRA-STARRA)*15.d0*60.d0*CD
      DECREL = (CALIBDEC-STARDEC)*60.d0
      PMRAREL = (CALIBPMRA-STARPMRA)*1.d-3/60.d0
      PMDECREL = (CALIBPMDEC-STARPMDEC)*1.d-3/60.d0
      D0 = SQRT(RAREL*RAREL+DECREL*DECREL)
      W = SQRT(PMRAREL*PMRAREL+PMDECREL*PMDECREL)
      PSCAL = RAREL*PMRAREL + DECREL*PMDECREL
      D1 = SQRT(D0*D0+W*W*TIMESPAN*TIMESPAN+2.d0*TIMESPAN*PSCAL)

      TMIN = -PSCAL/W/W
      DMIN = SQRT(D0*D0-PSCAL*PSCAL/W/W) 
      DMLIM = 1.d0/60.d0

      PRINT*,'<calibInfo>' 
      OK = ((D0.LT.DLIM).OR.(D1.LT.DLIM).OR.
     &         ((DMIN.LT.DLIM).AND.(TMIN*(TMIN-TIMESPAN).LT.0.d0)))
      WRITE(*,1)TREF,D0,TREF+TIMESPAN,D1,DMIN,TMIN+TREF
      IF (OK) THEN 
           IF((D0.GT.DMLIM).OR.(D1.GT.DMLIM)) THEN
               PRINT*,'<accepted/>' 
           ELSE
               PRINT*,'<rejected><seemsScienceStar/></rejected>' 
           END IF 
      ELSE
        PRINT*,'<rejected/>'
      END IF 

      PRINT*,'</calibInfo>' 
      END


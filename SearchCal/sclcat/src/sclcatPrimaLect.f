c*******************************************************************************
c JMMC project
c
c "@(#) $Id: sclcatPrimaLect.f,v 1.1 2006-11-27 10:31:19 scetre Exp $"
c
c History
c -------
c $Log: not supported by cvs2svn $
c
c*******************************************************************************

      program lecture

      implicit none

      INTEGER n, status
      character*1300 LIG
      character*200 LIG2
      character*300 LIG1
      character*128 repertoire
      character*21 namec,PHOTKJHN,DIAMOKS,DDIST,FNAM(200),FNAMC(200)
      character*21 CALIBS,RAS,DECS,CALIBNAME(100),ID(200)
      character*1 TAB
      character*19 STAR,NAME(200)
      integer I,J,K,L,SC,ERROR,NS,PP,JJ,NLIG,NCAL,nstars,NSTCAL
      logical ok,DIAMOK,DISTOK,newstar,bad,INDISK,LECTFLAG
      real*8 ms(200),ra(200),dec(200),RACAL(100),DECCAL(100)
      real*8 rdist(100),NCALM,DLIM,DIST,parx(200),dalph(200),ddec(200)
      real*8 ANGDIST(4,200),msinic,perc,ac,ec,angdistc,distc,msc,magc
      real*8 MAG(200),mAGK(100),timespan,dkala(200),dkald(200)
      real*8 DEG,MIN,SEC,S,SS,RACC,DECC,DRA0(100),DDEC0(100)
      real*8 CFACM(100),CFASM(100),CFACMT(100),CFASMT(100)
      REAL*8 CFAC2M(100),CFAS2M(100),CFAC3M(100),CFAS3M(100)
      REAL*8 CFACMJ(100),CFASMJ(100),CFAC2MJ(100),CFAS2MJ(100)
      REAL*8 CFACMS(100),CFASMS(100),CFAC2MS(100),CFAS2MS(100)
      REAL*8 CFACMU(100),CFASMU(100),CFAC2MU(100),CFAS2MU(100)
      REAL*8 CFACMN(100),CFASMN(100),CFAC2MN(100),CFAS2MN(100)
      real*8 CFDCM(100),CFDSM(100),CFDCMT(100),CFDSMT(100)
      REAL*8 CFDC2M(100),CFDS2M(100),CFDC3M(100),CFDS3M(100)
      REAL*8 CFDCMJ(100),CFDSMJ(100),CFDC2MJ(100),CFDS2MJ(100)
      REAL*8 CFDCMS(100),CFDSMS(100),CFDC2MS(100),CFDS2MS(100)
      REAL*8 CFDCMU(100),CFDSMU(100),CFDC2MU(100),CFDS2MU(100)
      REAL*8 CFDCMN(100),CFDSMN(100),CFDC2MN(100),CFDS2MN(100)
      REAL*8 CA0,CD0,SA0,SD0
      integer NP(200),IT(200),MT(200),IRA(3,200),IDEC(3,200)
      integer irac(3),idecc(3),ix
      REAL*8,PARAMETER :: PI=3.14159265358979323846d0
 1    FORMAT(A21,8f12.3)
 2    FORMAT(A21,6f12.3,2f11.3,2X,3I3,7X,3I3)

 3    FORMAT('[',(A),']')
 4    FORMAT('objectName=',(A))
 5    FORMAT('mag=',f5.2)
 6    FORMAT('ra=',I3,':',I2,':',I2)
 7    FORMAT('dec=',I3,':',I2,':',I2)
 8    FORMAT('    dalpha = [',(g),'] + [',(g),']*t')
 9    FORMAT('    ddelta = [',(g),'] + [',(g),']*t')
 10   FORMAT('              + [',(g),']*cos(M) + [',(g),']*sin(M)')
 11   FORMAT('              + [',(g),']*cos(M)*t + [',(g),']*sin(M)*t')
 12   FORMAT('              + [',(g),']*cos(2*M) + [',(g),']*sin(2*M)')
 13   FORMAT('              + [',(g),']*cos(3*M) + [',(g),']*sin(3*M)')
 14   FORMAT('              + [',(g),']*cos(Mj) + [',(g),']*sin(Mj)')
 15   FORMAT('              + [',(g),']*cos(2*Mj) + [',
     &                                      (g),']*sin(2*Mj)')
 16   FORMAT('              + [',(g),']*cos(Ms) + [',(g),']*sin(Ms)')
 17   FORMAT('              + [',(g),']*cos(2*Ms) + [',
     &                                      (g),']*sin(2*Ms)')
 18   FORMAT('              + [',(g),']*cos(Mu) + [',(g),']*sin(Mu)')
 19   FORMAT('              + [',(g),']*cos(2*Mu) + [',
     &                                      (g),']*sin(2*Mu)')
 20   FORMAT('              + [',(g),']*cos(Mn) + [',(g),']*sin(Mn)')
 21   FORMAT('              + [',(g),']*cos(2*Mn) + [',
     &                                      (g),']*sin(2*Mn)')
      
      n=1
      CALL GETARG(n, repertoire)
          write(*,*) "repertoire = ", repertoire

      timespan = 30.d0
      open(15,file='../config/sclcatPrimaExoplanetData.txt',
     &     status='OLD')
      open(16,file='../config/sclcatPrimaExoplanetData_comp.txt',
     &     status='OLD')
      READ(15,'(a)')LIG1
      READ(15,'(a)')LIG1
      READ(16,'(a)')LIG2
      READ(16,'(a)')LIG2
      NLIG = 0
      OK = .TRUE.
      DO WHILE(OK)
        READ(15,'(a)',IOSTAT=ERROR)LIG
        OK = (ERROR.EQ.0)
        IF (OK) NLIG = NLIG+1
      END DO
      REWIND(15)
      READ(15,'(a)')LIG1
      READ(15,'(a)')LIG1
      STAR='                   '
      NS=0
      DO I=1,NLIG
c        READ(15,1)J,DIST(I),MS(I),NP(I) 
c        read(15,1)name(i),msini(1,i),per(1,I),a(1,I) ! ,e(1,I)
c     &            ,angdist(1,i),dist(i)
c,ms(i)
c,ira(1:3,i),idec(1:3,i)
c         READ(15,1)namec,magc,msinic,perc,ac,ec,angdistc,distc
c         print*,distc
         READ(15,'(a)',IOSTAT=ERROR)LIG1
         READ(16,'(a)',IOSTAT=ERROR)LIG2
         OK = (ERROR.EQ.0)
         IF (OK) THEN
           K=0
           L=0
           DO J=1,300
             IF (LIG1(J:J).EQ.'|') THEN
               NAMEC=LIG1(K+1:J-1)
               K=J
               L=L+1 
               IF (L.EQ.1) THEN
                 NEWSTAR=(NAMEC(1:19).ne.STAR) 
                 IF (NEWSTAR) THEN
                   IF (.NOT.BAD) NS = NS+1
                   NAME(NS) = NAMEC(1:19)
                   STAR = NAMEC(1:19)
                   DO WHILE (NAME(NS)(1:1).EQ.' ')
                     NAME(NS)=NAME(NS)(2:LEN_TRIM(NAME(NS)))
                   END DO
                   NAME(NS) = TRIM(NAME(NS))
                   IX = INDEX(LIG2,'|')
                   READ(LIG2(IX+1:IX+25),*,IOSTAT=ERROR)
     &                            DALPH(NS),DDEC(NS)
                   DKALA(NS)=DALPH(NS)*TIMESPAN/1000.d0
                   DKALD(NS)=DDEC(NS)*TIMESPAN/1000.d0
                   BAD=.FALSE.
                 END IF
               ELSE IF (L.EQ.8) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)PARX(NS)
                 PARX(NS) = 1.d0/PARX(NS)
                 IF (ERROR.NE.0) THEN
c                   print*,NAME(NS),' gloub !'
                   BAD=.TRUE.
                 END IF
               ELSE IF (L.EQ.10) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)MS(NS)
                 IF (ERROR.NE.0) THEN
c                   print*,NAME(NS),' gloub !'
                   BAD=.TRUE.
                 END IF
               ELSE IF (L.EQ.12) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)IRA(1:3,NS)
                 RA(NS)=IRA(1,NS)+IRA(2,NS)/60.d0+IRA(3,NS)/3600.d0
c                 RA(NS)=RA(NS)*PI/180.d0
               ELSE IF (L.EQ.13) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)IDEC(1:3,NS)
                 DEC(NS)=IDEC(1,NS)+IDEC(2,NS)/60.d0+IDEC(3,NS)/3600.d0
c                 DEC(NS)=DEC(NS)*PI/180.d0
               END IF 
             END IF
           END DO
         END IF
      END DO
      CLOSE(15)
      CLOSE(16)
      DO I=1,NS
        print*,name(i),RA(i),dec(i)
      end do      

      TAB = "	"
c      open(15,file='list_csv.dat', status='OLD')
c      OK=.TRUE.
c      NSTARS=0
c      DO WHILE(OK)
c        READ(15, '(a)',IOSTAT=ERROR)NAMEC
c       OK=(ERROR.EQ.0)
c       IF (OK) THEN
c         NSTARS=NSTARS+1
c         FNAM(NSTARS)=NAMEC
c          I=INDEX(FNAM(NSTARS),'.csv')
c          FNAMC(NSTARS)=FNAM(NSTARS)(1:I-1)
c          print*,FNAMC(NSTARS)
c        END IF
c      END DO
      print*,'Limite en distance : '
      READ(5,*)DLIM
      DLIM = DLIM*60.d0
      open(16,file=TRIM(repertoire)//'/scalresults.dat',
     &     status='UNKNOWN')
      write(16,*)'-------------------------------------------------'
      write(16,*)'      Limite en distance : ',SNGL(DLIM/60.d0),'arcmin'
      write(16,*)'-------------------------------------------------'
      NSTCAL=0
      NCALM=0.0d0
      NSTARS=0
      DO SC=1,NS
        open(15,file=TRIM(repertoire)//'/'//TRIM(NAME(SC))//'.csv',
     &       status='OLD',IOSTAT=ERROR)
        IF (ERROR.EQ.0) THEN
          print*,NAME(SC),'open ok'
          DO I=1,10           
            READ(15,'(a)')LIG
          END DO
          J=0
          K=0
          DO I=1,1300
            IF (LIG(I:I).EQ.TAB) THEN 
              NAMEC = LIG(J+1:I-1)
              J=I
              K=K+1
              ID(K) = NAMEC
            END IF
          END DO
          OK= .TRUE.
          NCAL=0
          DO WHILE (OK)
            READ(15,'(a)',IOSTAT=ERROR)LIG
            OK=(ERROR.EQ.0)
            IF (OK) THEN
              J=0
              K=0      
              DO I=1,1300
                IF (LIG(I:I).EQ.TAB) THEN
                  namec=LIG(J+1:I-1)
                  J=I
                  K=K+1
                  IF (TRIM(ID(K)).EQ.'2MASS') CALIBS=NAMEC
                  IF (TRIM(ID(K)).EQ.'RAJ2000') RAS=NAMEC 
                  IF (TRIM(ID(K)).EQ.'DEJ2000') DECS=NAMEC 
                  IF (TRIM(ID(K)).EQ.'K') PHOTKJHN=NAMEC 
                  IF (TRIM(ID(K)).EQ.'diamFlag') DIAMOKS=NAMEC 
                  IF (TRIM(ID(K)).EQ.'dist') DDIST=NAMEC 
c                  IF (K.EQ.8) CALIBS=NAMEC
c                  IF (K.EQ.9) RAS=NAMEC
c                  IF (K.EQ.10) DECS=NAMEC
c                  IF (K.EQ.53) PHOTKJHN=NAMEC
c                  IF (K.EQ.88) DIAMOKS=NAMEC
c                  IF (K.EQ.108) DDIST=NAMEC
                END IF
              END DO
              LECTFLAG = .TRUE.
              DIAMOK = (TRIM(DIAMOKS).EQ."OK") 
c              diamok=.true.
              READ(DDIST,*,IOSTAT=ERROR)dist
              LECTFLAG = (LECTFLAG.AND.(ERROR.EQ.0))
              DIST=DIST*3600.d0
              DISTOK = (DIST.LE.DLIM)
              READ(RAS,*,IOSTAT=ERROR)DEG,MIN,SEC
              LECTFLAG = (LECTFLAG.AND.(ERROR.EQ.0))
              RACC = (DEG+MIN/60.d0+SEC/3600.d0-RA(SC))
     &                    *3600.d0*COS(DEC(SC)*PI/180.d0)
              READ(DECS,*,IOSTAT=ERROR)DEG,MIN,SEC
              LECTFLAG = (LECTFLAG.AND.(ERROR.EQ.0))
              DECC = (DEG+MIN/60.d0+SEC/3600.d0-DEC(SC))*3600.d0
 
              
              S = RACC*DKALA(SC)+DECC*DKALD(SC)
     &                /(DKALA(SC)*DKALA(SC)+DKALD(SC)*DKALD(SC))
              IF (S.LT.0.d0) THEN
                INDISK = ((RACC*RACC+DECC*DECC).LT.(DLIM*DLIM))
              ELSE IF (S.GT.1.d0) THEN
                INDISK = (((RACC-DKALA(SC))**2+(DECC-DKALD(SC))**2)
     &                                            .LT.(DLIM*DLIM)) 
              ELSE
                SS = (RACC*RACC+DECC*DECC)
     &                 -S*S*(DKALA(SC)*DKALA(SC)+DKALD(SC)*DKALD(SC))
                 
                INDISK = (SS.LT.DLIM*DLIM)
              END IF
              READ(PHOTKJHN,*,IOSTAT=ERROR)MAGC
              LECTFLAG = (LECTFLAG.AND.(ERROR.EQ.0))
              IF ((DIAMOK).and.(DISTOK).and.
     &                  (INDISK).and.(LECTFLAG)) THEN 
                NCAL = NCAL+1
                CALIBNAME(NCAL) = CALIBS
                READ(PHOTKJHN,*)MAGK(NCAL)
                READ(DDIST,*)RDIST(NCAL)
                RDIST(NCAL) = RDIST(NCAL)*60.d0
                READ(RAS,*)DEG,MIN,SEC
                RACAL(NCAL) = DEG+MIN/60.d0+SEC/3600.d0
                READ(DECS,*)DEG,MIN,SEC
                DECCAL(NCAL) = DEG+MIN/60.d0+SEC/3600.d0
                DRA0(NCAL) = -RACC
                DDEC0(NCAL) = -DECC
                
                CA0 = COS(RA(SC)*PI/180.d0)
                SA0 = SIN(RA(SC)*PI/180.d0)
                CD0 = COS(DEC(SC)*PI/180.d0)
                SD0 = SIN(DEC(SC)*PI/180.d0)
                CFACM(NCAL) = (-0.8968790854d0*CA0-0.2095672747d0*SA0)   
     &                                   *PARX(SC)
                CFACMT(NCAL) = ((-0.1016259470d-8*CA0
     &                          +0.4349256650d-8*SA0)*DALPH(SC)/CD0
     &                   +(-0.1016259470d-8*SA0-0.4349256650d-8*CA0)
     &                     *SD0*DDEC(SC)/CD0)*PARX(SC)
                CFASM(NCAL) = (0.1922607710d0*CA0-0.9774756474d0*SA0)
     &                                   *PARX(SC)
                CFASMT(NCAL) = ((-0.9323998338d-9*SA0
     &                           -0.4740426904d-8*CA0)*DALPH(SC)/CD0
     &                  +(0.9323998338d-9*CA0-0.4740426904d-8*SA0)
     &                     *SD0*DDEC(SC)/CD0)*PARX(SC)
                CFAC2M(NCAL) = (-0.1752620802d-2*SA0
     &                          -0.7500641232d-2*CA0)*PARX(SC)
                CFAS2M(NCAL) = (0.1607998148d-2*CA0-0.8175245643d-2*SA0)
     &                                   *PARX(SC)
                CFAC3M(NCAL) = (-0.2198049379d-4*SA0
     &                          -0.9406929199d-4*CA0)*PARX(SC)
                CFAS3M(NCAL) = (0.2016670877d-4*CA0
     &                          -0.1025298432d-3*SA0)*PARX(SC)
                CFACMJ(NCAL) = (-0.4834356661d-2*SA0
     &                          +0.1093587044d-2*CA0)*PARX(SC)
                CFASMJ(NCAL) = (0.1142793492d-2*SA0
     &                           +0.4432677421d-2*CA0)*PARX(SC)
                CFAC2MJ(NCAL) = (0.2516382071d-4*CA0
     &                            -0.1160245599d-3*SA0)*PARX(SC)
                CFAS2MJ(NCAL) = (0.2742704380d-4*SA0
     &                             +0.1064504525d-3*CA0)*PARX(SC)
                CFACMS(NCAL) = (0.7137221124d-4*SA0
     &                           +0.2518340722d-2*CA0)*PARX(SC)
                CFASMS(NCAL) = (-0.1091838418d-3*CA0
     &                            +0.2725594785d-2*SA0)*PARX(SC)
                CFAC2MS(NCAL) = (0.7001916106d-4*CA0
     &                            +0.1998421922d-5*SA0)*PARX(SC)
                CFAS2MS(NCAL) = (-0.1833516262d-5*CA0
     &                            +0.7631665399d-4*SA0)*PARX(SC)
                CFACMU(NCAL) = (0.1013860931d-3*CA0
     &                            +0.8312236950d-3*SA0)*PARX(SC)
                CFASMU(NCAL) = (-0.7619774893d-3*CA0
     &                           +0.1153418190d-3*SA0)*PARX(SC)
                CFAC2MU(NCAL) = (0.1911814498d-4*SA0
     &                           +0.2433953151d-5*CA0)*PARX(SC)
                CFAS2MU(NCAL) = (-0.1754055508d-4*CA0
     &                            +0.2652861837d-5*SA0)*PARX(SC)
                CFACMN(NCAL) = (0.8931769201d-3*CA0
     &                             -0.1219545186d-2*SA0)*PARX(SC)
                CFASMN(NCAL) = (0.1119972424d-2*CA0
     &                          +0.9528131249d-3*SA0)*PARX(SC)
                CFAC2MN(NCAL) = (-0.6097725928d-5*SA0
     &                           +0.4370944753d-5*CA0)*PARX(SC)
                CFAS2MN(NCAL) = (0.4764065625d-5*SA0
     &                            +0.5594554158d-5*CA0)*PARX(SC)

                CFDCM(NCAL) = (0.8968790854d0*SD0*SA0
     &             -0.3888446721d0*CD0-0.2095672747*SD0*CA0)*PARX(SC)
                CFDCMT(NCAL) = ((0.1885633533d-8*SD0
     &      +0.4349256650d-8*CD0*SA0-0.1016259470d-8*CD0*CA0)*DDEC(SC)
     &                   +(0.1016259470d-8*SA0+0.4349256650d-8*CA0)
     &                    *SD0*DALPH(SC)/CD0)*PARX(SC)
                CFDSM(NCAL) = (-0.9774756474d0*SD0*CA0
     &            -0.1922607710*SD0*SA0+0.8335524562d-1*CD0)*PARX(SC)
                CFDSMT(NCAL) = ((-0.4042448016d-9*SD0
     &      -0.4740426904d-8*CD0*CA0-0.9323998338d-9*CD0*SA0)*DDEC(SC)
     &                   +(-0.9323998338d-9*CA0+0.4740426904d-8*SA0)
     &                   *SD0*DALPH(SC)/CD0)*PARX(SC)
                CFDC2M(NCAL) = (-0.1752620802d-2*SD0*CA0
     &      +0.7500641232d-2*SD0*SA0-0.3251925963d-2*CD0)*PARX(SC)
                CFDS2M(NCAL) = (-0.8175245643d-2*SD0*CA0
     &      -0.1607998147d-2*SD0*SA0+0.6971525180d-3*CD0)*PARX(SC)
                CFDC3M(NCAL) = (-0.4078402944d-4*CD0
     &     +0.9406929199d-4*SD0*SA0-0.2198049379d-4*SD0*CA0)*PARX(SC)
                CFDS3M(NCAL) = (-0.1025298432d-3*SD0*CA0
     &        +0.8743338303d-5*CD0-0.2016670876d-4*SD0*SA0)*PARX(SC)
                CFDCMJ(NCAL) = (0.3505656214d-3*CD0
     &        -0.1093587044d-2*SD0*SA0-0.4834356660d-2*SD0*CA0)*PARX(SC)
                CFDSMJ(NCAL) = (-0.4432677419d-2*SD0*SA0
     &      +0.1929358261d-2*CD0+0.1142793491d-2*SD0*CA0)*PARX(SC)
                CFDC2MJ(NCAL) = (-.2516382070d-4*SD0*SA0
     &       -0.1160245599d-3*SD0*CA0+0.1090985148d-4*CD0)*PARX(SC)
                CFDS2MJ(NCAL) = (0.2742704381d-4*SD0*CA0
     &       -0.1064504525d-3*SD0*SA0+0.4615191947d-4*CD0)*PARX(SC)
                CFDCMS(NCAL) = (0.1043454459d-2*CD0
     &       -0.2518340722d-2*SD0*SA0+0.7137221107d-4*SD0*CA0)*PARX(SC)
                CFDSMS(NCAL) = (0.1091838423d-3*SD0*SA0
     &       +0.7240738854d-4*CD0+0.2725594786d-2*SD0*CA0)*PARX(SC)
                CFDC2MS(NCAL) = (-0.7001916105d-4*SD0*SA0
     &       +0.1998421906d-5*SD0*CA0+0.3035702159d-4*CD0)*PARX(SC)
                CFDS2MS(NCAL) = (0.2742704381d-4*SD0*CA0
     &       -0.1064504525d-3*SD0*SA0+0.4615191947d-4*CD0)*PARX(SC)
                CFDCMU(NCAL) = (-0.1013860929d-3*SD0*SA0
     &       +0.5611658921d-4*CD0+0.8312236947d-3*SD0*CA0)*PARX(SC)
                CFDSMU(NCAL) = (-0.3321533549d-3*CD0
     &       +0.1153418189d-3*SD0*CA0+0.7619774890d-3*SD0*SA0)*PARX(SC)
                CFDC2MU(NCAL) = (-0.2433953146d-5*SD0*SA0
     &       +0.1055247835d-5*CD0+0.1911814498d-4*SD0*CA0)*PARX(SC)
                CFDS2MU(NCAL) = (0.1754055507d-4*SD0*SA0
     &       -0.7604761335d-5*CD0+0.2652861835d-5*SD0*CA0)*PARX(SC)
                CFDCMN(NCAL) = (-0.1219545185d-2*SD0*CA0
     &       -0.8931769197d-3*SD0*SA0+0.3352111108d-3*CD0)*PARX(SC)
                CFDSMN(NCAL) = (0.9528131241d-3*SD0*CA0
     &       -0.1119972423d-2*SD0*SA0+0.4826586283d-3*CD0)*PARX(SC)
                CFDC2MN(NCAL) = (0.1895036474d-5*CD0
     &       -0.4370944751d-5*SD0*SA0-0.6097725924d-5*SD0*CA0)*PARX(SC)
                CFDS2MN(NCAL) = (-0.5594554156d-5*SD0*SA0
     &       +0.2425536077d-5*CD0+0.4764065622d-5*SD0*CA0)*PARX(SC)


              ELSE
                print*,NAME(SC),'1 calibrateur rejeté : DIAM',DIAMOK,
     &             'DIST',DISTOK,'INDISK',INDISK,'LECT',LECTFLAG
              ENDIF
            END IF
          END DO
          IF (NCAL.GT.0) NSTCAL = NSTCAL+1
          NCALM = NCALM+DBLE(NCAL)
c          I = 1
c          print*,TRIM(name(i)),fnamc(sc)
c        DO WHILE (TRIM(NAME(I)).NE.(FNAMC(SC)))
c          print*,name(i)
c          I=I+1
c        END DO
          write(16,*)TRIM(NAME(SC)),NCAL, 'calibrateurs',sngl(ra(SC)),
     &               sngl(dec(SC))
          DO I=1,NCAL
            WRITE(16,*)'Nom : ',TRIM(CALIBNAME(I)),' RA = ',
     &                    SNGL(RACAL(I)),'DEC = ',SNGL(DECCAL(I))
            WRITE(16,*)'    K =',SNGL(MAGK(I)),'d =',SNGL(RDIST(I)),
     &                      'arcmin'
c            WRITE(16,8)SNGL(DRA0(I)),SNGL(0.001*DALPH(SC))
c            WRITE(16,10)SNGL(CFACM(I)),SNGL(CFASM(I))
c            WRITE(16,11)SNGL(CFACMT(I)),SNGL(CFASMT(I))
c            WRITE(16,12)SNGL(CFAC2M(I)),SNGL(CFAS2M(I))
c            WRITE(16,13)SNGL(CFAC3M(I)),SNGL(CFAS3M(I))
c            WRITE(16,14)SNGL(CFACMJ(I)),SNGL(CFASMJ(I))
c            WRITE(16,15)SNGL(CFAC2MJ(I)),SNGL(CFAS2MJ(I))
c            WRITE(16,16)SNGL(CFACMS(I)),SNGL(CFASMS(I))
c            WRITE(16,17)SNGL(CFAC2MS(I)),SNGL(CFAS2MS(I))
c            WRITE(16,18)SNGL(CFACMU(I)),SNGL(CFASMU(I))
c            WRITE(16,19)SNGL(CFAC2MU(I)),SNGL(CFAS2MU(I))
c            WRITE(16,20)SNGL(CFACMN(I)),SNGL(CFASMN(I))
c            WRITE(16,21)SNGL(CFAC2MN(I)),SNGL(CFAS2MN(I))

c            WRITE(16,9)SNGL(DDEC0(I)),SNGL(0.001*DDEC(SC))
c            WRITE(16,10)SNGL(CFDCM(I)),SNGL(CFDSM(I))
c            WRITE(16,11)SNGL(CFDCMT(I)),SNGL(CFDSMT(I))
c            WRITE(16,12)SNGL(CFDC2M(I)),SNGL(CFDS2M(I))
c            WRITE(16,13)SNGL(CFDC3M(I)),SNGL(CFDS3M(I))
c            WRITE(16,14)SNGL(CFDCMJ(I)),SNGL(CFDSMJ(I))
c            WRITE(16,15)SNGL(CFDC2MJ(I)),SNGL(CFDS2MJ(I))
c            WRITE(16,16)SNGL(CFDCMS(I)),SNGL(CFDSMS(I))
c            WRITE(16,17)SNGL(CFDC2MS(I)),SNGL(CFDS2MS(I))
c            WRITE(16,18)SNGL(CFDCMU(I)),SNGL(CFDSMU(I))
c            WRITE(16,19)SNGL(CFDC2MU(I)),SNGL(CFDS2MU(I))
c            WRITE(16,20)SNGL(CFDCMN(I)),SNGL(CFDSMN(I))
c            WRITE(16,21)SNGL(CFDC2MN(I)),SNGL(CFDS2MN(I))
          END DO
          write(16,*)'-------------------------------------------------'
          close(15)
        END IF
      END DO
      ncalm=ncalm/ns
      WRITE(16,*)ns, 'étoiles'
      WRITE(16,*)nstcal, 'étoiles avec des calibrateurs'
      WRITE(16,*)'Nombre moyen de calibrateurs par étoile : ',
     &                SNGL(NCALM)
      CLOse(16)
      END

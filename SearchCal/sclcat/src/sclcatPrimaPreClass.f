c*******************************************************************************
c JMMC project
c
c "@(#) $Id: sclcatPrimaPreClass.f,v 1.1 2006-11-27 10:31:19 scetre Exp $"
c
c History
c -------
c $Log: not supported by cvs2svn $
c
c*******************************************************************************

      program preclass

      implicit none

      character*300 LIG,LIG2
      character*21 namec
      character*19 STAR,NAME(200)
      integer I,J,K,L,ERROR,NS,PP,JJ,NLIG
      logical OK,NEWSTAR,BAD
      real*8 DIST(200),MS(200),XX,YY,PER(4,200),A(4,200),PMAX(200)
      real*8 E(4,200),MSINI(4,200),OM(4,200),TP(4,200),R(200),LIM
      real*8 ANGDIST(4,200),msinic,perc,ac,ec,angdistc,distc,msc,magc
      real*8 MAG(200),dalph(200),ddec(200),timespan,vitsky(200)
      real*8 dkal(200)
      integer NP(200),IT(200),MT(200),IRA(3,200),IDEC(3,200)
      integer irac(3),idecc(3),ix
 1    FORMAT(A21,8f12.3)
 2    FORMAT(A21,6f12.3,2f11.3,2X,3I3,7X,3I3)

 3    FORMAT('[',(A),']')
 4    FORMAT('objectName=',(A))
 5    FORMAT('mag=',f5.2)
 6    FORMAT('ra=',I3,':',I2,':',I2)
 7    FORMAT('dec=',I3,':',I2,':',I2)
 8    FORMAT('radius=',f4.2)
 9    FORMAT('file=',(A),'.csv')
      timespan = 20.d0
      open(15,file='../config/sclcatPrimaExoplanetData.txt',
     &     status='OLD')
      open(16,file='../config/sclcatPrimaExoplanetData_comp.txt',
     &     status='OLD')
      READ(15,'(a)')LIG
c      print*,LIG
      READ(15,'(a)')LIG
c      READ(15,*)LIG
c      print*,LIG
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
      READ(15,'(a)')LIG
      READ(15,'(a)')LIG
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
         READ(15,'(a)',IOSTAT=ERROR)LIG
         READ(16,'(a)',IOSTAT=ERROR)LIG2
         OK = (ERROR.EQ.0)
         IF (OK) THEN
           K=0
           L=0
           DO J=1,300
             IF (LIG(J:J).EQ.'|') THEN
               NAMEC=LIG(K+1:J-1)
               K=J
               L=L+1
               IF (L.EQ.1) THEN
                 NEWSTAR=(NAMEC(1:19).ne.STAR) 
                 IF (NEWSTAR) THEN
                   IF (.NOT.BAD) NS = NS+1
                   NP(NS) = 1
                   NAME(NS) = NAMEC(1:19)
                   STAR = NAMEC(1:19)
                   IX = INDEX(LIG2,'|')
                   READ(LIG2(IX+1:IX+25),*,IOSTAT=ERROR)
     &                            DALPH(NS),DDEC(NS)
                   VITSKY(NS)=
     &                SQRT(DALPH(NS)**2+DDEC(NS)**2)/1000.d0/60.d0
                   DKAL(NS)=VITSKY(NS)*TIMESPAN+1.d0
                   BAD=.FALSE.
                 ELSE
                   NP(NS) = NP(NS)+1  
                 END IF
               ELSE IF (L.EQ.2) THEN
                 READ(NAMEC,*,IOSTAT=ERROR)MSINI(NP(NS),NS)
               ELSE IF (L.EQ.3) THEN
                 READ(NAMEC,*,IOSTAT=ERROR)PER(NP(NS),NS)
               ELSE IF (L.EQ.4) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)A(NP(NS),NS)
               ELSE IF (L.EQ.5) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)E(NP(NS),NS)
               ELSE IF (L.EQ.7) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)ANGDIST(NP(NS),NS)
               ELSE IF (L.EQ.8) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)DIST(NS)
                 IF (ERROR.NE.0) THEN
                   print*,NAME(NS),' gloub !'
c                   BAD=.TRUE.
                 END IF
               ELSE IF (L.EQ.10) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)MS(NS)
                 IF (ERROR.NE.0) THEN
                   print*,NAME(NS),' gloub !'
c                   BAD=.TRUE.
                 END IF
               ELSE IF (L.EQ.12) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)IRA(1:3,NS)
               ELSE IF (L.EQ.13) THEN 
                 READ(NAMEC,*,IOSTAT=ERROR)IDEC(1:3,NS)
               END IF 
             END IF
           END DO
         END IF
      END DO
c      rewind(15) 
c      do i=1,ns
c        print*,NAMe(I),np(i)
c      end do
c      stop
c      READ(15,*)LIG
c      READ(15,*)LIG
c      DO I=1,NS
c        READ(15,*)JJ,XX,YY,PP,
c     &(MSINI(J,I),A(J,I),PER(J,I),E(J,I),OM(J,I),TP(J,I),J=1,NP(I)),
c     &NAME(I)
c      END DO
      CLOSE(15)
      CLOSE(16)
      OPEN(16,FILE='../tmp/classement.dat',STATUS='UNKNOWN') 
      i=66
      print*,name(i),np(i),msini(1,i),per(1,i),a(1,i),e(1,i),
     &         angdist(1,i),dist(i),ms(i),ira(1:3,i),idec(1:3,i)

      DO I=1,NS
c        if (name(i).eq.'            14 Her ')  print*,name(i),
c     & np(i),msini(1,i),per(1,i),a(1,i),e(1,i),
c     &         angdist(1,i),dist(i),ms(i),ira(1:3,i),idec(1:3,i)

        R(I) = 0.d0
        PMAX(I) = 0.0d0
        DO J=1,NP(I)
          MSINI(J,I) = MSINI(J,I)/1047.355d0
          R(I) = R(I)
     &      +MSINI(J,I)/MS(I)*A(J,I)/DIST(I)*(1.d0+E(J,I))*1000.d0
          IF (PER(J,I).GT.PMAX(I)) PMAX(I) = PER(J,I)
        END DO
      END DO
      CALL ORDREB(NS,R,IT,MT)
      LIM = 1.0d0
      DO I=NS,1,-1        
        IF (R(IT(I)).LT.LIM) THEN
          WRITE(16,*)'--------------- ',LIM,' mas -------------------'
          LIM = 0.1d0*LIM
        END IF
        IF (PMAX(IT(I)).GT.(365.25d0)) THEN
          WRITE(16,*)NAME(IT(I)),R(IT(I)),PMAX(IT(I))/365.25d0," ans"
        ELSE
          WRITE(16,*)NAME(IT(I)),R(IT(I)),PMAX(IT(I))," jours"
        END IF
c        IF (I.eq.NS-2)
c     &     WRITE(16,*)'------------ trop long ----------------'
      END DO
      CLOSE(16)
      OPEN(16,FILE='../tmp/command.dat',STATUS='UNKNOWN')




      DO I=NS,1,-1
        IF (NAME(IT(I))(1:3).ne.'(*)') then
          J=1
          DO WHILE(NAME(IT(I))(J:J).EQ.' ')
            J=J+1
          END DO 
          WRITE(16,3)TRIM(NAME(IT(I))(J:))
          WRITE(16,4)TRIM(NAME(IT(I))(J:))
          WRITE(16,5)MAG(IT(I))
          WRITE(16,6)IRA(1:3,IT(I))
          WRITE(16,7)IDEC(1:3,IT(I))
          WRITE(16,8)DKAL(IT(I))
          WRITE(16,9)TRIM(NAME(IT(I))(J:))
          WRITE(16,'(a)')'         '
c        else
c          print*,i,'blm'
        END IF
      END DO
      CLOSE(16)

      END




c**********************************************************************
c		      ORDREB.F
c**********************************************************************
c
c  Sorting routine.  Given an array a(n) of n numbers, it sorts 
c  it by ascending order. a itself is not modified, but the output
c  is two integer arrays it(n) and mt(n)
c      it(m) = # in array a() of number classified at rank m
c      mt(i) = rank of number a(i)
c
c Author:  Michel Henon,
c Note : Modified by Herve Beust
c Date:    10/23/2000

      subroutine ordreb(n,a,it,mt)

      implicit none

      integer ngi,k,j,m,i2,i1,ng,i,n,jmax,imax,lg
      integer it(n),mt(n)
      real*8 a(n)
      logical ok

      if (n.le.0) return
      if (n.eq.1) then
        it(1) = 1
        mt(1) = 1
        return
      end if        

c classement initial par paires.
      i=1
      it(n)=n
      do while(i.lt.n)
        if (a(i).gt.a(i+1)) then
          it(i)=i+1
          it(i+1)=i
        else
          it(i)=i
          it(i+1)=i+1
        end if
        i=i+2
      end do

      if(n.eq.2) then
        do m=1,n
          i=it(m)
          mt(i)=m
        end do
        return
      end if

c... interclassement.la table it contient ng groupes de longueur lg(sauf le
c... dernier qui peut etre plus court). ces groupes sont interclasses deux
c... a deux et les groupes resultants sont transferes dans la table mt.
c... imax et jmax sont les adresses finales des groupes en cours d'inter-
c... classement. i et j sont les adresses des deux nombres presentement
c... compares dans la table it. k est l'adresse dans la nouvelle table mt.

      ng=(n+1)/2
      lg=2
      do
        imax=lg
        jmax=min0(2*lg,n)
        i=1
        j=lg+1
        k=0
        ngi=0

        do while (ng-ngi.ge.2)
          i1=it(i)
          i2=it(j)
          ok=.false.
          do while (.not.ok)
            k=k+1
            if (a(i1).gt.a(i2)) then
              mt(k)=i2
              if (j.eq.jmax) then
                do while (i.le.imax)
                  k=k+1
                  mt(k)=it(i)
                  i=i+1
                end do
                ok=.true.
              else
                j=j+1
                i2=it(j)
              end if
            else
              mt(k)=i1
              if(i.eq.imax) then
                do while (j.le.jmax)
                  k=k+1
                  mt(k)=it(j)
                  j=j+1
                end do
                ok=.true.
              else
                i=i+1
                i1=it(i)
              end if
            end if
          end do
          ngi=ngi+2

c... on passe aux deux groupes suivants
          if (ng-ngi.ge.2) then
            i=jmax+1
            j=i+lg
            imax=jmax+lg
            jmax=min0(imax+lg,n)
          end if
        end do

c... ng est impair. le dernier groupe est transfere tel quel.
        if (ng.ne.ngi) then
          i=jmax
          do while (i.lt.n)
            i=i+1
            mt(i)=it(i)
          end do
        end if

        if (ng.eq.2) then
          do m=1,n
            it(m)=mt(m)
          end do
          do m=1,n
            i=it(m)
            mt(i)=m
          end do
          exit
        end if

c... on passe a l'interclassement suivant.transfert de mt a it.
        ng=(ng+1)/2
        lg=2*lg
        imax=lg
        jmax=min0(2*lg,n)
        i=1
        j=lg+1
        k=0
        ngi=0
        do while (ng-ngi.ge.2) 
          i1=mt(i)
          i2=mt(j)
          ok=.false.
          do while (.not.ok)
            k=k+1
            if (a(i1).gt.a(i2)) then
              it(k)=i2
              if (j.eq.jmax) then
                do while (i.le.imax)
                  k=k+1
                  it(k)=mt(i)
                  i=i+1
                end do
                ok=.true.
              else
                j=j+1
                i2=mt(j)
              end if
            else
              it(k)=i1
              if (i.eq.imax) then
                do while (j.le.jmax)
                  k=k+1
                  it(k)=mt(j)
                  j=j+1
                end do
                ok=.true.
              else
                i=i+1
                i1=mt(i)
              end if
            end if
          end do
          ngi=ngi+2

          if (ng-ngi.ge.2) then
            i=jmax+1
            j=i+lg
            imax=jmax+lg
            jmax=min0(imax+lg,n)
          end if
        end do
        
        if (ng.ne.ngi) then
          i=jmax
          do while (i.lt.n)
            i=i+1
            it(i)=mt(i)
          end do
        end if

        if (ng.eq.2) then

c... formation de la table inverse mt(i).
          do m=1,n
            i=it(m)
            mt(i)=m
          end do
          exit
        end if

c... on passe a l'interclassement suivant.
        ng=(ng+1)/2
        lg=2*lg
      end do
      return
      end    ! ordreb.f
c---------------------------------------------------------------------

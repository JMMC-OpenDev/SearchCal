PRO MAKE_JSDC_SCRIPT_SIMPLE, Database , InputCatalog, nopause=nopause, verbose=verbose, nocatalog=nocatalog, test=test
;
DEFSYSV, '!gdl', exists=isGDL
if (!version.release lt 8.0 and ~isGDL) then message,"This procedure needs IDL >= 8.0"
@ jsdc_define_common.pro
;
;  Database="JMDC_final_lddUpdated.fits"
;  Database="cadarsMatched_spTypeSimbad_MiraAndSBFiltered_14092015-noO4.fits"
  Database=file_basename(Database,".fits")+".fits"
  dowait=(n_elements(nopause) le 0)
  doprint=(n_elements(verbose) gt 0)
  docatalog=(n_elements(nocatalog) le 0)
  dotest=(n_elements(test) gt 0)
  red='0000FF'x & yellow='00FFFF'x & green='00FF00'x & blue='FF0000'x & violet='FF00FF'x
  COLORTABLE=[blue,yellow,green,red,violet]
; open logfile
  openw,unitlog,"make_jsdc.log",/get_lun
;unitlog=-1
;
; Database points : 863 entries
; selected magnitudes : B & V from (?), J, H, K from 2mass
; Basic equation : alog10(d)-0.2*(ci*mj-cj*mi)/(ci-cj) = function(sptype_number), eq. 1
; data : p diameters & m mags each
; m mags --> m-1 linearly independent equations per point --> p*(m-1) linear eqs.
; hypothesis : function = polynom of degree n --> (n+1)*(m-1) unknowns
; define : P (parameter vector), dim = (n+1)*(m-1)
; define : M (measurements vector, left hand side of eq. 1), dim = p*(m-1)
; P & M are related by the linear equation M = L # P where L is the p*(m-1) x (n+1)*(m-1) matrix whose values are 0 or i^k with k=0, n
; introduce: C (covariance matrix of M), dim = p*(m-1) x p*(m-1)
; Method : linear least square fit
; Solution : P = inv[transpose(L)#inv(C)#L]#inv(C)#L#M
;            E_P = sqrt{diagonal(inv[transpose(L)#inv(C)#L])}
;            Chi2_pol_coefs = transpose(M-L#P)#inv(C)#(M-L#P)/[p*(m-1)]
;
; Modeling database
  LUM_CLASSES=0 & DEG=4 & NSIG=5 & NSIG_CHI2=5 & EMAG_MIN=0.01D & STAT=0 & SNR=5; parameters


DOPRINT=1; LBO: debug enabled

PRINTF,UNITLOG,"Polynom degree: ",DEG
PRINTF,UNITLOG,"EMAG MIN: ",EMAG_MIN
PRINTF,UNITLOG,"Residual limit for selection: ",NSIG
PRINTF,UNITLOG,"CHI2 limit for selection: ",NSIG_CHI2
PRINTF,UNITLOG,"Measured diameter SNR threshold: ",SNR

  DATA_B=MRDFITS(Database,1,HEADER) ; restore diameter database from file with new spectral index classification, zero index for SPTYPE="O0.0"
  nn=n_elements(data_b) & printf,unitlog,"Database consists of "+strtrim(nn,2)+" observations."
print,"db init: ",n_elements(data_b)
; database filtering:
; 1) some faint stars have no e_v: put them at 0.04
 A=data_b.e_v & S=WHERE(~finite(A), COUNT) & IF (COUNT GT 0) THEN A[S]=0.04D & data_b.e_v=A ; LBO: why 0.04 ? seems low
;DO NOT filter SB9 stars
;ok=where( strlen(strcompress(data_b.sbc9,/remove_all)) lt 1, count)
;if (count gt 0) then data_b=data_b[ok]
;filter presence sep2 ou sep1 < 1 sec: sufficient to filter sb9 usually
  w=where(data_b.sep2 lt 1, count, comp=ok)
  if (count gt 0) then data_b=data_b[ok]
print,"db filter(sep2): ",n_elements(data_b)
;;filter rotvel > 100 km/s ?
 w=where(float(data_b.rotvel) gt 100.0, count, comp=ok); rotvel may be string!!!
 if (count gt 0) then data_b=data_b[ok]
print,"db filter(rotvel): ",n_elements(data_b)
;
;; filter following objtypes. Note "SB" is not present. I add "sr*" as
;; this is a mira-like star and unreliable
  ListOfOtypesToRemove=[",C*",",S*",",Pu*",",RR*",",Ce*",",dS*",",RV*",",WV*",",bC*",",cC*",",gD*",",SX*",",Mi*"] ;,",sr*"]
  nn=n_elements(data_b)
  ww=bytarr(nn)*0
  for i=0,n_elements(ListOfOtypesToRemove)-1 do begin &$
     ww=(ww or  (strpos(data_b.objtypes, ListOfOtypesToRemove[i] ) ge 0)) &$
  end
  ok=where(ww eq 0) & data_b=data_b[ok]
print,"db filter(Otypes): ",n_elements(data_b)

  nn=n_elements(data_b) & printf,unitlog,"After removing some ObjTypes, we have "+strtrim(nn,2)+" observations left."

; Compute polynom coefficents & covariance matrix from database
; 5 colors BVJHK:
;  USEDBANDS=[0,1,3,4,5] & IBAND=[0,0,0,0] & JBAND=[1,3,4,5] & NCOLORS=N_ELEMENTS(IBAND)
; 5 colors VJHKN:
;  USEDBANDS=[1,3,4,5,8] & IBAND=[1,1,1,1] & JBAND=[3,4,5,8] & NCOLORS=N_ELEMENTS(IBAND)
; 6 colors BVJHKN:
;  USEDBANDS=[0,1,3,4,5,8] & IBAND=[1,1,1,1,1] & JBAND=[0,3,4,5,8] & NCOLORS=N_ELEMENTS(IBAND)

; 4 colors VJHK:
  USEDBANDS=[1,3,4,5] & IBAND=[1,1,1] & JBAND=[3,4,5] & NCOLORS=N_ELEMENTS(IBAND)

; 2020: test BV JHK LMN (too few L/M)
;  USEDBANDS=[0,1, 3,4,5, 6,7,8] & IBAND=[1, 1,1,1, 1,1,1] & JBAND=[0, 3,4,5, 6,7,8] & NCOLORS=N_ELEMENTS(IBAND)

; 2020: test GJHK: 
;  USEDBANDS=[9, 3,4,5] & IBAND=[9,9,9] & JBAND=[3,4,5] & NCOLORS=N_ELEMENTS(IBAND) ; not enough G values
;  USEDBANDS=[0,1, 3,4,5, 6,7,8, 9] & IBAND=[0,1, 9,9,9, 9] & JBAND=[5,5, 3,4,5, 8] & NCOLORS=N_ELEMENTS(IBAND)

; flag to load GAIA mags (G/Bp/Rp) only when needed (old database has not these columns)
LOAD_GAIA_MAG=0 & IF (max(USEDBANDS) GE 9) THEN LOAD_GAIA_MAG=1 & PRINT,"LOAD_GAIA_MAG: ",LOAD_GAIA_MAG


; LBO: fix missing mag or emag to very low-confidence values 
;      to help fitting when the dataset is too small (gaia or all wise)
FIX_MISSING_MAG=0 ; 1 (emag only), 2 (mag+emag)

; generate COLOR TAGS & NAMES:
  MAG_BAND=['B','V','I','J','H','K','L','M','N','G','Bp','Rp']
; B=0,V=1,Ic=2,J=3,H=4,K=5,L=6,M=7,N=8,G=9,Bp=10,Rp=11
  SCOLORS=STRARR(NCOLORS)
  FOR II=0, NCOLORS-1 DO SCOLORS[II]=MAG_BAND[IBAND[II]]+"-"+MAG_BAND[JBAND[II]]
  PRINT,"Colors used: ",SCOLORS

  NSPECTRALTYPES=280            ; 40 per SPTYPE, 4 per subtype.
  E_SPECTRAL_DSB=DBLARR(NSPECTRALTYPES,NCOLORS) & SPECTRAL_DSB=DBLARR(NSPECTRALTYPES,NCOLORS)

; normally we should check these bands exits in the data...

;
; Interstellar reddening coefficients in COMMON
  CF=DBLARR(12)                 ; Rc coefficients
; B V I J H K L M N
; LBO: for LMN use Indebetouw 2005 that gives 0.205 0.155 0.133
;  CF[0]=4.10D & CF[1]=3.1D & CF[2]=1.57D & CF[3]=0.86D & CF[4]=0.53D & CF[5]=0.36D & CF[6]=0.57D*CF[5] & CF[7]=0.43D*CF[5] & CF[8]=0.37D*CF[5]
; 2020: LBO: use VOSA filter coefficients (2MASS + WISE W1-W3) from
; http://svo2.cab.inta-csic.es/theory/fps3/pavosa.php?oby=id&fid=2MASS/2MASS.Ks#2MASS/2MASS.Ks

; 2MASS/2MASS.J	12350.00	1624.32	0.31	1594.00	Vega	Pogson	2MASS J
; 2MASS/2MASS.H	16620.00	2509.40	0.19	1024.00	Vega	Pogson	2MASS H
; 2MASS/2MASS.Ks	21590.00	2618.87	0.13	666.80	Vega	Pogson	2MASS Ks

; WISE/WISE.W1	33526.00	6626.42	0.07	309.54	Vega	Pogson	WISE W1 filter
; WISE/WISE.W2	46028.00	10422.66	0.05	171.79	Vega	Pogson	WISE W2 filter
; WISE/WISE.W3	115608.00	55055.71	0.06	31.67	Vega	Pogson	WISE W3 filter

; GAIA/GAIA2r.G	5836.31	4358.43	0.94	2835.09	Vega	Pogson	GAIA G filter, DR2 revised curve
; GAIA/GAIA2r.Gbp	5020.92	2279.45	1.13	3393.28	Vega	Pogson	GAIA Gbp filter, DR2 revised curve
; GAIA/GAIA2r.Grp	7588.83	2943.72	0.66	2485.08	Vega	Pogson	GAIA Grp filter, DR2 revised curve

  CF[0]=4.10D & CF[1]=3.1D & CF[2]=1.57D & CF[3]=0.31D*CF[1] & CF[4]=0.19D*CF[1] & CF[5]=0.13D*CF[1] & CF[6]=0.07D*CF[1] & CF[7]=0.05D*CF[1] & CF[8]=0.06D*CF[1]
  ; GAIA G/BP/RP:
  CF[9]=0.94D*CF[1] & CF[10]=1.13D*CF[1] & CF[11]=0.66D*CF[1]
  CF/=3.1D                      ; divide by Rv

  IF (DOPRINT) THEN PRINTF,UNITLOG,'Interstellar reddening coefficients CF: ',CF
  CI=CF[IBAND]/(CF[IBAND]-CF[JBAND]) & CJ=CF[JBAND]/(CF[IBAND]-CF[JBAND])

; valeurs d'alain (reference):
; CF=[1.32,1.0,0.48,0.28,0.17,0.12]  & CI=CF[IBAND]/(CF[IBAND]-CF[JBAND]) & CJ=CF[JBAND]/(CF[IBAND]-CF[JBAND])

; INITIALIZE the dataBase (all arrays subscripted by _B) here instead
; of in the routines.
  IF (LOAD_GAIA_MAG EQ 1) THEN BEGIN
  MAG_B=[TRANSPOSE(DATA_B.B),TRANSPOSE(DATA_B.V),TRANSPOSE(DATA_B.ICOUS),TRANSPOSE(DATA_B.J),TRANSPOSE(DATA_B.H),TRANSPOSE(DATA_B.K),TRANSPOSE(DATA_B.L),TRANSPOSE(DATA_B.M),TRANSPOSE(DATA_B.N),TRANSPOSE(DATA_B.G),TRANSPOSE(DATA_B.BP),TRANSPOSE(DATA_B.RP)]
  EMAG_B=[TRANSPOSE(DATA_B.E_B),TRANSPOSE(DATA_B.E_V),TRANSPOSE(DATA_B.E_ICOUS),TRANSPOSE(DATA_B.E_J),TRANSPOSE(DATA_B.E_H),TRANSPOSE(DATA_B.E_K),TRANSPOSE(DATA_B.E_L),TRANSPOSE(DATA_B.E_M),TRANSPOSE(DATA_B.E_N),TRANSPOSE(DATA_B.E_G),TRANSPOSE(DATA_B.E_BP),TRANSPOSE(DATA_B.E_RP)]
  ENDIF ELSE BEGIN
  MAG_B=[TRANSPOSE(DATA_B.B),TRANSPOSE(DATA_B.V),TRANSPOSE(DATA_B.ICOUS),TRANSPOSE(DATA_B.J),TRANSPOSE(DATA_B.H),TRANSPOSE(DATA_B.K),TRANSPOSE(DATA_B.L),TRANSPOSE(DATA_B.M),TRANSPOSE(DATA_B.N)]
  EMAG_B=[TRANSPOSE(DATA_B.E_B),TRANSPOSE(DATA_B.E_V),TRANSPOSE(DATA_B.E_ICOUS),TRANSPOSE(DATA_B.E_J),TRANSPOSE(DATA_B.E_H),TRANSPOSE(DATA_B.E_K),TRANSPOSE(DATA_B.E_L),TRANSPOSE(DATA_B.E_M),TRANSPOSE(DATA_B.E_N)]
  ENDELSE
  LUMCLASS_B=DATA_B.LUM_CLASS & DLUMCLASS_B=DATA_B.LUM_CLASS_DELTA & SPTYPE_B=DOUBLE(DATA_B.COLOR_TABLE_INDEX) & DSPTYPE_B=DOUBLE(DATA_B.COLOR_TABLE_DELTA) & ORIG_SPTYPE=DATA_B.SPTYPE
  MAG_B=TRANSPOSE(MAG_B) & EMAG_B=ABS(TRANSPOSE(EMAG_B)) & DIAM_I=DATA_B.LD_DIAM & EDIAM_I=DATA_B.E_LD_DIAM
  PARAMS=DBLARR(NCOLORS,DEG+1) & E_PARAMS=PARAMS
  NSTAR_B=N_ELEMENTS(MAG_B[*,0])
  DIAM_B=DBLARR(NSTAR_B,NCOLORS) & EDIAM_B=DIAM_B & CHI2_MD=DBLARR(NSTAR_B) & DMEAN_B=DBLARR(NSTAR_B) & EDMEAN_B=DMEAN_B
  RES_B=DBLARR(NSTAR_B,NCOLORS)-100 & RES_C=RES_B

; stats on mags:
  FOR II=0, N_ELEMENTS(MAG_B[0,*])-1 DO BEGIN
    IF (FIX_MISSING_MAG EQ 1) THEN BEGIN
        PRINT,"Fixing missing emag:"
        M_EMAG=WHERE(FINITE(MAG_B[*,II]) AND ~FINITE(EMAG_B[*,II]), N_EMAG)
        EMAG_B[M_EMAG,II]=1.0D ; delta=5 mags (5sigma ie covers [-5; +5])
        PRINT,"Band ",MAG_BAND[II]," missing e_mag = ",N_EMAG
    ENDIF
    IF (FIX_MISSING_MAG EQ 2) THEN BEGIN
        PRINT,"Fixing missing mag/emag:"
        M_MAG=WHERE(~FINITE(MAG_B[*,II]), N_MAG)
        M_EMAG=WHERE(~FINITE(MAG_B[*,II]) OR ~FINITE(EMAG_B[*,II]), N_EMAG)
        MAG_B[M_MAG,II]=5.0D & EMAG_B[M_EMAG,II]=2.5D ; delta=10 mags (5sigma ie covers [-5; +15])
        PRINT,"Band ",MAG_BAND[II]," missing mag = ",N_MAG," missing e_mag = ",N_EMAG
    ENDIF
    HAS_MAG=WHERE(FINITE(MAG_B[*,II]) AND FINITE(EMAG_B[*,II]), NMAG)
    PRINT,"mag ",MAG_BAND[II],NMAG," MIN: ",MIN(MAG_B[HAS_MAG,II])," MAX: ",MAX(MAG_B[HAS_MAG,II])," MEAN: ",MEAN(MAG_B[HAS_MAG,II])," E_MIN: ",MIN(EMAG_B[HAS_MAG,II])," E_MAX: ",MAX(EMAG_B[HAS_MAG,II])," E_MEAN: ",MEAN(EMAG_B[HAS_MAG,II])
  ENDFOR

; start info on database health
  PRINTF,UNITLOG, "Statistics on stars used:"
  STAR_ID=DATA_B.SIMBAD & BY_STARS=STAR_ID[UNIQ(STAR_ID,SORT(STAR_ID))] & PRINTF,UNITLOG,STRTRIM(N_ELEMENTS(STAR_ID),2)+" data points with "+STRTRIM(N_ELEMENTS(BY_STARS),2),+" different stars"
; normally only the subset USEDBANDS should be checked for Nans.
  USEABLE_MEASUREMENTS=WHERE(FINITE(TOTAL(MAG_B[*,usedbands],2)) AND FINITE(TOTAL(EMAG_B[*,usedbands],2)) AND SPTYPE_B NE -1 AND DIAM_I NE -1 AND EDIAM_I NE -1 AND DIAM_I/EDIAM_I GT SNR, NUSEABLE)
; number of unique stars in this useable list
  BY_U_STARS=STAR_ID[USEABLE_MEASUREMENTS] & BY_STARS=BY_U_STARS[UNIQ(BY_U_STARS,SORT(BY_U_STARS))] & PRINTF,UNITLOG,STRTRIM(NUSEABLE,2)+" observations with correct measurements on "+STRTRIM(N_ELEMENTS(BY_STARS),2)+" different stars"

; run modeling, first freely to find sigmas;
  PRINTF,UNITLOG,""
  PRINTF,UNITLOG,"Calling make_jsdc_polynoms in mode VAR:"
  MODE='VAR' & MAKE_JSDC_POLYNOMS,RESIDU,E_RESIDU

; LBO then fix the used stars to some sigma:
  PRINTF,UNITLOG,""
  PRINTF,UNITLOG,"Calling make_jsdc_polynoms in mode FIX:"
  S=WHERE(CHI2_MD[GOOD_B] LT NSIG_CHI2) & GOOD_FIX=GOOD_B[S] ; select stars with chi2 smaller than NSIG_CHI2
  MODE='FIX' & MAKE_JSDC_POLYNOMS,RESIDU,E_RESIDU

; Results
  PRINTF,UNITLOG,""
  NOUT=N_ELEMENTS(GOOD_B) &  BY_STARS=STAR_ID[GOOD_B]  & BY_STARS=BY_STARS[UNIQ(BY_STARS,SORT(BY_STARS))] & PRINTF,UNITLOG,"Statistics:" &  PRINTF,UNITLOG,"Selected data points :"+STRTRIM(NOUT,2)+" data points, on "+STRTRIM(N_ELEMENTS(BY_STARS),2)+" distinct stars"
  STAR_IDC=STAR_ID[GOOD_B]
  S1=WHERE(LUMCLASS_B[GOOD_B] EQ 1,  N1) & Q1=STAR_IDC[S1] & Q1=Q1[UNIQ(Q1,SORT(Q1))] & PRINTF,UNITLOG,"lumClass   I: "+STRTRIM(N1,2)+" points, "+STRTRIM(N_ELEMENTS(Q1),2)+" stars"
  S2=WHERE(LUMCLASS_B[GOOD_B] EQ 2,  N2) & Q2=STAR_IDC[S2] & Q2=Q2[UNIQ(Q2,SORT(Q2))] & PRINTF,UNITLOG,"lumClass  II: "+STRTRIM(N2,2)+" points, "+STRTRIM(N_ELEMENTS(Q2),2)+" stars"
  S3=WHERE(LUMCLASS_B[GOOD_B] EQ 3,  N3) & Q3=STAR_IDC[S3] & Q3=Q3[UNIQ(Q3,SORT(Q3))] & PRINTF,UNITLOG,"lumClass III: "+STRTRIM(N3,2)+" points, "+STRTRIM(N_ELEMENTS(Q3),2)+" stars"
  S4=WHERE(LUMCLASS_B[GOOD_B] EQ 4,  N4) & Q4=STAR_IDC[S4] & Q4=Q4[UNIQ(Q4,SORT(Q4))] & PRINTF,UNITLOG,"lumClass  IV: "+STRTRIM(N4,2)+" points, "+STRTRIM(N_ELEMENTS(Q4),2)+" stars"
  S5=WHERE(LUMCLASS_B[GOOD_B] EQ 5,  N5) & Q5=STAR_IDC[S5] & Q5=Q5[UNIQ(Q5,SORT(Q5))] & PRINTF,UNITLOG,"lumClass   V: "+STRTRIM(N5,2)+" points, "+STRTRIM(N_ELEMENTS(Q5),2)+" stars"
  S0=WHERE(LUMCLASS_B[GOOD_B] EQ -1, N0) & Q0=STAR_IDC[S0] & Q0=Q0[UNIQ(Q0,SORT(Q0))] & PRINTF,UNITLOG,"lumClass ???: "+STRTRIM(N0,2)+" points, "+STRTRIM(N_ELEMENTS(Q0),2)+" stars"

  A=[S1,S2,S3,S4,S5,S0] & B=[Q1,Q2,Q3,Q4,Q5,Q0] & PRINTF,UNITLOG,"Total   "+STRTRIM(N_ELEMENTS(A),2)+" points, "+STRTRIM(N_ELEMENTS(B),2)+" stars"

; 2/ Plot residuals & fit band per band
  window,0
  !P.MULTI=[0,2,2] & X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X),/FILL
  Y1=DINDGEN(NSPECTRALTYPES)/4.
  FOR N=0,N_ELEMENTS(RESIDU[0,*])-1 DO BEGIN
     A=WHERE(Y1 GE MIN(SPTYPE_B[GOOD_B]/4.) AND Y1 LE MAX(SPTYPE_B[GOOD_B]/4.))
     PLOT,SPTYPE_B[GOOD_B]/4.,RESIDU[GOOD_B,N],PSYM=8,YRANGE=[0,1.1],YSTYLE=1,XRANGE=[0,70],XSTYLE=1,XTITLE='SPECTRAL TYPE',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0']
     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 3 AND DLUMCLASS_B EQ 0,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=yellow
     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 1 AND DLUMCLASS_B EQ 0,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=green
     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 2 AND DLUMCLASS_B EQ 0,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=blue
     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 4 AND DLUMCLASS_B EQ 0,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=violet

     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 1 OR LUMCLASS_B[GOOD_B] EQ 2 OR LUMCLASS_B[GOOD_B] EQ 3,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=yellow
     Q=WHERE(LUMCLASS_B[GOOD_B] EQ 5 AND DLUMCLASS_B EQ 0,M) & IF (M NE 0) THEN OPLOT,SPTYPE_B[GOOD_B[Q]]/4.,RESIDU[GOOD_B[Q],N],PSYM=8,COLOR=red
     OPLOTERR,Y1[A],SPECTRAL_DSB[A,N],E_SPECTRAL_DSB[A,N],NOHAT=1,PSYM=3,THICK=4
  ENDFOR
  !P.MULTI=0

; 3/ Resample residuals & fit with polynom
  X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X), /FILL
  window,1
 !P.MULTI=[0,1,NCOLORS]

  SPM=DBLARR(300)-100 & RM=DBLARR(300,10)-100 & ERM=RM
  NPMIN=1 & RRR=RM & PARAM=DBLARR(NCOLORS,DEG+1) & E_PARAM=PARAM
  A=DINDGEN(N_ELEMENTS(GOOD_B))

  PRINTF,UNITLOG,""
  PRINTF,UNITLOG,"Number of stars per spectral type:"
  BIN=12 & HH=HISTOGRAM(SPTYPE_B[GOOD_B[A]],BINSIZE=BIN,LOCATIONS=XX,REVERSE_INDICES=R,MIN=-BIN/2.) & XX=XX+BIN/2.
  NofClass=intarr(N_ELEMENTS(XX))
  FOR II=0, N_ELEMENTS(XX)-1 DO BEGIN
     IF (R[II+1]-R[II] GE NPMIN) THEN BEGIN
        T=R[R[II]:R[II+1]-1] & SPM[II]=MEDIAN(SPTYPE_B[(GOOD_B[A[T]])])
        FOR N=0, NCOLORS-1 DO BEGIN
           ERM[II,N]=1./SQRT(TOTAL(1./E_RESIDU[GOOD_B[A[T]],N]^2)) & RM[II,N]=TOTAL(RESIDU[GOOD_B[A[T]],N]/E_RESIDU[GOOD_B[A[T]],N]^2)*ERM[II,N]^2
           IF (N EQ 1) THEN SPM[II]=TOTAL(SPTYPE_B[GOOD_B[A[T]]]/E_RESIDU[GOOD_B[A[T]],N]^2)*ERM[II,N]^2
        ENDFOR
        NoFclass[II]=N_ELEMENTS(T)
        IF (DOPRINT) THEN PRINTF,UNITLOG,FLOOR(SPM[II]/4.),NoFclass[II]
     ENDIF
  ENDFOR

  S=WHERE(SPM NE -100) & NS=N_ELEMENTS(S) & YFIT=DBLARR(NSPECTRALTYPES,NCOLORS) & XFIT=YFIT & FIT=DBLARR(NS,NCOLORS)

  PRINTF,UNITLOG,""
  PRINTF,UNITLOG,"Gauss Fit of Residual Histograms:"
  PRINTF,UNITLOG,"    Color    size(S)   chi2   Sigma"

  FOR N=0, NCOLORS-1 DO BEGIN
     MDAT=RM[S,N]/ERM[S,N] & VEC=DBLARR(NS,DEG+1) & FOR KK=0, DEG DO VEC[*,KK]=SPM[S]^KK/ERM[S,N]
     INV=INVERT(TRANSPOSE(VEC)#VEC,/DOUBLE,STATUS) & PARAM[N,*]=INV#TRANSPOSE(VEC)#MDAT & R=DINDGEN(DEG+1) & E_PARAM[N,*]=SQRT(INV[R,R])
     YMIN=0 & IF (N EQ 0) THEN YMAX=1.0 & IF (N NE 0) THEN YMAX=1.0
     FOR KK=0, DEG DO FIT[*,N]=FIT[*,N]+PARAMS[N,KK]*SPM[S]^KK
     FOR KK=0, DEG DO YFIT[*,N]=YFIT[*,N]+PARAMS[N,KK]*DINDGEN(NSPECTRALTYPES)^KK & XFIT=DINDGEN(NSPECTRALTYPES)
     PLOTERR,SPM[S]/4,RM[S,N],ERM[S,N],PSYM=8,NOHAT=1,CHARS=2,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX],XTITLE='SPECTRAL TYPE',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0']
     XYOUTS,FLOOR(SPM/4.),YMIN+0.1,strtrim(NoFclass,2)
     OPLOT,SPM[S]/4,FIT[*,N],LINESTYLE=1
     RRR[S,N]=(RM[S,N]-FIT[*,N])/ERM[S,N] & CHI2=MEAN(RRR[S,N]^2) & SIG=SIGMA(RM[S,N]-FIT[*,N])

     IF (DOPRINT) THEN PRINTF,UNITLOG,N,N_ELEMENTS(S),CHI2,SIG*ALOG(10)

  ENDFOR
  !P.MULTI=0

; 4/ Plot output versus input diameter & histogram of residuals (database)
  window,2
  !P.MULTI=[0,1,2]
  X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X)
  ELDI=EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10.) & ZZ=DINDGEN(100)/10-5 & PLOT,ZZ,ZZ,YRANGE=[-1,2],XRANGE=[-1,2],XSTYLE=1,YSTYLE=1,$
     XTITLE='INPUT DIAMETER', YTITLE='COMPUTED DIAMETER',CHARSIZE=1.5
  OPLOT,ALOG10(DIAM_I[GOOD_B]),DMEAN_B[GOOD_B],PSYM=8
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 1 AND DLUMCLASS_B EQ 0) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=green
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 2 AND DLUMCLASS_B EQ 0) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=blue
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 3 AND DLUMCLASS_B EQ 0) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=yellow
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 4 AND DLUMCLASS_B EQ 0) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=violet
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 5 AND DLUMCLASS_B EQ 0) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=red
;
  USERSYM,COS(X),SIN(X),/FILL
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 1 OR LUMCLASS_B[GOOD_B] EQ 2 OR LUMCLASS_B[GOOD_B] EQ 3) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=yellow
  Q=WHERE(LUMCLASS_B[GOOD_B] EQ 4 OR LUMCLASS_B[GOOD_B] EQ 5) & OPLOT,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=red

; LBO: CHI2 or CHI2_DMEAN ??
CHI2_DMEAN=MEAN(CHI2_MD[GOOD_B])

  OPLOT,ZZ,ZZ,COLOR=blue,THICK=2 & XYOUTS,-0.5,1.5,'CHI2 = '+strtrim(string(CHI2_DMEAN,format='(F5.2)')),CHARSIZE=1.5
;
  num=DMEAN_B[GOOD_B]-ALOG10(DIAM_I[GOOD_B])
  den=sqrt(EDMEAN_B[GOOD_B]^2+(EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10.))^2)
  QQ=num/den
  BIN=0.25 & HH=HISTOGRAM(QQ,BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10,XRANGE=[-5,5],THICK=2,XTITLE='HISTOGRAM OF NORMALIZED RESIDUALS',YTITLE='COUNTS',CHARSIZE=1.5,XSTYLE=1
  RES=GAUSSFIT(XX,HH,NTERMS=3,Z,SIGMA=MYSIGMA) & OPLOT,XX,RES,THICK=4
  XYOUTS,-4,Z[0]-10,'SIGMA = '+string(Z[2],format='(F5.2)'),CHARSIZE=1.5

  for icolor=0,NCOLORS-1 do begin
     HH=HISTOGRAM(RES_B[GOOD_B,icolor],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & RES=GAUSSFIT(XX,HH,NTERMS=3,Z) & OPLOT,XX,RES*3,COLOR=COLORTABLE[icolor mod 5],THICK=2 & IF (DOPRINT) THEN PRINT,Z
  END
  !P.MULTI=0
rep='' & if (dowait) then READ, 'press any key to continue', rep

; PRINT POLYNOMS & Covariance matrix for SearchCal in cfg mode
; open alxAngDiamPolynomial.cfg file as output
  openw,unitpol,"alxAngDiamPolynomial.cfg",/get_lun,width=2048

  PRINTF,unitpol,"##*******************************************************************************"
  PRINTF,unitpol,"# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS."
  PRINTF,unitpol,"#********************************************************************************"
  PRINTF,unitpol,"# This file contains the polynomial coefficients to compute the angular diameter."
  PRINTF,unitpol,"#"
  PRINTF,unitpol,"# Warning : The first column (color) is used during file parsing (consistency)."
  PRINTF,unitpol,"# Therefore, lines must not be re-ordered, without changing software !"
  PRINTF,unitpol,"#*********************************************************************************"
  PRINTF,unitpol,"#"
  PRINTF,unitpol,"# IDL Fit results ("+systime()+") with NSIG="+strtrim(NSIG,2)+" DEG="+strtrim(DEG,2)
  PRINTF,unitpol,"#"
  PRINTF,unitpol,"#FIT COLORS: ",SCOLORS
  CHI2_DMEAN=MEAN(CHI2_MD[GOOD_B])
  PRINTF,unitpol,"#domain:", MIN(SPTYPE_B[GOOD_B]), MAX(SPTYPE_B[GOOD_B])
  PRINTF,unitpol,"# Polynom coefficients ("+STRTRIM(DEG,1)+"th degree) from idl fit on "+strtrim(N_ELEMENTS(GOOD_B),1)+" stars."
  PRINTF,unitpol,"# CHI2_POLYNOM: ",CHI2_POL
  PRINTF,unitpol,"# CHI2_DMEAN  : ",CHI2_DMEAN
  PRINTF,unitpol,"#Color a0 a1 a2 a3 a4 etc..."
    myformat='(A,'+STRTRIM(DEG+1,1)+'(1X,G23.15))' & FOR II=0, NCOLORS-1 DO PRINTF,unitpol,format=myformat,SCOLORS[II],PAR[II,*]
  PRINTF,unitpol,"#"
  CLOSE,unitpol

;DEBUG
FOR II=0, NCOLORS-1 DO PRINT,format=myformat,SCOLORS[II],PAR[II,*]

; open alxAngDiamPolynomialCovariance.cfg file as output
  openw,unitcovpol ,"alxAngDiamPolynomialCovariance.cfg",/get_lun,width=2048
  IF (((DEG+1) * NCOLORS) NE 20) THEN IF (DOPRINT) THEN PRINT,"FIX Covariance matrix output"

  PRINTF,unitcovpol,"##*******************************************************************************"
  PRINTF,unitcovpol,"# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS."
  PRINTF,unitcovpol,"#********************************************************************************"
  PRINTF,unitcovpol,"# This file contains the polynomial coefficients to compute the"
  PRINTF,unitcovpol,"# angular diameter errors. See alxAngDiamPolynomial.cfg."
; false info:
; PRINTF,unitcovpol,"# Warning : The first column (color) is used during file parsing (consistency)."
; PRINTF,unitcovpol,"# Therefore, lines must not be re-ordered, without changing software !"
  PRINTF,unitcovpol,"#*********************************************************************************"
  PRINTF,unitcovpol,"#"
  PRINTF,unitcovpol,"# MCOV_POL matrix from IDL ["+strtrim(ncolors*(deg+1),1)+" x "+strtrim(ncolors*(deg+1),1)+"] for "+strtrim(NCOLORS,1)+" and "+strtrim(deg+1,1)+" polynomial coefficients (a0...a"+strtrim(deg,1)+")"
  PRINTF,unitcovpol,"#"
  PRINTF,unitcovpol,"# Covariance matrix of polynom coefficients [a0ij...an_ij][a0ij...an_ij]"
  format='(I,'+STRTRIM((DEG+1)*NCOLORS,2)+'(1x,G23.15))' & FOR II=0, (((DEG+1)*NCOLORS) -1) DO PRINTF,unitcovpol,format=format,II,MCOV_POL[II,*]
  PRINTF,unitcovpol,"#"
  CLOSE,unitcovpol

;;FIG6 : reconstructed vs measured diameters & histogram of normalized residuals
;;
;window,3
;!P.MULTI=[0,2,1]
;X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X)
;ELDI=EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10.) & ZZ=DINDGEN(100)/10-5 & PLOT,ZZ,ZZ,YRANGE=[-1,2],XRANGE=[-1,2],XSTYLE=1,YSTYLE=1,$
;XTITLE='INPUT DIAMETER', YTITLE='COMPUTED DIAMETER',CHARSIZE=1.5
;OPLOTERR,ALOG10(DIAM_I[GOOD_B]),DMEAN_B[GOOD_B],EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10),EDMEAN_B[GOOD_B],PSYM=8,NOHAT=1
;Q=WHERE((LUMCLASS_B[GOOD_B] EQ 1 OR LUMCLASS_B[GOOD_B] EQ 2 OR LUMCLASS_B[GOOD_B] EQ 3) AND DLUMCLASS_B[GOOD_B] EQ 0)
;OPLOTERR,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],EDIAM_I[GOOD_B[Q]]/DIAM_I[GOOD_B[Q]]/ALOG(10),EDMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=green,NOHAT=1
;Q=WHERE((LUMCLASS_B[GOOD_B] EQ 4 OR LUMCLASS_B[GOOD_B] EQ 5) AND DLUMCLASS_B[GOOD_B] EQ 0)
;OPLOTERR,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],EDIAM_I[GOOD_B[Q]]/DIAM_I[GOOD_B[Q]]/ALOG(10),EDMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=red,NOHAT=1
;RR=(ALOG10(DIAM_I[GOOD_B])-DMEAN_B[GOOD_B])/SQRT(EDIAM_I[GOOD_B]^2/DIAM_I[GOOD_B]^2/ALOG(10)^2+EDMEAN_B[GOOD_B]^2) & IF (DOPRINT) THEN PRINT,MEAN(RR^2),MEDIAN(RR^2)
;BIN=0.25 & HH=HISTOGRAM(RR,BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10 & RES=GAUSSFIT(XX,HH,NTERMS=3,Z) & OPLOT,XX,RES & IF (DOPRINT) THEN PRINT,Z
;!P.MULTI=0

; produce figure 2 of paper (sort of)
  GRAF_RESIDUAL_VS_SPTYPE,RESIDU,SPECTRAL_DSB,E_SPECTRAL_DSB,'figure2'
; sort of fig. 4
  GRAF_DIAM_IN_VS_OUT,'diam_in_vs_out' ; input versus computed diameter
; LBO: EXIT HERE
  rep='' & READ, 'press any key to finish (and closing all windows)', rep
  wdelete,0,1,2

;
; output catalog as fits file: replace LDD with DMEAN_C, E_LDD with
; EDMEAN_C and DIAM_CHI2 with some chi2.
  data_b.diam_chi2=MEAN(RES_B^2,dimension=2)
  bad=where(data_b.diam_chi2 ge 10000, count)
  if (count gt 0 ) then data_b[bad].diam_chi2=-1

  ln_10=alog(10.)
  data_b.LDD=exp(dmean_b*ln_10)
  data_b.E_LDD=edmean_b*ln_10*data_b.LDD
  bad=where(data_b.e_ldd le 0, count)
  if (count gt 0 ) then begin  data_b[bad].e_ldd=-1 &  data_b[bad].ldd=-1 & end

; output only measurements saved
  mwrfits,data_b[good_b],"DataBaseUsed.fits",/CREATE

if (~docatalog) then exit,status=0

; Create JSDC v.2 :
  if (dotest) then catalog='DataBaseUsed.fits' else if n_elements(InputCatalog gt 0) then catalog=InputCatalog else catalog='CatalogAugmented.fits'

  PRINTF,UNITLOG,""
  PRINTF,UNITLOG,"Make_jsdc_catalog on "+catalog

  DATA_C=MRDFITS(catalog,1,HEADER)
  LUM_CLASSES=0
  MAKE_JSDC_CATALOG

; output as SearchCal catalog with columns updated :
  data_c.diam_chi2=chi2_ds
  bad=where(data_c.diam_chi2 le 0, count)
  if (count gt 0 ) then data_c[bad].diam_chi2=-1
  ln_10=alog(10.)
  data_c.LDD=exp(dmean_c*ln_10)

; normally we have
;  data_c.E_LDD=edmean_c*ln_10*data_c.LDD
; instead here we add 2% on relative error to take into account biases
  unbiased_relerr=sqrt((edmean_c*ln_10)^2+0.02^2)
  data_c.E_LDD=unbiased_relerr*data_c.LDD

  bad=where(data_c.diam_chi2 le 0, count)
  if (count gt 0 ) then begin data_c[bad].e_ldd=-1 &  data_c[bad].ldd=-1 & end

; output a subset of columns as JSDC v.2 for publication + first cols
; of Database in case of TEST:
if (doTest) then begin
  testcolumns=["ID1","LD_DIAM","E_LD_DIAM","NOTES","REFERENCE"]
  teststruct=create_struct(testcolumns,"",0.0,0.0,"","")
end
; columns are:
columns=[              "NAME","RAJ2000","DEJ2000","Bmag","e_Bmag","Vmag","e_Vmag","Rmag","Imag",$
"Jmag","e_Jmag","Hmag","e_Hmag","Kmag","e_Kmag","Lmag","e_Lmag","Mmag","e_Mmag","Nmag","e_Nmag",$
"LDD","e_LDD","LDD_Chi2","CalFlag","UDDB","UDDV","UDDR","UDDI","UDDJ","UDDH","UDDK","UDDL","UDDM","UDDN","MainId_SIMBAD","SpType_SIMBAD","ObjTypes_SIMBAD","SpType_JMMC","Color_Table_Index"]
jsdc=create_struct(columns,"",       "",       "",0.0   ,0.0     ,0.0   ,0.0     ,0.0   ,0.0   ,$
0.0   ,0.0     ,0.0   ,0.0     ,0.0   ,0.0     ,0.0   ,0.0     ,0.0   ,0.0     ,0.0   ,0.0     ,$
0.0  ,0.0    ,0.0       ,0b       ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,""             ,""             ,""               ,""           ,0L)

if (dotest) then jsdc=create_struct(teststruct,jsdc)

jsdc=replicate(jsdc,n_elements(data_c))

if (doTest) then begin
 jsdc.id1=data_c.id1
 jsdc.ld_diam=data_c.ld_diam
 jsdc.e_ld_diam=data_c.e_ld_diam
 jsdc.notes=data_c.notes
 jsdc.reference=data_c.reference
endif

jsdc.name=data_c.simbad
jsdc.raj2000=data_c.raj2000
jsdc.deJ2000=data_c.dej2000
jsdc.bMag=data_c.b
jsdc.vMag=data_c.v
jsdc.rMag=data_c.r
jsdc.iMag=data_c.i
jsdc.jMag=data_c.j
jsdc.hMag=data_c.h
jsdc.kMag=data_c.k
jsdc.lMag=data_c.l
jsdc.mMag=data_c.m
jsdc.nMag=data_c.n

jsdc.e_bMag=data_c.e_b
jsdc.e_vMag=data_c.e_v
;jsdc.e_rMag=data_c.e_r
;jsdc.e_iMag=data_c.e_i
jsdc.e_jMag=data_c.e_j
jsdc.e_hMag=data_c.e_h
jsdc.e_kMag=data_c.e_k
jsdc.e_lMag=data_c.e_l
jsdc.e_mMag=data_c.e_m
jsdc.e_nMag=data_c.e_n

;jsdc.f_bMag=data_c.b_origin
;jsdc.f_vMag=data_c.v_origin
;jsdc.f_rMag=data_c.r_origin
;jsdc.f_iMag=data_c.i_origin
;jsdc.f_jMag=data_c.j_origin
;jsdc.f_hMag=data_c.h_origin
;jsdc.f_kMag=data_c.k_origin
;jsdc.f_lMag=data_c.l_origin
;jsdc.f_mMag=data_c.m_origin
;jsdc.f_nMag=data_c.n_origin

jsdc.mainId_simbad=data_c.simbad
jsdc.sptype_simbad=data_c.sptype
jsdc.objtypes_simbad=data_c.objtypes
jsdc.sptype_jmmc=data_c.sptype_jmmc
jsdc.Color_Table_Index=data_c.color_table_index
;jsdc.teff_sptype=data_c.teff_sptype
;jsdc.logg_sptype=data_c.logg_sptype

jsdc.ldd=data_c.ldd
jsdc.e_ldd=data_c.e_ldd
jsdc.ldd_chi2=data_c.diam_chi2
w=where(data_c.diam_chi2 gt NSIG_CHI2, count)
if(count gt 0) then jsdc[w].calflag=1
w=where(data_c.sep2 lt 1.0,count)
if(count gt 0) then jsdc[w].calflag+=2;
; filter following objtypes. Note "SB" *is* present.
; LBO: long list of "usual suspects"
ListOfOtypesToRemove=[",EB?,",",Sy?,",",CV?,",",No?,",",pr?,",",TT?,",",C*?,",",S*?,",",OH?,",",CH?,",",WR?,",",Ae?,",",RR?,",",Ce?,",",LP?,",",Mi?,",",sv?,",",pA?,",",WD?,",",N*?,",",BH?,",",SN?,",",BD?,",",EB*,",",Al*,",",bL*,",",WU*,",",EP*,",",SB*,",",El*,",",Sy*,",",CV*,",",NL*,",",No*,",",DN*,",",Ae*,",",C*,",",S*,",",pA*,",",WD*,",",ZZ*,",",BD*,",",N*,",",OH*,",",CH*,",",pr*,",",TT*,",",WR*,",",Ir*,",",Or*,",",RI*,",",Er*,",",FU*,",",RC*,",",RC?,",",Psr,",",BY*,",",RS*,",",Pu*,",",RR*,",",Ce*,",",dS*,",",RV*,",",WV*,",",bC*,",",cC*,",",gD*,",",SX*,",",LP*,",",Mi*,",",sr*,",",SN*,"]
; ListOfOtypesToRemove=[",C*",",S*",",Pu*",",RR*",",Ce*",",dS*",",RV*",",WV*",",bC*",",cC*",",gD*",",SX*",",Mi*","SB*","Al*"]
nn=n_elements(data_c)
ww=bytarr(nn)*0
for i=0,n_elements(ListOfOtypesToRemove)-1 do begin &$
   ww=(ww or  (strpos(data_c.objtypes, ListOfOtypesToRemove[i] ) ge 0)) &$
end
w=where(ww gt 0, count)
if(count gt 0) then jsdc[w].calflag+=4 ;
; thus meaning of bits in flag is: 0 OK, 1:bad karma, 2: binary < 1 as
; 4 : SB, Algol, Mira or any other suspect type
; bands start at 1 in u. Q is 12.
jsdc.uddb=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,2)
jsdc.uddv=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,3)
jsdc.uddr=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,4)
jsdc.uddi=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,5)
jsdc.uddj=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,6)
jsdc.uddh=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,7)
jsdc.uddk=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,8)
jsdc.uddl=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,9)
jsdc.uddm=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,10)
jsdc.uddn=to_udd(data_c.ldd,data_c.color_table_index,data_c.lum_class,11)
; write jsdc as fits
if (dotest) then outputCatalog="DatabaseAsJSDC2.fits" else outputCatalog="JSDC2.fits"
print,'Writing final product "'+outputCatalog+'"'
mwrfits,jsdc,outputCatalog,/CREATE
EXIT,status=0

end

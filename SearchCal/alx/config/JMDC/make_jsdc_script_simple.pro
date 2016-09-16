PRO MAKE_JSDC_SCRIPT_SIMPLE, Database , nopause=nopause, verbose=verbose
;
if (!version.release lt 8.0) then message,"This procedure needs IDL >= 8.0"
@ jsdc_define_common.pro
;
;  Database="JMDC_final_lddUpdated.fits"
  Database=file_basename(Database,".fits")+".fits"
  dowait=(n_elements(nopause) le 0)
  doprint=(n_elements(verbose) gt 0)
  red='0000FF'x & yellow='00FFFF'x & green='00FF00'x & blue='FF0000'x & violet='FF00FF'x
  COLORTABLE=[blue,yellow,green,red,violet]
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
  LUM_CLASSES=0 & DEG=5 & NSIG=3.5 & EMAG_MIN=0.001 & STAT=0 & SNR=5; parameters 

  PRINT,"CHI2 limit for selection:"+STRING(NSIG)

  DATA_B=MRDFITS(Database,1,HEADER) ; restore diameter database from file with new spectral index classification, zero index for SPTYPE="O0.0"
  nn=n_elements(data_b) & print,"Database consists of "+strtrim(nn,2)+" observations."
; database filtering:
; 1) some faint stars have no e_v: put them at 0.04
 ok=where(~finite(data_b.e_v), count) & if (count gt 0) then data_b.e_v[ok]=0.04d 
;DO NOT filter SB9 stars
;ok=where( strlen(strcompress(data_b.sbc9,/remove_all)) lt 1, count) 
;if (count gt 0) then data_b=data_b[ok]
;filter presence sep2 ou sep1 < 1 sec: sufficient to filter sb9 usually
  w=where(data_b.sep2 lt 1, count, comp=ok) 
  if (count gt 0) then data_b=data_b[ok]
; filter following objtypes. Note "SB" is not present.
  ListOfOtypesToRemove=[",C*",",S*",",Pu*",",RR*",",Ce*",",dS*",",RV*",",WV*",",bC*",",cC*",",gD*",",SX*",",Mi*"]
  nn=n_elements(data_b)
  ww=bytarr(nn)*0
  for i=0,n_elements(ListOfOtypesToRemove)-1 do begin 
     ww=(ww or  (strpos(data_b.objtypes, ListOfOtypesToRemove[i] ) ge 0))
  end
  ok=where(ww eq 0) & data_b=data_b[ok]

  nn=n_elements(data_b) & print,"After removing some ObjTypes, we have "+strtrim(nn,2)+" observations left."

; Compute polynom coefficents & covariance matrix from database
  USEDBANDS=[1,3,4,5] & IBAND=[1,1,1] & JBAND=[3,4,5] & NCOLORS=N_ELEMENTS(IBAND)
  NSPECTRALTYPES=250            ; 40 per SPTYPE, 4 per subtype.
  E_SPECTRAL_DSB=DBLARR(NSPECTRALTYPES,NCOLORS) & SPECTRAL_DSB=DBLARR(NSPECTRALTYPES,NCOLORS)

; normally we should check these bands exits in the data...

;
; Interstellar reddening coefficients in COMMON
; LBO: Fix extinction coefficients: use explicit computation from
; literature values:
  CF=DBLARR(6)                  ; Rc coefficients
;CF=[1.32,1.0,0.48,0.28,0.17,0.12] & CI=CF(IBAND)/(CF(IBAND)-CF(JBAND)) & CJ=CF(JBAND)/(CF(IBAND)-CF(JBAND))
  CF[0]=4.10D & CF[1]=3.1D & CF[2]=1.57D & CF[3]=0.86D & CF[4]=0.53D & CF[5]=0.36D
  CF/=3.1D                      ; divide by Rv
  CI=CF[IBAND]/(CF[IBAND]-CF[JBAND]) & CJ=CF[JBAND]/(CF[IBAND]-CF[JBAND])

; INITIALIZE the dataBase (all arrays subscripted by _B) here instead
; of in the routines.
  MAG_B=[TRANSPOSE(DATA_B.B),TRANSPOSE(DATA_B.V),TRANSPOSE(DATA_B.ICOUS),TRANSPOSE(DATA_B.J),TRANSPOSE(DATA_B.H),TRANSPOSE(DATA_B.K)]
  EMAG_B=[TRANSPOSE(DATA_B.E_B),TRANSPOSE(DATA_B.E_V),TRANSPOSE(DATA_B.E_ICOUS),TRANSPOSE(DATA_B.E_J),TRANSPOSE(DATA_B.E_H),TRANSPOSE(DATA_B.E_K)]
  LUMCLASS_B=DATA_B.LUM_CLASS & DLUMCLASS_B=DATA_B.LUM_CLASS_DELTA & SPTYPE_B=DOUBLE(DATA_B.COLOR_TABLE_INDEX) & DSPTYPE_B=DOUBLE(DATA_B.COLOR_TABLE_DELTA)
  MAG_B=TRANSPOSE(MAG_B) & EMAG_B=ABS(TRANSPOSE(EMAG_B)) & DIAM_I=DATA_B.LD_MEAS & EDIAM_I=DATA_B.E_LD_MEAS 
  PARAMS=DBLARR(NCOLORS,DEG+1) & E_PARAMS=PARAMS
  NSTAR_B=N_ELEMENTS(MAG_B[*,0]) 
  DIAM_B=DBLARR(NSTAR_B,NCOLORS) & EDIAM_B=DIAM_B & CHI2_MD=DBLARR(NSTAR_B) & DMEAN_B=DBLARR(NSTAR_B) & EDMEAN_B=DMEAN_B 
  RES_B=DBLARR(NSTAR_B,NCOLORS)-100 & RES_C=RES_B 
; correction of database from too low photometric errors:
  A=EMAG_B[*,usedbands] & S=WHERE(A LT EMAG_MIN, COUNT) & if(count gt 0) then A[S]=EMAG_MIN & EMAG_B[*,usedbands]=A ; magnitude error correction

; start info on database health
  PRINT, "Statistics on stars used:"
  STAR_ID=DATA_B.SIMBAD & BY_STARS=STAR_ID[UNIQ(STAR_ID,SORT(STAR_ID))] & PRINT,STRING(N_ELEMENTS(STAR_ID))+" data points with "+STRTRIM(N_ELEMENTS(BY_STARS),2),+" different stars"
; normally only the subset USEDBANDS should be checked for Nans.
  USEABLE_MEASUREMENTS=WHERE(FINITE(TOTAL(MAG_B[*,usedbands],2)) AND FINITE(TOTAL(EMAG_B[*,usedbands],2)) AND SPTYPE_B NE -1 AND DIAM_I NE -1 AND EDIAM_I NE -1 AND DIAM_I/EDIAM_I GT SNR, NUSEABLE)
; number of unique stars in this useable list  
  BY_U_STARS=STAR_ID[USEABLE_MEASUREMENTS] & BY_STARS=BY_U_STARS[UNIQ(BY_U_STARS,SORT(BY_U_STARS))] & PRINT,STRING(NUSEABLE)+" observations with correct measurements on "+STRTRIM(N_ELEMENTS(BY_STARS),2)+" different stars"

; run modeling, first freely to find sigmas;
  MODE='VAR' & MAKE_JSDC_POLYNOMS,RESIDU,E_RESIDU

; LBO then fix the used stars to some sigma:
  S=WHERE(CHI2_MD[GOOD_B] LT NSIG) & GOOD_B=GOOD_B[S] ; select stars with chi2 smaller than NSIG
  MODE='FIX' & GOOD_FIX=GOOD_B & MAKE_JSDC_POLYNOMS,RESIDU,E_RESIDU

; Results 
  NOUT=N_ELEMENTS(GOOD_B) &  BY_STARS=STAR_ID[GOOD_B]  & BY_STARS=BY_STARS[UNIQ(BY_STARS,SORT(BY_STARS))] & PRINT," ; At the end, selected data points :"+STRTRIM(NOUT,2)+" data points, on "+STRTRIM(N_ELEMENTS(BY_STARS),2)+" distinct stars"
  STAR_IDC=STAR_ID[GOOD_B]
  S1=WHERE(LUMCLASS_B[GOOD_B] EQ 1,  N1) & Q1=STAR_IDC[S1] & Q1=Q1[UNIQ(Q1,SORT(Q1))] & PRINT,"lumClass   I: "+STRTRIM(N1,2)+" points, "+STRTRIM(N_ELEMENTS(Q1),2)+" stars" 
  S2=WHERE(LUMCLASS_B[GOOD_B] EQ 2,  N2) & Q2=STAR_IDC[S2] & Q2=Q2[UNIQ(Q2,SORT(Q2))] & PRINT,"lumClass  II: "+STRTRIM(N2,2)+" points, "+STRTRIM(N_ELEMENTS(Q2),2)+" stars"
  S3=WHERE(LUMCLASS_B[GOOD_B] EQ 3,  N3) & Q3=STAR_IDC[S3] & Q3=Q3[UNIQ(Q3,SORT(Q3))] & PRINT,"lumClass III: "+STRTRIM(N3,2)+" points, "+STRTRIM(N_ELEMENTS(Q3),2)+" stars"
  S4=WHERE(LUMCLASS_B[GOOD_B] EQ 4,  N4) & Q4=STAR_IDC[S4] & Q4=Q4[UNIQ(Q4,SORT(Q4))] & PRINT,"lumClass  IV: "+STRTRIM(N4,2)+" points, "+STRTRIM(N_ELEMENTS(Q4),2)+" stars"
  S5=WHERE(LUMCLASS_B[GOOD_B] EQ 5,  N5) & Q5=STAR_IDC[S5] & Q5=Q5[UNIQ(Q5,SORT(Q5))] & PRINT,"lumClass   V: "+STRTRIM(N5,2)+" points, "+STRTRIM(N_ELEMENTS(Q5),2)+" stars"
  S0=WHERE(LUMCLASS_B[GOOD_B] EQ -1, N0) & Q0=STAR_IDC[S0] & Q0=Q0[UNIQ(Q0,SORT(Q0))] & PRINT,"lumClass ???: "+STRTRIM(N0,2)+" points, "+STRTRIM(N_ELEMENTS(Q0),2)+" stars"
  
  A=[S1,S2,S3,S4,S5,S0] & B=[Q1,Q2,Q3,Q4,Q5,Q0] & PRINT,"Total   "+STRTRIM(N_ELEMENTS(A),2)+" points, "+STRTRIM(N_ELEMENTS(B),2)+" stars"

; 2/ Plot residuals & fit band per band
  window,0
  !P.MULTI=[0,2,2] & X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X),/FILL
  Y1=DINDGEN(NSPECTRALTYPES)/4. 
  FOR N=0,N_ELEMENTS(RESIDU[0,*])-1 DO BEGIN
     A=WHERE(Y1 GE MIN(SPTYPE_B[GOOD_B]/4.) AND Y1 LE MAX(SPTYPE_B[GOOD_B]/4.))
     PLOT,SPTYPE_B[GOOD_B]/4.,RESIDU[GOOD_B,N],PSYM=8,YRANGE=[0,1.6],YSTYLE=1,XRANGE=[0,70],XSTYLE=1,XTITLE='SPECTRAL TYPE',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0']
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
 !P.MULTI=[0,1,4] 

  SPM=DBLARR(300)-100 & RM=DBLARR(300,10)-100 & ERM=RM
  NPMIN=1 & RRR=RM & PARAM=DBLARR(NCOLORS,DEG+1) & E_PARAM=PARAM
  A=DINDGEN(N_ELEMENTS(GOOD_B))

  BIN=12 & HH=HISTOGRAM(SPTYPE_B[GOOD_B[A]],BINSIZE=BIN,LOCATIONS=XX,REVERSE_INDICES=R,MIN=-BIN/2.) & XX=XX+BIN/2.
  FOR II=0, N_ELEMENTS(XX)-1 DO BEGIN
     IF (R[II+1]-R[II] GE NPMIN) THEN BEGIN
        T=R[R[II]:R[II+1]-1] & SPM[II]=MEDIAN(SPTYPE_B[(GOOD_B[A[T]])])
        FOR N=0, NCOLORS-1 DO BEGIN
           ERM[II,N]=1./SQRT(TOTAL(1./E_RESIDU[GOOD_B[A[T]],N]^2)) & RM[II,N]=TOTAL(RESIDU[GOOD_B[A[T]],N]/E_RESIDU[GOOD_B[A[T]],N]^2)*ERM[II,N]^2
           IF (N EQ 1) THEN SPM[II]=TOTAL(SPTYPE_B[GOOD_B[A[T]]]/E_RESIDU[GOOD_B[A[T]],N]^2)*ERM[II,N]^2
        ENDFOR
        IF (DOPRINT) THEN PRINT,SPM[II]/4.,N_ELEMENTS(T)
     ENDIF
  ENDFOR

  S=WHERE(SPM NE -100) & NS=N_ELEMENTS(S) & YFIT=DBLARR(NSPECTRALTYPES,NCOLORS) & XFIT=YFIT & FIT=DBLARR(NS,NCOLORS)

  FOR N=0, NCOLORS-1 DO BEGIN
     MDAT=RM[S,N]/ERM[S,N] & VEC=DBLARR(NS,DEG+1) & FOR KK=0, DEG DO VEC[*,KK]=SPM[S]^KK/ERM[S,N]
     INV=INVERT(TRANSPOSE(VEC)#VEC,/DOUBLE,STATUS) & PARAM[N,*]=INV#TRANSPOSE(VEC)#MDAT & R=DINDGEN(DEG+1) & E_PARAM[N,*]=SQRT(INV[R,R])
     YMIN=0 & IF (N EQ 0) THEN YMAX=1.0 & IF (N NE 0) THEN YMAX=1.0
     FOR KK=0, DEG DO FIT[*,N]=FIT[*,N]+PARAMS[N,KK]*SPM[S]^KK
     FOR KK=0, DEG DO YFIT[*,N]=YFIT[*,N]+PARAMS[N,KK]*DINDGEN(NSPECTRALTYPES)^KK & XFIT=DINDGEN(NSPECTRALTYPES)
     PLOTERR,SPM[S]/4,RM[S,N],ERM[S,N],PSYM=8,NOHAT=1,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX],XTITLE='SPECTRAL TYPE',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0']
     OPLOT,SPM[S]/4,FIT[*,N],LINESTYLE=1
     RRR[S,N]=(RM[S,N]-FIT[*,N])/ERM[S,N] & CHI2=MEAN(RRR[S,N]^2) & SIG=SIGMA(RM[S,N]-FIT[*,N]) & IF (DOPRINT) THEN PRINT,N,N_ELEMENTS(S),CHI2,SIG*ALOG(10)
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

  OPLOT,ZZ,ZZ,COLOR=blue,THICK=2 & XYOUTS,-0.5,1.5,'CHI2 = '+string(CHI2,format='(F5.2)'),CHARSIZE=1.5
;
  BIN=0.3 & HH=HISTOGRAM(RES_B[GOOD_B,*],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10,XRANGE=[-5,5],THICK=2,$
     XTITLE='HISTOGRAM OF NORMALIZED RESIDUALS',YTITLE='COUNTS',CHARSIZE=1.5,XSTYLE=1 
  RES=GAUSSFIT(XX,HH,NTERMS=3,Z,SIGMA=MYSIGMA) & OPLOT,XX,RES,THICK=4 & IF (DOPRINT) THEN PRINT,Z
  XYOUTS,-4,NSPECTRALTYPES,'SIGMA = ??'+string(mysigma[0],format='(F5.2)'),CHARSIZE=1.5

  for icolor=0,NCOLORS-1 do begin
     HH=HISTOGRAM(RES_B[GOOD_B,icolor],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & RES=GAUSSFIT(XX,HH,NTERMS=3,Z) & OPLOT,XX,RES*3,COLOR=COLORTABLE[icolor mod 5],THICK=2 & IF (DOPRINT) THEN PRINT,Z 
  END
  !P.MULTI=0

; LBO: PRINT POLYNOMS & Covariance matrix (SearchCal)
; generate COLORS
  MAG_BAND=['B','V','I','J','H','K']
  SCOLORS=STRARR(NCOLORS)
  FOR II=0, NCOLORS-1 DO SCOLORS[II]=MAG_BAND[IBAND[II]]+"-"+MAG_BAND[JBAND[II]]
  IF (DOPRINT) THEN PRINT,"#"
  IF (DOPRINT) THEN PRINT,"#FIT COLORS: ",SCOLORS
  
  S=WHERE(CHI2_MD[GOOD_B] LE NSIG, NS) & CHI2_DMEAN=MEAN(CHI2_MD[GOOD_B[S]])
  IF (DOPRINT) THEN PRINT,"#domain:", MIN(SPTYPE_B[GOOD_B]), MAX(SPTYPE_B[GOOD_B])
  
  IF (DOPRINT) THEN PRINT,"# Polynom coefficients ("+STRTRIM(DEG)+"th degree) from idl fit on ", N_ELEMENTS(GOOD_B), " stars."
  IF (DOPRINT) THEN PRINT,"# CHI2_POLYNOM: ",CHI2_POL
  IF (DOPRINT) THEN PRINT,"# CHI2_DMEAN  : ",CHI2_DMEAN
  
  IF (DOPRINT) THEN PRINT,"#Color a0 a1 a2 a3 a4 etc..."
  
  myformat='(A,'+STRTRIM(DEG+1)+'(G))' & FOR II=0, NCOLORS-1 DO IF (DOPRINT) THEN PRINT,format=myformat,SCOLORS[II],PAR[II,*]
  
  IF (((DEG+1) * NCOLORS) NE 20) THEN IF (DOPRINT) THEN PRINT,"FIX Covariance matrix output"
  
  IF (DOPRINT) THEN PRINT,"# Covariance matrix of polynom coefficients [a0ij...a4ij][a0ij...a1ij] (i < 10)"
  format='(I,'+STRTRIM((DEG+1)*NCOLORS)+'(G))' & FOR II=0, (((DEG+1) * NCOLORS) -1) DO IF (DOPRINT) THEN PRINT,format=format,II,MCOV_POL[II,*]
  
; output catalog as fits file: replace LDD with DMEAN_C, E_LDD with
; EDMEAN_C and DIAM_CHI2 with some chi2.
  data_b.diam_chi2=MEAN(RES_B^2,dimension=2)
  bad=where(data_b.diam_chi2 ge 10000, count)               
  if (count gt 0 ) then data_b[bad].diam_chi2=-1

  ln_10=alog(10.)
;dmean_b=ln(ldd)/ln_10 -> ldd=exp(dmean_b*ln_10)
  data_b.LDD=exp(dmean_b*ln_10)
  
;edmean_b = sigma(dmean_b) = sigma(ln(ldd))/ln_10  =
;sigma(ldd)/ldd/ln_10 -> sigma(ldd)/ldd = edmean_b*ln_10 ->
;sigma(ldd)=E_LDD= edmean_b*ln_10*ldd
  data_b.E_LDD=edmean_b*ln_10*data_b.LDD
  bad=where(data_b.e_ldd le 0, count) 
  if (count gt 0 ) then begin  data_b[bad].e_ldd=-1 &  data_b[bad].ldd=-1 & end


  mwrfits,data_b,"DataBaseUsed.fits",/CREATE

;
;FIG6 : reconstructed vs measured diameters & histogram of normalized residuals
; 
window,3
!P.MULTI=[0,2,1]
X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X)
ELDI=EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10.) & ZZ=DINDGEN(100)/10-5 & PLOT,ZZ,ZZ,YRANGE=[-1,2],XRANGE=[-1,2],XSTYLE=1,YSTYLE=1,$
XTITLE='INPUT DIAMETER', YTITLE='COMPUTED DIAMETER',CHARSIZE=1.5
OPLOTERR,ALOG10(DIAM_I[GOOD_B]),DMEAN_B[GOOD_B],EDIAM_I[GOOD_B]/DIAM_I[GOOD_B]/ALOG(10),EDMEAN_B[GOOD_B],PSYM=8,NOHAT=1
Q=WHERE((LUMCLASS_B[GOOD_B] EQ 1 OR LUMCLASS_B[GOOD_B] EQ 2 OR LUMCLASS_B[GOOD_B] EQ 3) AND DLUMCLASS_B[GOOD_B] EQ 0)
OPLOTERR,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],EDIAM_I[GOOD_B[Q]]/DIAM_I[GOOD_B[Q]]/ALOG(10),EDMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=green,NOHAT=1
Q=WHERE((LUMCLASS_B[GOOD_B] EQ 4 OR LUMCLASS_B[GOOD_B] EQ 5) AND DLUMCLASS_B[GOOD_B] EQ 0)
OPLOTERR,ALOG10(DIAM_I[GOOD_B[Q]]),DMEAN_B[GOOD_B[Q]],EDIAM_I[GOOD_B[Q]]/DIAM_I[GOOD_B[Q]]/ALOG(10),EDMEAN_B[GOOD_B[Q]],PSYM=8,COLOR=red,NOHAT=1
RR=(ALOG10(DIAM_I[GOOD_B])-DMEAN_B[GOOD_B])/SQRT(EDIAM_I[GOOD_B]^2/DIAM_I[GOOD_B]^2/ALOG(10)^2+EDMEAN_B[GOOD_B]^2) & IF (DOPRINT) THEN PRINT,MEAN(RR^2),MEDIAN(RR^2)
BIN=0.25 & HH=HISTOGRAM(RR,BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10 & RES=GAUSSFIT(XX,HH,NTERMS=3,Z) & OPLOT,XX,RES & IF (DOPRINT) THEN PRINT,Z
!P.MULTI=0

; produce figure 2 of paper (sort of)
  GRAF_RESIDUAL_VS_SPTYPE,RESIDU,SPECTRAL_DSB,E_SPECTRAL_DSB,'figure2'
; sort of fig. 4
  GRAF_DIAM_IN_VS_OUT,'diam_in_vs_out' ; input versus computed diameter  
; LBO: EXIT HERE
  rep='' & READ, 'press any key to finish (and closing all windows)', rep

EXIT,STATUS=0

  LUM_CLASSES=0
  DATA_C=MRDFITS(Database,1,HEADER) & SAVE_FILE='idlsave_Tycho.dat'
  MAKE_JSDC_CATALOG
; output as catalog:
  data_c.diam_chi2=chi2_ds
  bad=where(data_c.diam_chi2 le 0, count)               
  if (count gt 0 ) then data_c[bad].diam_chi2=-1


  ln_10=alog(10.)
  data_c.LDD=exp(dmean_c*ln_10)
  data_c.E_LDD=edmean_c*ln_10*data_c.LDD
  bad=where(data_c.e_ldd le 0, count)               
  if (count gt 0 ) then begin data_c[bad].e_ldd=-1 &  data_c[bad].ldd=-1 & end

  mwrfits,data_c,"DataBaseAsCatalog.fits",/CREATE
  
  EXIT,STATUS=0
END

; ----------------following tests work ok in MAIN mode---------------------------------------------------------------------------
;
; Plots database results (ps)  
  GRAF_HISTO_SPTYPE,DLUMCLASS_B,4,5,'histo_dwarves' ; histogram of database selected stars, class V
  GRAF_HISTO_SPTYPE,DLUMCLASS_B,4,13,'histo_giants' ; histogram of database selected stars, class I & III
  GRAF_RESIDUAL_VS_SPTYPE,RESIDU,SPECTRAL_DSB,E_SPECTRAL_DSB,'residual_vs_sptype'
; ------------> GRAF_LINEAR_DIAM_VS_VMK0,'linear_diam_vs_vmk0'
  GRAF_DIAM_IN_VS_OUT,'diam_in_vs_out' ; input versus computed diameter
; ------------> GRAF_HISTO_DIAM_RESIDUAL,'histo_diam_residual'
; 

; CHOICE:
; EITHER Compute angular diameters: Hipparcos catalogue
;  LUM_CLASSES=0
;  DATA_C=MRDFITS('Hipparcos.fits',1,HEADER) & SAVE_FILE='idlsave_Hipparcos.dat' ; restore diameter database
;  MAKE_JSDC_CATALOG,COLORS_C,RCOLORS_C
;  GOOD_C0=GOOD_C & DLUMCLASS_C=DATA_C.LUM_CLASS_DELTA
;
; OR Compute angular diameters: Tycho catalogue
  LUM_CLASSES=0
  DATA_C=MRDFITS('Tycho.fits',1,HEADER) & SAVE_FILE='idlsave_Tycho.dat'
  MAKE_JSDC_CATALOG
  GOOD_C0=GOOD_C & DLUMCLASS_C=DATA_C.LUM_CLASS_DELTA
; END CHOICE


; output catalog as fits file: replace LDD with DMEAN_C, E_LDD with EDMEAN_C and DIAM_CHI2 with CHI2_DS
data_c.diam_chi2=chi2_ds                   

ln_10=alog(10.)
;dmean_c=ln(ldd)/ln_10 -> ldd=exp(dmean_c*ln_10) 
data_c.LDD=exp(dmean_c*ln_10)
 
;edmean_c = sigma(dmean_c) = sigma(ln(ldd))/ln_10  =
;sigma(ldd)/ldd/ln_10 -> sigma(ldd)/ldd = edmean_c*ln_10 ->
;sigma(ldd)=E_LDD= edmean_c*ln_10*ldd
data_c.E_LDD=edmean_c*ln_10*data_c.LDD
mwrfits,data_c,"OurTycho.fits",/CREATE

stop


; Save results
  SAVE,DATA_B,DATA_C,CF,CI,CJ,IBAND,JBAND,LUM_CLASSES,DEG,NSIG,EMAG_MIN,$
       DIAM_I,EDIAM_I,SPECTRAL_DSB,E_SPECTRAL_DSB,RESIDU,E_RESIDU,PARAMS,E_PARAMS,CHI2_POL,MCOV_POL,GOOD_B,GOOD_C,$
       MAG_B,EMAG_B,CHI2_MD,DMEAN_B,EDMEAN_B,DIAM_B,EDIAM_B,RES_B,SPTYPE_B,LUMCLASS_B,DLUMCLASS_B,$
       MAG_C,EMAG_C,CHI2_DS,DMEAN_C,EDMEAN_C,DIAM_C,EDIAM_C,RES_C,SPTYPE_C,LUMCLASS_C,DLUMCLASS_C,$
       FILEN=SAVE_FILE
;
; Screen plots
;
; 1/  Plot histogram of database lc & sp
;

;

;
RM0=RM & ERM0=ERM & SP0=SPM & S0=S & F0=SPECTRAL_DSB & EF0=E_SPECTRAL_DSB & D0=DMEAN_C & ED0=EDMEAN_C & W0=GOOD_C & C0=CHI2_DS
RM45=RM & ERM45=ERM & SP45=SPM & S45=S & F45=SPECTRAL_DSB & EF45=E_SPECTRAL_DSB & D45=DMEAN_C & ED45=EDMEAN_C & W45=GOOD_C & C45=CHI2_DS
RM123=RM & ERM123=ERM & SP123=SPM & S123=S & F123=SPECTRAL_DSB & EF123=E_SPECTRAL_DSB & D123=DMEAN_C & ED123=EDMEAN_C & W123=GOOD_C & C123=CHI2_DS
;
; Comparison of results from classes (I,II,III) and (IV,V) to results from no selection   
dif123=1.-10^(dc123(good_c)-dc0(good_c)) & bin=0.01 & hh123=histogram(dif123,binsize=0.01,locations=xx123) & xx123=xx123+bin/2. 
plot,xx123,hh123,psym=10 & res123=gaussfit(xx123,hh123,nterms=3,zz123) & oplot,xx123,res123 & if (doprint) then print,zz123
s123=where(dif123 ge -0.04 and dif123 le 0.06) & if (doprint) then print,n_elements(s123)/float(n_elements(good_c)),mean(dif123(s123)),sigma(dif123(s123))
dif45=1.-10^(dc45(good_c)-dc0(good_c)) & bin=0.01 & hh45=histogram(dif45,binsize=0.01,locations=xx45) & xx45=xx45+bin/2. 
plot,xx45,hh45,psym=10 & res45=gaussfit(xx45,hh45,nterms=3,zz45) & oplot,xx45,res45 & if (doprint) then print,zz45
s45=where(dif45 ge -0.1 and dif45 le 0.1) & if (doprint) then print,n_elements(s45)/float(n_elements(good_c)),mean(dif45(s45)),sigma(dif45(s45))
;
;
; 5a/ Plot catalogs Hipparcos & Tycho summary
!P.MULTI=[0,2,1]
X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X), /FILL & T=WHERE(CHI2_DS[GOOD_C] LT NSIG) & HELP,T
PLOT_IO,DMEAN_C[GOOD_C[T]],EDMEAN_C[GOOD_C[T]]*ALOG(10),PSYM=3,YRANGE=[0.01,0.15],YSTYLE=1,XRANGE=[-2.5,1],XSTYLE=1,$
XTITLE='MEAN DIAMETER',YTITLE='RELATIVE DIAMETER ERROR',CHARSIZE=1.5
S=WHERE(CHI2_DS[GOOD_C] LT NSIG) & HELP,S & OPLOT,DMEAN_C[GOOD_C[S]],EDMEAN_C[GOOD_C[S]]*ALOG(10),PSYM=3,COLOR=red
XYOUTS,-2.3,0.1,'MEDIAN RELATIVE ERROR : 1.4 %',CHARSIZE=1.5
XYOUTS,-2.3,0.08,'HIPPARCOS : 92094 STARS',CHARSIZE=1.5
XYOUTS,-2.3,0.065,'TYCHO : 468966 STARS',CHARSIZE=1.5
.RUN
BIN=0.1 & G=WHERE(CHI2_DS[GOOD_C] LT NSIG AND CHI2_DS[GOOD_C] GT 0)
HH=HISTOGRAM(RES_C[GOOD_C[G],*],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2. & RES=GAUSSFIT(XX,HH,NTERMS=3,V)
PLOT,XX,HH,PSYM=10,XRANGE=[-5,5],XSTYLE=1,XTITLE='HISTOGRAM OF SINGLE VS MEAN DIAMETER RESIDUALS (SIGMA UNITS)',$
YTITLE='COUNTS',THICK=2,YRANGE=[0,130000.],YSTYLE=1,CHARSIZE=1.5 & OPLOT,XX,RES,COLOR=red,THICK=2 & IF (DOPRINT) THEN PRINT,V & IF (DOPRINT) THEN PRINT,' '
FOR N=0, NCOLORS-1 DO BEGIN
HH=HISTOGRAM(RES_C[GOOD_C[G],N],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2.
RES=GAUSSFIT(XX,HH,NTERMS=3,V) & OPLOT,XX,RES*2,COLOR=yellow & IF (DOPRINT) THEN PRINT,V
ENDFOR
XYOUTS,-4,110000.,'HIPPARCOS : CHI2=1.0',CHARSIZE=1.5
!P.MULTI=0
END
;
; 5b/ Ajouter distribution de Chi2 (Hipparcos et Tycho)
;
; 5c/ Ajouter table de diametres angulaire vs sptype 
;
; 6d/ Formule approchee : alog10(d)=rcolor+0.65 (+-20%) if nothing is known
;

; contour dmean vs sptype
IF (DOPRINT) THEN PRINT,NP & DEVICE,DECOMPOSED=0
LOADCT,1
CONTOUR,FDMEAN_A,XXA,YYA,XRANGE=[-1.5,6.],XSTYLE=1,YSTYLE=1,nlev=20,/fill,zrange=[-1,1]
DEVICE,DECOMPOSED=1 
;--------------------------------------------------
; End
;--------------------------------------------------
;
; Plot histograms
DEVICE,DECOMPOSED=0 & LOADCT,1 &!P.MULTI=[0,2,2]
CONTOUR,FDMEAN_ALL,XXA_ALL/4,YYA_ALL,XRANGE=[5,67],XSTYLE=1,YRANGE=[-1.,2.5],YSTYLE=1,nlev=32,/fill,zrange=[-1,1]
XYOUTS,10,2,'ALL STARS (SIGMA_DIAM < 0.1) : 64101 OBJECTS',CHARSIZE=1.5
CONTOUR,FDMEAN_NOLC,XXA_NOLC/4,YYA_NOLC,XRANGE=[5,67],XSTYLE=1,YRANGE=[-1.,2.5],YSTYLE=1,nlev=32,/fill,zrange=[-1,1]
XYOUTS,10,2,'NO LUMINOSITY CLASS (SIGMA_DIAM < 0.1) : 29085 OBJECTS',CHARSIZE=1.5
CONTOUR,FDMEAN_LC5,XXA_LC5/4,YYA_LC5,XRANGE=[5,67],XSTYLE=1,YRANGE=[-1.,2.5],YSTYLE=1,nlev=32,/fill,zrange=[-1,1]
XYOUTS,10,2,'DWARVES (SIGMA_DIAM < 0.1) : 23574 OBJECTS',CHARSIZE=1.5
CONTOUR,FDMEAN_LC31,XXA_LC31/4,YYA_LC31,XRANGE=[5,67],XSTYLE=1,YRANGE=[-1.,2.5],YSTYLE=1,nlev=32,/fill,zrange=[-1,1]
XYOUTS,10,2,'GIANTS & SUPERGIANTS (SIGMA_DIAM < 0.1) : 11442 OBJECTS',CHARSIZE=1.5
!P.MULTI=0 & DEVICE,DECOMPOSED=1 
;
; Compare fits
.RUN
!P.MULTI=[0,2,2] & X=FINDGEN(17)*(!PI*2./16.) & USERSYM,0.75*COS(X),0.75*SIN(X), /FILL & M=[0,4,5,6]
REST=REST0(*,M) & E_REST=E_REST0(*,M) & FLC=FLC0(*,M) & EFLC=EFLC0(*,M) & NCOLORS=N_ELEMENTS(REST(0,*)) & Y1=DINDGEN(NSPECTRALTYPES)/4.
FOR N=0,NCOLORS-1 DO BEGIN
A=WHERE(Y1 GE MIN(SPTYPE_B[GOOD_B]/4.) AND Y1 LE MAX(SPTYPE_B[GOOD_B]/4.))
PLOTERR,Y1[A],FLC(A,N),EFLC(A,N),NOHAT=1,PSYM=3,COLOR=green,THICK=2,YRANGE=[0,1],YSTYLE=1,XRANGE=[5,70],XSTYLE=1
OPLOT,XFIT[*,N],YFIT[*,N],COLOR=green
ENDFOR
!P.MULTI=0
END
;

;--------------------------------------------------------------------------------------- 
;
; FIGURES PAPIER I
;
;FIG1 : Spectral histograms of selected measurements (classes 123 and classes 45)
;
RESTORE,FILEN='idlsave_b3_p8.dat'
MAKE_GRAF_HISTOGRAMS,'paper1_histogram_lc'
;
A=WHERE(LUMCLASS_B[GOOD_B] EQ 1 OR LUMCLASS_B[GOOD_B] EQ 2 OR LUMCLASS_B[GOOD_B] EQ 3)
BIN=8 & HH123=HISTOGRAM(SPTYPE_B[GOOD_B[A]],BINSIZE=BIN,LOCATIONS=XX123,MIN=-BIN/2,MAX=NSPECTRALTYPES+BIN/2.) & XX123=XX123+BIN/2.
A=WHERE(LUMCLASS_B[GOOD_B] EQ 4 OR LUMCLASS_B[GOOD_B] EQ 5) & HH45=HISTOGRAM(SPTYPE_B[GOOD_B[A]],BINSIZE=BIN,LOCATIONS=XX45,MIN=-BIN/2.,MAX=NSPECTRALTYPES+2) & XX45=XX45+BIN/2.
PLOT,XX123/4,HH123,PSYM=10,XSTYLE=1,XRANGE=[0,70]
OPLOT,XX123/4,HH123,PSYM=10,COLOR=green & OPLOT,XX45/4,HH45,PSYM=10,COLOR=red
;
;--------------------------------------------------------------------------------------- 
;
;FIG2 : reduced brigthness surface, comparison between classes 123 & 45 for pseudo-colors (V-B, V-J, V-H, V-K) 
; vs spectral type (resolution : 2 sub spectral types) + polynomial model of degre 8
;
RESTORE,FILEN='idlsave_b4_p8.dat'
MAKE_GRAF_COMP_CLASSES, SP123,RM123,ERM123,SP45,RM45,ERM45,F0,'paper1_comp_classes'
;
.RUN
!P.MULTI=[0,1,4] & SG=1. & X=FINDGEN(17)*(!PI*2./16.) & USERSYM,SG*COS(X),SG*SIN(X),/FILL
FOR N=0, NCOLORS-1 DO BEGIN
IF (N EQ 0) THEN BEGIN
YMIN=0.1 & YMAX=1.0 & S0=WHERE(SP0 NE -100)
PLOT,SP0(S0)/4,RM0(S0,N),PSYM=3,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX]
S123=WHERE(SP123 NE -100) & OPLOTERR,SP123(S123)/4,RM123(S123,N),ERM123(S123,N),NOHAT=1,PSYM=8,COLOR=green
S45=WHERE(SP45 NE -100) & OPLOTERR,SP45(S45)/4,RM45(S45,N),ERM45(S45,N),NOHAT=1,PSYM=8,COLOR=red
OPLOT,DINDGEN(NSPECTRALTYPES)/4.,F0(*,N),PSYM=3
ENDIF
IF (N NE 0) THEN BEGIN
YMIN=0.1 & YMAX=0.9 & S0=WHERE(SP0 NE -100)
PLOT,SP0(S0)/4,RM0(S0,N),PSYM=3,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX]
S123=WHERE(SP123 NE -100) & OPLOTERR,(P123(S123)/4,RM123(S123,N),ERM123(S123,N),PSYM=8,NOHAT=1,COLOR=green
S45=WHERE(SP45 NE -100) & OPLOTERR,SP45(S45)/4,RM45(S45,N),ERM45(S45,N),PSYM=8,NOHAT=1,COLOR=red
OPLOT,DINDGEN(NSPECTRALTYPES)/4.,F0(*,N),PSYM=3
ENDIF
ENDFOR
!P.MULTI=0
END
; -->  Fine structures of the order of a few % but similar for classes 45 and 123 
;--------------------------------------------------------------------------------------- 
;
;FIG3: Comparaison of lumInosity classes
;
RESTORE,FILEN='idlsave_jsdc_hipparcos_b3_p8.dat'
MAKE_GRAF_COMP_FITS,D123,ED123,W123,D45,ED45,W45,'paper1_comp_fits'
;
DD=0.05 & A=WHERE(ED45(W45)*ALOG(10) LE DD AND ED123(W45)*ALOG(10) LE DD AND ABS(10.^(D123(W45)-D45(W45))-1) LE 0.1)
BIN=0.005 & HH=HISTOGRAM(10.^(D45(W45(A))-D123(W45(A)))-1,BINSIZE=BIN,LOCATIONS=XX)
XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10,XRANGE=[-0.1,0.1],XSTYLE=1
IF (DOPRINT) THEN PRINT,N_ELEMENTS(A),TOTAL(XX*HH)/TOTAL(HH),SIGMA(10.^(D45(W45(A))-D123(W45(A))))
;
;--------------------------------------------------------------------------------------- 
;
;FIG4 : reconstructed vs new measured diameters measurements (6 degree polynomial): Liste Duvert
; 
;   HIP       HD             REF                LD_MEASURED       LD_COMPUTED    SP_TYPE  LC
;--------------------------------------------------------------------------------------------
;  96459    185351  x  2014ApJ...794...15J     1.133 +- 0.009     1.157 +- 0.106     G9    III   
;  40693     69830  x  2015ApJ...800..115T     0.674 +- 0.014     0.672 +- 0.013     G8     -
; 108859    209458  x  2015MNRAS.447..846B     0.2254 +- 0.0074   0.233 +- 0.0024     G0     V  
;            97671  x  2015A&A...575A..50A     5.08 +- 0.75       4.942 +- 0.480     M3     I    
;            95687  x  2015A&A...575A..50A     3.26 +- 0.5        3.269 +- 0.300     M3     I
;  95898    183589  x  2015A&A...575A..50A     3.04 +- 0.5        2.790 +- 0.222     K5     I  
;  98505    189733  x  2015MNRAS.447..846B     0.3848 +- 0.0046   0.386 +- 0.004   K1.5   V  
;  98954    190658  x  2014A&A...564A...1B     2.519 +- 0.03      2.931 +- 0.251     M2.5  III    
;------------------------------------------------------------------------------------
;
aa=[1.133,0.674,0.2254,5.08,3.26,3.04,0.3848,2.519] & eaa=[0.016,0.014,0.0074,0.75,0.5,0.5,0.0046,0.03]
bb=[1.157,0.672,0.233,4.942,3.269,2.79,0.386,2.931] & ebb=[0.106,0.013,0.0024,0.48,0.300,0.222,0.004,0.251]
if (doprint) then print,total((aa-bb)^2/(eaa^2+ebb^2))/8.
;
;FIG4 : reconstructed vs new measured diameters measurements (5 degree polynomial): Liste Duvert
;
;   HIP       HD             REF                LD_MEASURED       LD_COMPUTED    SP_TYPE  LC
;--------------------------------------------------------------------------------------------
;  96459    185351  x  2014ApJ...794...15J     1.133 +- 0.009     1.157 +- 0.106     G9    III   
;  40693     69830  x  2015ApJ...800..115T     0.674 +- 0.014     0.672 +- 0.013     G8     -
; 108859    209458  x  2015MNRAS.447..846B     0.2254 +- 0.0074   0.233 +- 0.0023     G0     V  
;            97671  x  2015A&A...575A..50A     5.08 +- 0.75       4.942 +- 0.480     M3     I    
;            95687  x  2015A&A...575A..50A     3.26 +- 0.5        3.258 +- 0.300     M3     I
;  95898    183589  x  2015A&A...575A..50A     3.04 +- 0.5        2.789 +- 0.222     K5     I  
;  98505    189733  x  2015MNRAS.447..846B     0.3848 +- 0.0046   0.386 +- 0.004   K1.5   V  
;  98954    190658  x  2014A&A...564A...1B     2.519 +- 0.03      2.931 +- 0.251     M2.5  III    
;------------------------------------------------------------------------------------
;
aa=[1.133,0.674,0.2254,5.08,3.26,3.04,0.3848,2.519] & eaa=[0.016,0.014,0.0074,0.75,0.5,0.5,0.0046,0.03]
bb=[1.157,0.672,0.233,4.942,3.258,2.789,0.386,2.931] & ebb=[0.106,0.013,0.0023,0.48,0.300,0.222,0.004,0.251]
if (doprint) then print,total((aa-bb)^2/(eaa^2+ebb^2))/8.
  ;
HIP=DATA_C.HIP & NHIP=ULONG(HIP) & HD=DATA_C.HD & NHD=ULONG(HD) 
S=WHERE(NHD EQ 183589) & IF (DOPRINT) THEN PRINT,S,10.^DMEAN_C[S],EDMEAN_C[S]*10.^DMEAN_C[S]*ALOG(10)
;
XX=AA & EXX=EAA & YY=BB & EYY=EBB
IF (DOPRINT) THEN PRINT,MEAN((XX-YY)^2/(EXX^2+EYY^2)),SIGMA((XX-YY)/XX)
;
MAKE_GRAF_COMP_VS_MEAS_DIAM,XX,EXX,YY,EYY,'paper1_comp_vs_meas_diam1'
;
;--------------------------------------------------------------------------------------- 
;
;FIG5 : reduced brigthness surface (no luminosity class condition) for pseudo-colors (V-J, V-H, V-K) 
; vs spectral type (resolution : 2.5 sub spectral type) + polynomial model of degre 8
;
RESTORE,FILEN='idlsave_b3b_p8.dat'
MAKE_GRAF_RSB_VS_SPTYPE,SP0,S0,RM0,ERM0,F0,'paper1_rsb_vs_sptype_t1'
;
.RUN
!P.MULTI=[0,1,3]
X=FINDGEN(17)*(!PI*2./16.) & USERSYM,COS(X),SIN(X),/FILL & YMIN=0.1 & YMAX=0.9
FOR N=0, NCOLORS-1 DO BEGIN
PLOTERR,SP0(S0)/4,RM0(S0,N),ERM0(S0,N),PSYM=8,NOHAT=1,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX]
OPLOT,DINDGEN(NSPECTRALTYPES)/4.,F0(*,N),COLOR=red
ENDFOR
!P.MULTI=0
END
;
;--------------------------------------------------------------------------------------- 

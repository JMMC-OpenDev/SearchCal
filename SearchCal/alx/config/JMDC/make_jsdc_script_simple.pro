PRO MAKE_JSDC_SCRIPT_SIMPLE, Database , nopause=nopause, verbose=verbose, nocatalog=nocatalog, test=test
;
DEFSYSV, '!gdl', exists=isGDL
if (!version.release lt 8.0 and ~isGDL) then message,"This procedure needs IDL >= 8.0"
@ jsdc_define_common.pro
;
;  Database="JMDC_final_lddUpdated.fits"
  Database=file_basename(Database,".fits")+".fits"
  dowait=(n_elements(nopause) le 0)
  doprint=(n_elements(verbose) gt 0)
  docatalog=(n_elements(nocatalog) le 0)
  dotest=(n_elements(test) gt 0)
  red='0000FF'x & yellow='00FFFF'x & green='00FF00'x & blue='FF0000'x & violet='FF00FF'x
  COLORTABLE=[blue,yellow,green,red,violet]
; open logfile
  openw,unitlog,"make_jsdc.log",/get_lun

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
  LUM_CLASSES=0 & DEG=4 & NSIG=2.5 & EMAG_MIN=0.03 & EMAG_MAX=0.2 & STAT=0 & SNR=5; parameters 

  PRINTF,UNITLOG,"CHI2 limit for selection:"+STRING(NSIG)

  DATA_B=MRDFITS(Database,1,HEADER) ; restore diameter database from file with new spectral index classification, zero index for SPTYPE="O0.0"
  nn=n_elements(data_b) & printf,unitlog,"Database consists of "+strtrim(nn,2)+" observations."
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

  nn=n_elements(data_b) & printf,unitlog,"After removing some ObjTypes, we have "+strtrim(nn,2)+" observations left."

; Compute polynom coefficents & covariance matrix from database
; 5 colors BVJHK:
;  USEDBANDS=[0,1,3,4,5] & IBAND=[0,0,0] & JBAND=[1,3,4,5] & NCOLORS=N_ELEMENTS(IBAND)
; 4 colors VJHK:
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
  S=WHERE(CHI2_MD[GOOD_B] LT NSIG) & GOOD_B=GOOD_B[S] ; select stars with chi2 smaller than NSIG
  MODE='FIX' & GOOD_FIX=GOOD_B & MAKE_JSDC_POLYNOMS,RESIDU,E_RESIDU

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
     PLOTERR,SPM[S]/4,RM[S,N],ERM[S,N],PSYM=8,NOHAT=1,XRANGE=[0,70],XSTYLE=1,YSTYLE=1,YRANGE=[YMIN,YMAX],XTITLE='SPECTRAL TYPE',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0']
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

  OPLOT,ZZ,ZZ,COLOR=blue,THICK=2 & XYOUTS,-0.5,1.5,'CHI2 = '+strtrim(string(CHI2,format='(F5.2)')),CHARSIZE=1.5
;
  BIN=0.3 & HH=HISTOGRAM(RES_B[GOOD_B,*],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & PLOT,XX,HH,PSYM=10,XRANGE=[-5,5],THICK=2,$
     XTITLE='HISTOGRAM OF NORMALIZED RESIDUALS',YTITLE='COUNTS',CHARSIZE=1.5,XSTYLE=1 
  RES=GAUSSFIT(XX,HH,NTERMS=3,Z,SIGMA=MYSIGMA) & OPLOT,XX,RES,THICK=4 & IF (DOPRINT) THEN PRINT,Z
  XYOUTS,-4,NSPECTRALTYPES,'SIGMA = ??'+string(mysigma[0],format='(F5.2)'),CHARSIZE=1.5

  for icolor=0,NCOLORS-1 do begin
     HH=HISTOGRAM(RES_B[GOOD_B,icolor],BINSIZE=BIN,LOCATIONS=XX) & XX=XX+BIN/2 & RES=GAUSSFIT(XX,HH,NTERMS=3,Z) & OPLOT,XX,RES*3,COLOR=COLORTABLE[icolor mod 5],THICK=2 & IF (DOPRINT) THEN PRINT,Z 
  END
  !P.MULTI=0

; PRINT POLYNOMS & Covariance matrix for SearchCal in cfg mode
; open alxAngDiamPolynomial.cfg file as output
  openw,unitpol,"alxAngDiamPolynomial.cfg",/get_lun,width=2048
; generate COLORS
  MAG_BAND=['B','V','I','J','H','K']
  SCOLORS=STRARR(NCOLORS)
  FOR II=0, NCOLORS-1 DO SCOLORS[II]=MAG_BAND[IBAND[II]]+"-"+MAG_BAND[JBAND[II]]

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
  S=WHERE(CHI2_MD[GOOD_B] LE NSIG, NS) & CHI2_DMEAN=MEAN(CHI2_MD[GOOD_B[S]])
  PRINTF,unitpol,"#domain:", MIN(SPTYPE_B[GOOD_B]), MAX(SPTYPE_B[GOOD_B])
  PRINTF,unitpol,"# Polynom coefficients ("+STRTRIM(DEG,1)+"th degree) from idl fit on "+strtrim(N_ELEMENTS(GOOD_B),1)+" stars."
  PRINTF,unitpol,"# CHI2_POLYNOM: ",CHI2_POL
  PRINTF,unitpol,"# CHI2_DMEAN  : ",CHI2_DMEAN
  PRINTF,unitpol,"#Color a0 a1 a2 a3 a4 etc..."
    myformat='(A,'+STRTRIM(DEG+1,1)+'(1X,G12.4))' & FOR II=0, NCOLORS-1 DO PRINTF,unitpol,format=myformat,SCOLORS[II],PAR[II,*]
  PRINTF,unitpol,"#"
  CLOSE,unitpol
  
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
  PRINTF,unitcovpol,"# MCOV_POL matrix from IDL ["+strtrim(ncolors*(deg+1),1)+" x "+strtrim(ncolors*(deg+1),1)+"]for "+strtrim(NCOLORS,1)+" and "+strtrim(deg+1,1)+" polynomial coefficients (a0...a"+strtrim(deg,1)+")"
  PRINTF,unitcovpol,"#"
  PRINTF,unitcovpol,"# Covariance matrix of polynom coefficients [a0ij...a4ij][a0ij...a1ij] (i < ??)"
  format='(I,'+STRTRIM((DEG+1)*NCOLORS,2)+'(1x,G12.4))' & FOR II=0, (((DEG+1)*NCOLORS) -1) DO PRINTF,unitcovpol,format=format,II,MCOV_POL[II,*]
  PRINTF,unitcovpol,"#"
  CLOSE,unitcovpol

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
  wdelete,0,1,2,3

; use Database as catalog input
 LUM_CLASSES=0
 DATA_C=MRDFITS(Database,1,HEADER)
 MAKE_JSDC_CATALOG
;output as catalog:
 data_c.diam_chi2=chi2_ds
 bad=where(data_c.diam_chi2 le 0, count)               
 if (count gt 0 ) then data_c[bad].diam_chi2=-1


 ln_10=alog(10.)
 data_c.LDD=exp(dmean_c*ln_10)
 data_c.E_LDD=edmean_c*ln_10*data_c.LDD
 bad=where(data_c.e_ldd le 0, count)               
 if (count gt 0 ) then begin data_c[bad].e_ldd=-1 &  data_c[bad].ldd=-1 & end

 mwrfits,data_c,"DataBaseAsCatalog.fits",/CREATE
  
if (~docatalog) then exit,status=0

; Create JSDC v.2 :
  if (dotest) then catalog='Test.fits' else catalog='Catalog.fits'

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
  data_c.E_LDD=edmean_c*ln_10*data_c.LDD
  bad=where(data_c.e_ldd le 0, count)               
  if (count gt 0 ) then begin data_c[bad].e_ldd=-1 &  data_c[bad].ldd=-1 & end

; output a subset of columns as JSDC v.2 for publication.
; columns are: 
columns=[              "NAME","RAJ2000","DEJ2000","Bmag","e_Bmag","f_Bmag","Vmag","e_Vmag","f_Vmag","Rmag","e_Rmag","f_Rmag","Imag","e_Imag","f_Imag",$
"Jmag","e_Jmag","f_Jmag","Hmag","e_Hmag","f_Hmag","Kmag","e_Kmag","f_Kmag","Lmag","e_Lmag","f_Lmag","Mmag","e_Mmag","f_Mmag","Nmag","e_Nmag","f_Nmag",$
"LDD","e_LDD","LDD_Chi2","CalFlag","UDDB","UDDV","UDDR","UDDI","UDDJ","UDDH","UDDK","UDDL","UDDM","UDDN","MainId_SIMBAD","SpType_SIMBAD","ObjTypes_SIMBAD","SpType_JMMC","Teff_SpType","logg_SpType"]
jsdc=create_struct(columns,"",       "",       "",0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,$
0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,0.0   ,0.0     ,0b      ,$
0.0  ,0.0    ,0.0       ,0b       ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,0.0   ,""             ,""             ,""               ,""           ,0.0          ,0.0)

jsdc=replicate(jsdc,n_elements(data_c))

jsdc.name=data_c.name
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
jsdc.e_rMag=data_c.e_r
jsdc.e_iMag=data_c.e_i
jsdc.e_jMag=data_c.e_j
jsdc.e_hMag=data_c.e_h
jsdc.e_kMag=data_c.e_k
jsdc.e_lMag=data_c.e_l
jsdc.e_mMag=data_c.e_m
jsdc.e_nMag=data_c.e_n

jsdc.f_bMag=data_c.b_origin
jsdc.f_vMag=data_c.v_origin
jsdc.f_rMag=data_c.r_origin
jsdc.f_iMag=data_c.i_origin
jsdc.f_jMag=data_c.j_origin
jsdc.f_hMag=data_c.h_origin
jsdc.f_kMag=data_c.k_origin
jsdc.f_lMag=data_c.l_origin
jsdc.f_mMag=data_c.m_origin
jsdc.f_nMag=data_c.n_origin

jsdc.mainId_simbad=data_c.simbad
jsdc.sptype_simbad=data_c.sptype
jsdc.objtypes_simbad=data_c.objtypes
jsdc.sptype_jmmc=data_c.sptype_jmmc
jsdc.teff_sptype=data_c.teff_sptype
jsdc.logg_sptype=data_c.logg_sptype

jsdc.ldd=data_c.ldd
jsdc.e_ldd=data_c.e_ldd
jsdc.ldd_chi2=data_c.diam_chi2
w=where(data_c.diam_chi2 gt NSIG, count)
if(count gt 0) then jsdc[w].calflag=1
w=where(data_c.sep2 lt 1.0,count)
if(count gt 0) then jsdc[w].calflag+=2;
; filter following objtypes. Note "SB" *is* present.
ListOfOtypesToRemove=[",C*",",S*",",Pu*",",RR*",",Ce*",",dS*",",RV*",",WV*",",bC*",",cC*",",gD*",",SX*",",Mi*","SB*","Al*"]
nn=n_elements(data_c)
ww=bytarr(nn)*0
for i=0,n_elements(ListOfOtypesToRemove)-1 do begin 
   ww=(ww or  (strpos(data_c.objtypes, ListOfOtypesToRemove[i] ) ge 0))
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
if (dotest) then mwrfits,jsdc,"TestAsJSDC2.fits",/CREATE else mwrfits,jsdc,"JSDC2.fits",/CREATE

EXIT,status=0

end

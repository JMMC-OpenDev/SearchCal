PRO GRAF_DIAM_IN_VS_OUT, FIG_NAME
@ jsdc_define_common.pro

;
DEVICE, DECOMPOSED = 0
loadct,4
;----- red  green blue
TVLCT, 255, 255, 255, 11 ;white
TVLCT,   0,   0,   0, 10 ;black
TVLCT, 255,   0,   0,  0 ;red
TVLCT,   0, 205, 100,  1 ;green
TVLCT,   0,   0, 255,  2 ;blue
TVLCT, 255,   0, 255,  3 ;magenta
TVLCT, 100,  50, 200,  5 ;purple
TVLCT,   0, 120, 200,  6 ;light blue
TVLCT,   0, 160, 200,  4 ;lighter blue
TVLCT, 100, 100, 200,  7 ;light purple
TVLCT, 220, 220, 220,  8 ;grey
SET_PLOT,'PS'
DEVICE,FILENAME=FIG_NAME+'.ps',/landscape,/COLOR
DEVICE,XSIZE=20.0,YSIZE=20.0         
!P.charsize=0.8/1.5 & !x.charsize=0.8/1.5 & !y.charsize=0.8/1.5 & !p.thick=5 & !x.thick=2.5 & !y.thick=2.5 & !p.charthick=2.5
; ---------------------------------------------------------------------------------------------------------------------------
; Plot
; ---------------------------------------------------------------------------------------------------------------------------
X=[1,1] & Y=[1,1] & A=FINDGEN(32)*(!PI*2./31.) & USERSYM,0.75*COS(A),0.75*SIN(A)
PLOT,X,Y,LINESTYLE=0,BACKGROUND=11,COLOR=10,CHARSIZE=2,CHARTHICK=3,XRANGE=[-1,2],YRANGE=[-1,2],XTITLE='INPUT DIAMETER (log, mas)',$
YTITLE='OUTPUT DIAMETER (log, mas)'
;OPLOT,ALOG10(DIAM_I(GOOD_B)),DMEAN_B(GOOD_B),PSYM=8,COLOR=10,THICK=5
S=WHERE(LUMCLASS_B[GOOD_B] EQ 1) & OPLOT,ALOG10(DIAM_I[GOOD_B[S]]),DMEAN_B[GOOD_B[S]],PSYM=8,COLOR=2,THICK=5
S=WHERE(LUMCLASS_B[GOOD_B] EQ 2) & OPLOT,ALOG10(DIAM_I[GOOD_B[S]]),DMEAN_B[GOOD_B[S]],PSYM=8,COLOR=6,THICK=5
S=WHERE(LUMCLASS_B[GOOD_B] EQ 3) & OPLOT,ALOG10(DIAM_I[GOOD_B[S]]),DMEAN_B[GOOD_B[S]],PSYM=8,COLOR=1,THICK=5
S=WHERE(LUMCLASS_B[GOOD_B] EQ 4) & OPLOT,ALOG10(DIAM_I[GOOD_B[S]]),DMEAN_B[GOOD_B[S]],PSYM=8,COLOR=7,THICK=5
S=WHERE(LUMCLASS_B[GOOD_B] EQ 5) & OPLOT,ALOG10(DIAM_I[GOOD_B[S]]),DMEAN_B[GOOD_B[S]],PSYM=8,COLOR=0,THICK=5
ZZ=-1+DINDGEN(4) & OPLOT,ZZ,ZZ,COLOR=10,THICK=1
;XYOUTS,5,8,'DWARVES: 119 STARS',CHARSIZE=1.,CHARTHICK=3,COLOR=10
; ---------------------------------------------------------------------------------------------------------------------------
DEVICE,/CLOSE & SET_PLOT,'X'
!P.charsize=1. & !x.charsize=1. & !y.charsize=1. & !p.thick=1. & !x.thick=1. & !y.thick=1. & !p.charthick=1.
DEVICE, DECOMPOSED = 1
RETURN
END

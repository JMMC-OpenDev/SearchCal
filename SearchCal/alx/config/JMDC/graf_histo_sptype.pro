PRO GRAF_HISTO_SPTYPE, DLC,BIN,LC,FIG_NAME
; bin=4
; lc=5 or lc=13
; fig_name = string
;
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
DEVICE,XSIZE=20.0,YSIZE=20.         
;
!P.charsize=0.8/1.5
!x.charsize=0.8/1.5
!y.charsize=0.8/1.5
!p.thick=5
!x.thick=2.5
!y.thick=2.5
!p.charthick=2.5
;
X=[1,1] & Y=[1,1] 
IF (LC EQ 5) THEN BEGIN
S=WHERE(LC_B(WWD) EQ 5 AND DLC(WWD) EQ 0) & HH=HISTOGRAM(SP_B(WWD(S)),BINSIZE=BIN,LOCATIONS=XX,MIN=-2,MAX=260) & XX=XX+BIN/2.
PLOT,X,Y,LINESTYLE=0,BACKGROUND=11,COLOR=10,CHARSIZE=2,CHARTHICK=3,XRANGE=[0,70],YRANGE=[0,15],XTITLE='SPECTRAL TYPE',$
YTITLE='NUMBER OF STARS',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0'],THICK=5
OPLOT,XX/4,HH,PSYM=10,COLOR=10,THICK=5 & XYOUTS,5,13,'DWARVES: 80 STARS (106 POINTS)',CHARSIZE=1.,CHARTHICK=3,COLOR=10
ENDIF
IF (LC EQ 13) THEN BEGIN
S=WHERE((LC_B(WWD) EQ 1 OR LC_B(WWD) EQ 3) AND DLC(WWD) EQ 0)
HH=HISTOGRAM(SP_B(WWD(S)),BINSIZE=BIN,LOCATIONS=XX,MIN=-2,MAX=260) & XX=XX+BIN/2.
PLOT,X,Y,LINESTYLE=0,BACKGROUND=11,COLOR=10,CHARSIZE=2,CHARTHICK=3,XRANGE=[0,70],YRANGE=[0,35],YSTYLE=1,XTITLE='SPECTRAL TYPE',$
YTITLE='NUMBER OF STARS',XTICKV=DINDGEN(7)*10,XTICKS=6,XTICKNAME=['O0','B0','A0','F0','G0','K0','M0'],THICK=5
OPLOT,XX/4,HH,PSYM=10,COLOR=10,THICK=5 & XYOUTS,5,32,'GIANTS & SUPERGIANTS: 166 STARS (282 POINTS)',CHARSIZE=1.,CHARTHICK=3,COLOR=10
ENDIF
;
DEVICE,/CLOSE
SET_PLOT,'X'
;
!P.charsize=1.
!x.charsize=1.
!y.charsize=1.
!p.thick=1.
!x.thick=1.
!y.thick=1.
!p.charthick=1.
DEVICE, DECOMPOSED = 1
RETURN
END
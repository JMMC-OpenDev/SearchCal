; IDL interpolate:

; IDX: 
; O0 - O9 => [00..09]
; B0 - B9 => [10..19]
; A0 - A9 => [20..29]
; F0 - F9 => [30..39]
; G0 - G9 => [40..49]
; K0 - K9 => [50..59]
; M0 - M9 => [60..69]

;# IDX   B-V     V-Ic    V-R     Ic-Jc   Jc-Hc   Jc-Kc   Kc-L     L-M    Mv      M ; "SPTYPE"
I=0
NCOLS=11
NROWS=42
table = DBLARR(NROWS,NCOLS)

; alxColorTableForSuperGiantStar.cfg
table(I++,*)=[05,	-0.32,	-0.35,	-0.22,	-0.35,	-0.13,	-0.15,	-0.06,	-0.01,	99.99,	70.00 ]; O5
table(I++,*)=[07,	-0.31,	-0.35,	-0.21,	-0.33,	-0.12,	-0.14,	-0.06,	-0.01,	-6.50,	30.00 ]; O7
table(I++,*)=[08,	-0.30,	-0.33,	-0.19,	-0.33,	-0.11,	-0.13,	-0.06,	-0.01,	99.99,	99.99 ]; O8
table(I++,*)=[09,	-0.27,	-0.29,	-0.18,	-0.29,	-0.11,	-0.12,	-0.06,	-0.01,	-6.50,	99.99 ]; O9
table(I++,*)=[10,	-0.22,	-0.23,	-0.14,	-0.24,	-0.09,	-0.10,	-0.04,	-0.01,	99.99,	99.99 ]; B0
table(I++,*)=[11,	-0.19,	-0.19,	-0.11,	-0.20,	-0.09,	-0.09,	-0.04,	-0.01,	-6.40,	25.00 ]; B1
table(I++,*)=[12,	-0.16,	-0.16,	-0.08,	-0.16,	-0.08,	-0.08,	-0.03,	0.00 ,	99.99,	99.99 ]; B2
table(I++,*)=[13,	-0.13,	-0.12,	-0.05,	-0.13,	-0.07,	-0.06,	-0.03,	0.00 ,	-6.30,	99.99 ]; B3
table(I++,*)=[15,	-0.08,	-0.06,	-0.01,	-0.07,	-0.05,	-0.04,	-0.01,	0.00 ,	-6.20,	20.00 ]; B5
table(I++,*)=[16,	-0.06,	-0.04,	0.00 ,	-0.04,	-0.05,	-0.04,	0.00 ,	0.00 ,	99.99,	99.99 ]; B6
table(I++,*)=[17,	-0.04,	-0.01,	0.02 ,	-0.02,	-0.03,	-0.01,	-0.01,	0.00 ,	-6.20,	99.99 ]; B7
table(I++,*)=[18,	-0.03,	0.00 ,	0.02 ,	-0.01,	-0.04,	-0.01,	-0.01,	0.00 ,	-6.20,	99.99 ]; B8
table(I++,*)=[19,	-0.01,	0.03 ,	0.02 ,	0.02 ,	-0.03,	-0.01,	-0.01,	0.00 ,	99.99,	99.99 ]; B9
table(I++,*)=[20,	0.00 ,	0.08 ,	0.03 ,	0.02 ,	99.99,	0.05 ,	0.02 ,	99.99,	-6.30,	16.00 ]; A0
table(I++,*)=[21,	0.02 ,	0.10 ,	0.06 ,	0.03 ,	99.99,	0.10 ,	0.02 ,	99.99,	99.99,	99.99 ]; A1
table(I++,*)=[22,	0.04 ,	0.12 ,	0.07 ,	0.03 ,	99.99,	0.12 ,	0.02 ,	99.99,	-6.50,	99.99 ]; A2
table(I++,*)=[25,	0.09 ,	0.21 ,	0.13 ,	0.04 ,	99.99,	0.14 ,	0.02 ,	99.99,	-6.60,	13.00 ]; A5
table(I++,*)=[27,	0.12 ,	0.28 ,	0.18 ,	0.11 ,	99.99,	0.16 ,	0.02 ,	99.99,	-6.60,	99.99 ]; A7
table(I++,*)=[30,	0.17 ,	0.33 ,	0.21 ,	0.15 ,	99.99,	0.18 ,	0.02 ,	99.99,	-6.60,	12.00 ]; F0
table(I++,*)=[32,	0.23 ,	0.38 ,	0.27 ,	0.19 ,	99.99,	0.20 ,	0.02 ,	99.99,	-6.60,	99.99 ]; F2
table(I++,*)=[35,	0.32 ,	0.46 ,	0.35 ,	0.24 ,	99.99,	0.25 ,	0.02 ,	99.99,	-6.60,	10.00 ]; F5
table(I++,*)=[37,	0.44 ,	0.51 ,	0.41 ,	0.28 ,	99.99,	0.30 ,	0.03 ,	99.99,	-6.50,	99.99 ]; F7
table(I++,*)=[38,	0.56 ,	0.56 ,	0.45 ,	0.33 ,	99.99,	0.34 ,	0.03 ,	99.99,	99.99,	99.99 ]; F8
table(I++,*)=[40,	0.76 ,	0.66 ,	0.51 ,	0.42 ,	99.99,	0.38 ,	0.04 ,	99.99,	-6.40,	10.00 ]; G0
table(I++,*)=[42,	0.87 ,	0.77 ,	0.58 ,	0.48 ,	99.99,	0.44 ,	0.05 ,	99.99,	-6.30,	99.99 ]; G2
table(I++,*)=[44,	0.97 ,	0.83 ,	0.65 ,	0.57 ,	99.99,	0.47 ,	0.05 ,	99.99,	-6.20,	12.00 ]; G4
table(I++,*)=[45,	1.02 ,	0.86 ,	0.67 ,	0.62 ,	99.99,	0.49 ,	0.06 ,	99.99,	99.99,	99.99 ]; G5
table(I++,*)=[46,	1.06 ,	0.87 ,	0.67 ,	0.62 ,	99.99,	0.49 ,	0.07 ,	99.99,	-6.10,	99.99 ]; G6
table(I++,*)=[48,	1.15 ,	0.89 ,	0.69 ,	0.61 ,	99.99,	0.50 ,	0.08 ,	99.99,	99.99,	99.99 ]; G8
table(I++,*)=[50,	1.24 ,	0.96 ,	0.76 ,	0.67 ,	99.99,	0.54 ,	0.09 ,	99.99,	-6.00,	13.00 ]; K0
table(I++,*)=[51,	1.30 ,	1.02 ,	0.80 ,	0.70 ,	99.99,	0.58 ,	0.10 ,	99.99,	99.99,	99.99 ]; K1
table(I++,*)=[52,	1.35 ,	1.08 ,	0.86 ,	0.75 ,	99.99,	0.61 ,	0.11 ,	99.99,	-5.90,	99.99 ]; K2
table(I++,*)=[53,	1.46 ,	1.22 ,	0.94 ,	0.84 ,	99.99,	0.67 ,	0.11 ,	99.99,	99.99,	99.99 ]; K3
table(I++,*)=[54,	1.53 ,	1.37 ,	1.04 ,	0.94 ,	99.99,	0.70 ,	0.12 ,	99.99,	-5.80,	99.99 ]; K4
table(I++,*)=[55,	1.60 ,	1.68 ,	1.21 ,	1.10 ,	99.99,	0.91 ,	0.12 ,	99.99,	-5.80,	13.00 ]; K5
table(I++,*)=[57,	1.63 ,	1.72 ,	1.22 ,	1.13 ,	99.99,	0.91 ,	0.13 ,	99.99,	-5.70,	99.99 ]; K7
table(I++,*)=[60,	1.63 ,	1.74 ,	1.23 ,	1.15 ,	99.99,	0.90 ,	0.14 ,	99.99,	-5.60,	13.00 ]; M0
table(I++,*)=[61,	1.63 ,	1.82 ,	1.27 ,	1.16 ,	99.99,	0.94 ,	0.14 ,	99.99,	-5.60,	99.99 ]; M1
table(I++,*)=[62,	1.64 ,	1.97 ,	1.33 ,	1.19 ,	99.99,	0.95 ,	0.15 ,	99.99,	-5.60,	19.00 ]; M2
table(I++,*)=[63,	1.64 ,	2.25 ,	1.47 ,	1.33 ,	99.99,	0.99 ,	0.16 ,	99.99,	-5.60,	99.99 ]; M3
table(I++,*)=[64,	1.64 ,	2.74 ,	1.73 ,	1.58 ,	99.99,	0.94 ,	0.17 ,	99.99,	-5.60,	99.99 ]; M4
table(I++,*)=[65,	1.62 ,	3.28 ,	99.99, 	1.81 ,	99.99,	0.94 ,	0.22 ,	99.99,	-5.60,	24.00 ]; M5


PRINT,"NROWS:",NROWS,I
;PRINT,table(0,*)
;PRINT,table(NROWS-1,*)

TRANS=TRANSPOSE(TABLE)

; discard blanking values = NaN
TRANS(WHERE(TRANS EQ 99.99))=!VALUES.F_NAN

X=trans(0,*); IDX
;PRINT,"X: ",X

MIN=MIN(X)
MAX=MAX(X)

PRINT,"MIN: ",MIN, " MAX: ",MAX

; interpolate Y in [MIN..MAX] by 0.25 steps
STEP=0.25
GRID=MIN+FINDGEN((MAX-MIN)/STEP + 1) * STEP
;PRINT,"GRID: ",GRID

NP = N_ELEMENTS(GRID)
PRINT,"NP: ",NP

RESULT = DBLARR(NCOLS,NP)
RESULT(0,*)=GRID


; LOOP on columns
.RUN
FOR II=1, NCOLS-1 DO BEGIN
PRINT,"COL: ",II

Y=trans(II,*); input values
;PRINT,"Y: ",Y

; Interpolate Mv and colors: use /NAN to discard NAN values
;RES = INTERPOL(Y, X, GRID, /NAN, /SPLINE); /SPLINE looks better but may introduce biases
RES = INTERPOL(Y, X, GRID, /NAN); LINEAR interpolation

; indexes where input values are not NAN
Z=WHERE(FINITE(Y))
MIN_X=X(MIN(Z))
MAX_X=X(MAX(Z))

FOR JJ=0, NP-1 DO BEGIN
; discard interpolated values on boundaries
IF (GRID(JJ) LT MIN_X or GRID(JJ) GT MAX_X) THEN RES(JJ)=!VALUES.F_NAN
ENDFOR

PRINT,"MIN X: ",MIN_X," MAX X: ",MAX_X
;PRINT,RES

; Plot the function:
PLOT, X, Y, XRANGE=[MIN-1,MAX+1], XSTYLE=1, psym=-1, THICK=2

; Plot the interpolated values:
OPLOT, GRID, RES, COLOR=64000, psym=-1, thick=1

; copy interpolation results
RESULT(II,*) = RES

WAIT,1

ENDFOR
END


; discard blanking values = 99.99
RESULT(WHERE(FINITE(RESULT) EQ 0))=99.99

OUTPUT=TRANSPOSE(RESULT)

PRINT,"# TC    B-V   V-Ic    V-R   Ic-Jc  Jc-Hc  Jc-Kc  Kc-L    L-M     Mv    M"
FOR II=0, NP-1 DO PRINT,format='(%"%5.2f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f")',OUTPUT(II,*)


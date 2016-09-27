pro load_MuFactors_data
common loaded,mu,code_neilson,code_tradi
mu=mrdfits("MuFactors.fits",1,header)
;12 bands   U,   B,   V,    R,    I,    J,    H,    K,    L,    M,   N,    Q
;           1    2    3     4     5     6     7     8     9    10   11    12
code_neilson=reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
code_tradi  =reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
; for Neilson & Leister, bands are B V R I H K and start at 10 and 16
code_neilson[*,4]=[10,10,11,12,13,14,14,15,-1,-1,-1,-1]
code_neilson[*,2]=[16,16,17,18,19,20,20,21,-1,-1,-1,-1]
; for JMMC1 version, all bands are defined a,d start at col 22 and 33
code_tradi[*,4]=[22,23,24,25,26,27,28,29,30,31,32,32]
code_tradi[*,2]=[33,34,35,36,37,38,39,40,41,42,43,43]
end

function Neilson,lum_class,color_table_index,bandcode
common loaded,mu,code_neilson,code_tradi
if n_elements(mu) lt 1  then load_MuFactors_data
; our basic properties data table for spectral types and luminosity classes
if (lum_class ne 3 and lum_class ne 5) then return,-1
if (bandcode lt 1 or bandcode gt 12) then return,-1
col=code_neilson(bandcode-1,lum_class-1)
if (col ge 0) then return,(mu.(col))[color_table_index-20] ; see comment below for "-20"
return,-1
end

; as in C++ source!!!
function computeRho,coeff
 return, sqrt((1.0 - coeff / 3.0) / (1.0 - 7.0 * coeff / 15.0))
end


function Traditional,lum_class,color_table_index,bandcode
common loaded,mu,code_neilson,code_tradi
if n_elements(mu) lt 1  then load_MuFactors_data
; our basic properties data table for spectral types and luminosity classes
if (lum_class ne 3 and lum_class ne 5) then return,-1
if (bandcode lt 1 or bandcode gt 12) then return,-1
col=code_tradi(bandcode-1,lum_class-1)
if (col ge 0) then return,1.0/computeRho((mu.(col))[color_table_index-20]) ; see comment below for "-20"
return,-1
end

function to_udd,ldd,color_table_index,lum_class,band
common loaded,mu,code_neilson,code_tradi
if n_elements(mu) lt 1  then load_MuFactors_data
sz=n_elements(color_table_index)
udd=fltarr(sz)-1.0
for i=0,sz-1 do begin
   if (ldd[i] gt 0) then begin
; note: if class is not 5 or 3, use 5. This should not be a problem
; (no-class stars are faint, very small stars). As in searchCal.
      if (lum_class[i] ne 5 or  lum_class[i] ne 3) then lum_class[i]=5
; start with Neilson and Leister correction
      val = -1.0
      if (color_table_index[i] ge 20) then begin
         val=Neilson(lum_class[i],color_table_index[i],band)
         if (val lt 0) then val=Traditional(lum_class[i],color_table_index[i],band)
      endif
      if (val gt 0 and ldd[i] gt 0) then udd[i]=ldd[i]*val
   endif
endfor
return,udd
end

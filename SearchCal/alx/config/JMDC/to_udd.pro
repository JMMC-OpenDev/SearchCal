function to_udd,ldd,color_table_index,lum_class,band
common loaded,mu,max_color,code_neilson,code_tradi
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
      if (color_table_index[i] ge 0) then begin
         val=Neilson(lum_class[i],color_table_index[i],band)
         if (val lt 0) then val=Traditional(lum_class[i],color_table_index[i],band)
      endif
      if (val gt 0 and ldd[i] gt 0) then udd[i]=ldd[i]*val
   endif
endfor
return,udd
end

function computeRho,coeff
 return, sqrt((1.0 - coeff / 3.0) / (1.0 - 7.0 * coeff / 15.0))
end


function Traditional,lum_class,color_table_index,bandcode
common loaded,mu,max_color,code_neilson,code_tradi
if n_elements(mu) lt 1  then load_MuFactors_data
; our basic properties data table for spectral types and luminosity
; classes.
; if (lum_class ne 3 and lum_class ne 5) then return,-1
; It is assumed that an unknown class is a class III
if (lum_class le 1) then lum_class=3
if (bandcode lt 1 or bandcode gt 12) then return,-1
if (color_table_index lt -1 or color_table_index ge max_color) then return,-1
col=code_tradi(bandcode-1,lum_class-1)
;if (color_table_index eq 84) then print,color_table_index,col,(mu.(col))[color_table_index]
if (col ge 0) then return,1.0/computeRho((mu.(col))[color_table_index])
return,-1
end

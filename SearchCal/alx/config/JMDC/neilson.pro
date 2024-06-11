function Neilson,lum_class,color_table_index,bandcode
common loaded,mu,max_color,code_neilson,code_tradi
if n_elements(mu) lt 1  then load_MuFactors_data
; our basic properties data table for spectral types and luminosity classes
if (lum_class ne 3 and lum_class ne 5) then return,-1
if (bandcode lt 1 or bandcode gt 12) then return,-1
if (color_table_index ge max_color) then return,-1
col=code_neilson(bandcode-1,lum_class-1)
if (col ge 0) then return,(mu.(col))[color_table_index]
return,-1
end

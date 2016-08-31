pro load_udtold_data
common loaded,mu,code
mu=mrdfits("MuFactors.fits",1,header)
code=reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
;12 bands   U,   B,   V,    R,    I,    J,    H,    K,    L,    M,    N,    Q
code[*,4]=[10,10,11,12,13,14,14,15,-1,-1,-1,-1]
code[*,2]=[16,16,17,18,19,20,20,21,-1,-1,-1,-1]

end

function udtold,lum_class,color_table_index,bandcode
common loaded,mu,code
if n_elements(mu) lt 1  then load_udtold_data
; our basic properties data table for spectral types and luminosity classes
if (lum_class ne 3 and lum_class ne 5) then return,-1
if (bandcode lt 1 or bandcode gt 12) then return,-1
col=code(bandcode-1,lum_class-1)
if (col ge 0) then return,(mu.(col))[color_table_index-20] ; see comment below for "-20"
return,-1
end

pro update_ld_in_JMDC, filename
  test=1
  filename=file_basename(filename,".fits")
  jmdc=mrdfits(filename+".fits",1,header)
  nl=n_elements(jmdc)
  ldmeas=jmdc.LD_meas
  ud_to_ld_3d_coeff=jmdc.ud_to_ld_3d_coeff
; note that this version implies the new values of color_table_index,
; starting at O0=0, hence the '-20' in the udtold function above (table
; starts at O5=index 0 of table. A test is
; provided: at first useage (non-existing value in common), the result
; for val must be the one for a G2V star.
  if (test) then begin
    test=0
    val=fltarr(6) ; bands   U,   V,    R,    I,   H,    K
    band=[1,3,4,5,7,8]
    ref=[0.922,0.94,0.95,0.96,0.976,0.98]
    for i=1,6 do val[i-1]=udtold(5,168,band[i-1]) ; G2V=168
    if (abs(total(val-ref)) gt 1e-10) then message, "inconsistent table indexes, please correct."
  endif
  for i=0,nl-1 do begin
     val=-1
     if ( (jmdc.color_table_index)[i] ge 20 ) then val=udtold((jmdc.lum_class)[i],(jmdc.color_table_index)[i],(jmdc.bandcode)[i])
;this does not work with idl!!
;if (val gt 0 and (jmdc.UD_meas)[i] gt 0) then (jmdc.LD_meas)[i]=(jmdc.UD_meas)[i]/val
                                ;found Neison & Leister coeff, and ud
                                ;existing: proudly compute ldmeas from ud. 
     if (val gt 0 and (jmdc.UD_meas)[i] gt 0) then begin 
        ldmeas[i]=(jmdc.UD_meas)[i]/val
        ud_to_ld_3d_coeff[i]=val
     endif else if ( (jmdc.ld_diam_orig)[i] gt 0) then begin ; take this value instead
        ldmeas[i]=(jmdc.ld_diam_orig)[i]
        ud_to_ld_3d_coeff[i]=-1 ; meaning none used.
     endif
  end
  jmdc.LD_meas=ldmeas
  jmdc.ud_to_ld_3d_coeff=ud_to_ld_3d_coeff
  file_delete,filename+"_lddUpdated.fits",/ALLOW_NONEXISTENT
  mwrfits,jmdc,filename+"_lddUpdated.fits",header
end

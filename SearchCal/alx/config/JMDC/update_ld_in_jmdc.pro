pro load_MuFactors_data
common loaded,mu,code_neilson,code_tradi
mu=mrdfits("MuFactors.fits",1,header)
;12 bands   U,   B,   V,    R,    I,    J,    H,    K,    L,    M,   N,    Q
code_neilson=reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
code_tradi  =reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
; for Neilson & Leister, bands are B V R I H K and start at 10 and 16
code_neilson[*,4]=[10,10,11,12,13,14,14,15,-1,-1,-1,-1]
code_neilson[*,2]=[16,16,17,18,19,20,20,21,-1,-1,-1,-1]
; for JMMC1 version, all bands are defined a,d start at col 22 and 33
code_tradi[*,4]=[22,23,24,25,26,27,28,29,30,31,32,-1]
code_tradi[*,2]=[33,34,35,36,37,38,39,40,41,42,43,-1]
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

pro update_ld_in_JMDC, filename
  test=1
  filename=file_basename(filename,".fits")
  jmdc=mrdfits(filename+".fits",1,header)
  nl=n_elements(jmdc)
  ldmeas=jmdc.LD_meas
  ud_to_ld_3d_coeff=jmdc.ud_to_ld_3d_coeff
; note that this version implies the new values of color_table_index,
; starting at O0=0, hence the '-20' in the Neilson function above (table
; starts at O5=index 0 of table. A test is
; provided: at first useage (non-existing value in common), the result
; for val must be the one for a G2V star.
  if (test) then begin
    test=0
    val=fltarr(6) ; bands   U,   V,    R,    I,   H,    K
    band=[1,3,4,5,7,8]
    ref=[  0.92249936,  0.93962735,  0.94998389,  0.96013469,  0.97642386,  0.98019534]
    for i=1,6 do val[i-1]=Neilson(5,168,band[i-1]) ; G2V=168
    if (abs(total(val-ref)) gt 1e-10) then message, "inconsistent table indexes, please correct."
  endif
  for i=0,nl-1 do begin
     val=-1
     ud_to_ld_3d_coeff[i]=-1 ; meaning none used.
     ; start with Neilson and Leister correction
     if ( (jmdc.color_table_index)[i] ge 20 ) then val=Neilson((jmdc.lum_class)[i],(jmdc.color_table_index)[i],(jmdc.bandcode)[i])
;this does not work with idl!!
;if (val gt 0 and (jmdc.UD_meas)[i] gt 0) then (jmdc.LD_meas)[i]=(jmdc.UD_meas)[i]/val
                                ;found Neilson & Leister coeff, and ud
                                ;existing: proudly compute ldmeas from ud. 
     if (val gt 0 and (jmdc.UD_meas)[i] gt 0) then begin 
        ldmeas[i]=(jmdc.UD_meas)[i]/val
        ud_to_ld_3d_coeff[i]=val
; then original value
     endif else if ( (jmdc.ld_diam_orig)[i] gt 0) then begin ; take this value instead
        ldmeas[i]=(jmdc.ld_diam_orig)[i]
; then traditional correction
     endif else begin
        val=Traditional((jmdc.lum_class)[i],(jmdc.color_table_index)[i],(jmdc.bandcode)[i])
        if (val gt 0 and (jmdc.UD_meas)[i] gt 0) then begin
           ldmeas[i]=(jmdc.UD_meas)[i]/val
           ud_to_ld_3d_coeff[i]=val
        endif
     end
  end
  jmdc.LD_meas=ldmeas
  jmdc.ud_to_ld_3d_coeff=ud_to_ld_3d_coeff
  mwrfits,jmdc,filename+"_lddUpdated.fits",header,/create
end

pro update_ld_in_JMDC, filename
  test=1
  filename=file_basename(filename,".fits")
  jmdc=mrdfits(filename+".fits",1,header)
; test if input contains a column named "UD_TO_LD_CONVFACTOR"
  w=where(tag_names(jmdc) eq "UD_TO_LD_CONVFACTOR", count) & if (count lt 1) then message,'Column "UD_TO_LD_CONVFACTOR" absent, exiting.'
  nl=n_elements(jmdc)
  ldmeas=jmdc.LD_DIAM
; save immediately in LDD_ORIG:
  jmdc.LDD_ORIG=ldmeas
;  
  ud_to_ld_convfactor=jmdc.ud_to_ld_convfactor
; note that this version implies the new values of color_table_index,
; starting at O0=0 and the new MuFactor.fits table starting at O0.00. A test is
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
     ud_to_ld_convfactor[i]=-1 ; meaning none used.
     ; start with Neilson and Leister correction
     if ( (jmdc.color_table_index)[i] ge 0 ) then val=Neilson((jmdc.lum_class)[i],(jmdc.color_table_index)[i],(jmdc.bandcode)[i])
     ;found Neilson & Leister coeff, and ud existing: proudly compute ldmeas from ud. 
     if (val gt 0 and (jmdc.UD_DIAM)[i] gt 0) then begin 
        ldmeas[i]=(jmdc.UD_DIAM)[i]/val
        ud_to_ld_convfactor[i]=val
; then traditional correction...
     end else begin
        val=Traditional((jmdc.lum_class)[i],(jmdc.color_table_index)[i],(jmdc.bandcode)[i])
        if (val gt 0 and (jmdc.UD_DIAM)[i] gt 0) then begin
           ldmeas[i]=(jmdc.UD_DIAM)[i]/val
           ud_to_ld_convfactor[i]=val
        end
     end
; else: the ld_meas value is inchanged and may be OK if present in the publication.
  end
  jmdc.LD_DIAM=ldmeas
  jmdc.ud_to_ld_convfactor=ud_to_ld_convfactor
  mwrfits,jmdc,filename+"_lddUpdated.fits",header,/create
end

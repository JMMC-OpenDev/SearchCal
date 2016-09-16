; for all Teff and Logg values in file "TeffLogg.fits" (itself
; obtained with idl_interpol_teff_logg.pro from the contents of
; alxTableTeffLogg.cfg) add the cu,cv,cr,ci,ch,ck coefficients.
; do it for 3 tables: the FGK Neilson dwarfs, the Neilson Red Giants,
; and then the rest, if possible with the Claret coefficients.

pro addNeilsonToTeffLogg

; our basic properties data table for spectral types and luminosity classes
  tefflogg=mrdfits("TeffLogg.fits",1,header)
  nsptyp=n_elements(tefflogg)   ; is a structure
;Sun G2V = line tefflogg[156]

  files=["Neilson2013FGK.fits","Neilson2013RedGiants.fits"] ; beware order!
  lumclass=[5,3]                                            ; set lumclass
  for i=0,n_elements(files)-1 do begin
     
     skipcols=3 

; select dw, gt or sgt according to Neilson-like table and needs. Here
; we take advantage that TeffLogg is 3 columns dwarf, 3 giant, 3 superg
     datax=tefflogg.(i*3+1)
     datay=tefflogg.(i*3+2)
     dataz=tefflogg.(i*3+3)

; the coeff correction are sampled here
     ng=mrdfits(files[i],1,header)
     teff=ng.teff
     logg=ng.logg
     m=ng.m
;   coefb=ng.KLD_B_
;   coefv=ng.KLD_V_
;   coefr=ng.KLD_R_
;   coefi=ng.KLD_I_
;   coefh=ng.KLD_H_
;   coefk=ng.KLD_K_ ---------> 6 coeffs starting from column 3
     

; since m is a bit restrictive, interpolation is silly on the cube
; borders principally on this axe. duplicate ng's values for min and max m, adding another plane. 
     m_min=min(m)
     w=where(m eq m_min)
     newg1=ng[w]
     newg1.m=0.0  ; ok for all classes
     m_max=max(m)
     w=where(m eq m_max)
     newg2=ng[w]
     if (lumclass[i] eq 5) then newg2.m=1.8 else newg2.m=30.
; augment ng and reset tables
     ng=[ng,newg1,newg2]
; do it for the 2 other axes
     teff_min=min(teff)
     w=where(teff eq teff_min)
     newg1=ng[w]
     newg1.teff=2000.0  ; ok for all classes
     teff_max=max(teff)
     w=where(teff eq teff_max)
     newg2=ng[w]
     newg2.teff=8500.
     ng=[ng,newg1,newg2]
;
     logg_min=min(logg)
     w=where(logg eq logg_min)
     newg1=ng[w]
     if (lumclass[i] eq 5) then newg1.logg=3.5 else newg1.logg=-1.5
     logg_max=max(logg)
     w=where(logg eq logg_max)
     newg2=ng[w]
     if (lumclass[i] eq 5) then newg2.logg=5.0 else newg2.logg=3.5
     ng=[ng,newg1,newg2]

     teff=ng.teff
     logg=ng.logg
     m=ng.m


; find range and sampling of 3d function on each axes: teff=x, logg=y,
; m=z.
     axis=teff
     t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
     xinc=min(t3[uniq(t3,sort(t3))]) & xval=min(axis) & nx=CEIL((max(axis)-xval)/xinc) ; xinc/=2 Nyquist ?
     xref=0.0                                                                          ; first "pixel"
     nx*=2 & xinc/=2
     xx=[indgen(nx)-xref]*xinc+xval
     print,nx,min(xx),max(xx)
     
     axis=logg
     t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
     yinc=min(t3[uniq(t3,sort(t3))]) & yval=min(axis) & ny=CEIL((max(axis)-yval)/yinc) ; yinc/=2 Nyquist ?
     yref=0.0                                                                          ; first "pixel"
     ny*=2 & yinc/=2
     yy=[indgen(ny)-yref]*yinc+yval
     print,ny,min(yy),max(yy)
     
     axis=m
     t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*]
     zinc=min(t3[uniq(t3,sort(t3))]) & zval=min(axis) & nz=CEIL((max(axis)-zval)/zinc) ; zinc/=2 Nyquist ?

     nz*=2 & zinc/=2
     zref=0.0                                                                          ; first "pixel"
     zz=[indgen(nz)-zref]*zinc+zval
     print,nz,min(zz),max(zz)

     QHULL, teff, logg, m, tet, /DELAUNAY
     
     result1=[fltarr(n_tags(ng)-skipcols,nsptyp)]+1.0 ; skip the skipcols first columns
     x=(datax-xval)/xinc+xref                         ;
     y=(datay-yval)/yinc+yref                         ;
     z=(dataz-zval)/zinc+zref                         ;
; we could cut the values exactly within the min-max of the neilson tables...
;     wx=where((datax lt teff_min) or (datax gt teff_max), count)  
;     wy=where((datay lt logg_min) or (datay gt logg_max), count)  
;     wz=where((dataz lt m_min) or (dataz gt m_max), count)  
     for j=0,n_tags(ng)-1-skipcols do begin
        volume= QGRID3(teff, logg, m, ng.(j+skipcols), tet, START=[xval,yval,zval], DIMENSION=[nx,ny,nz], DELTA=[xinc,yinc,zinc] , missing=-1.0) ; skip the 3 first columns of Neilsonxxx.fits 
;transform real coordinates to fractional position in cube pixels.
        zz=interpolate(volume,x,y,z,missing=-1)
        result1[j,*]=zz
     end
; but it's worth trying a bit outside.
;     result1[*,wx]=-1
;     result1[*,wy]=-1
;     result1[*,wz]=-1
; add to struct
     tags=STRING(format='(%"CB%i;CV%i;CR%i;CI%i;CH%i;CK%i")',replicate(lumclass[i],6))
     tags=STRSPLIT(tags,';', /EXTRACT)
     str=create_struct(tags,0.0,1.0,2.0,3.0,4.0,5.0)

     if (n_elements(finalresult1) gt 0) then finalresult1=[finalresult1, result1] else finalresult1=result1
     if (n_elements(str1) gt 0) then str1=create_struct(str1,str) else str1=create_struct(str)
  end

; add tefflogg info
  firstcols=create_struct('Sptype','xxxxxx','Teff5',0.0D,'Logg5',0.0,'M5',0.0,'Teff3',0.0D,'Logg3',0.0,'M3',0.0,'Teff1',0.0D,'Logg1',0.0,'M1',0.0)
  str1=create_struct(firstcols,str1)

; now add historical LDtoUD correction,( independent from mass ) in
; the same spirit:
  files=["alxTableUDCoefficientCorrection.fits","alxTableUDCoefficientCorrection.fits"] 
  lumclass=[5,3]                ; set lumclass

  for i=0,n_elements(files)-1 do begin
     
     skipcols=2 
     
; select dw, gt or sgt according to Neilson-like table and needs. Here
; we take advantage that TeffLogg is 3 columns dwarf, 3 giant, 3 superg
     datax=tefflogg.(i*3+1)
     datay=tefflogg.(i*3+2)
     dataz=tefflogg.(i*3+3)     ; dataz is ignored
     ng=mrdfits(files[i],1,header)
     teff=ng.teff
     logg=ng.logg
;   coefu=ng.uu
;   coefb=ng.ub
;   coefv=ng.uv
;   coefr=ng.ur
;   coefi=ng.ui
;   coefj=ng.uj
;   coefh=ng.uh
;   coefk=ng.uk
;   coefl=ng.ul
;   coefm=ng.um
;   coefn=ng.un ------------> 11 coefs starting from col 2

     
; find range and sampling of 3d function on each axes: teff=x, logg=y, m=z
     axis=teff
     t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
     xinc=min(t3[uniq(t3,sort(t3))]) & xval=min(axis) & nx=CEIL((max(axis)-xval)/xinc) ; xinc/=2 Nyquist ?
     xref=0.0                                                                          ; first "pixel"
     xinc/=2
     nx*=2 ; ok
     xx=[indgen(nx)-xref]*xinc+xval
     print,nx,min(xx),max(xx)
     
     axis=logg
     t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
     yinc=min(t3[uniq(t3,sort(t3))]) & yval=min(axis) & ny=CEIL((max(axis)-yval)/yinc) ; yinc/=2 Nyquist ?
     yinc/=20 ; oversample
     ny*=20 ; 
     yref=0.0                                                                          ; first "pixel"
     yy=[indgen(ny)-yref]*yinc+yval
     print,ny,min(yy),max(yy)

     triangulate, teff, logg, tr, b
     
     result2=fltarr(n_tags(ng)-skipcols,nsptyp); skip the skipcols first columns.
     for j=0,n_tags(ng)-1-skipcols do begin
        surface= trigrid(teff, logg, ng.(j+skipcols), tr, [xinc,yinc],[xval,yval,xval+(nx-xref-1)*xinc,yval+(ny-yref-1)*yinc], missing=-1.0)
;transform real coordinates to fractional position in plane pixels.
        x=(datax-xval)/xinc+xref                       ;
        y=(datay-yval)/yinc+yref
        zz=interpolate(surface,x,y);,missing=-1)
        ww=where(zz lt 0, count) & if (count gt 0) then zz[ww]=-1.0
        result2[j,*]=zz
     end

; add to struct
     tags=STRING(format='(%"UU%i;UB%i;UV%i;UR%i;UI%i;UJ%i;UH%i;UK%i;UL%i;UM%i;UN%i")',replicate(lumclass[i],11))
     tags=STRSPLIT(tags,';', /EXTRACT)
     str=create_struct(tags,0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0)

     if (n_elements(finalresult2) gt 0) then finalresult2=[finalresult2, result2] else finalresult2=result2
     if (n_elements(str2) gt 0) then str2=create_struct(str2,str) else str2=create_struct(str)
  end


; size and populate struct
  strfin=create_struct(str1,str2)
  strfin=replicate(strfin,nsptyp)
  start=0
  for j=0,9 do strfin.(j)=tefflogg.(j) ; 10 cols
  for j=j,n_tags(str1)-1 do strfin.(j)=reform(finalresult1[j-10,*],nsptyp) ; 22 cols
  for j=j,n_tags(strfin)-1 do strfin.(j)=reform(finalresult2[j-22,*],nsptyp)

; make a fits file from all that:
     mwrfits,strfin,'MuFactors.fits',/create  
  end

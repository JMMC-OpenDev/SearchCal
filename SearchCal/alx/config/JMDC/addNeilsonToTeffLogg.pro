pro addNeilsonToTeffLogg

; our basic properties data table for spectral types and luminosity classes
data=mrdfits("TeffLogg.fits",1,header)
;Sun G2V = line data[156]
; select dw, gt or sgt according to Neilson-like table and needs
datax=data.teff_gt
datay=data.logg_gt
dataz=data.m_gt

; the coeff correction are sampled here
ng=mrdfits("Neilson2013RedGiants.fits",1,header)
;ng=mrdfits("Neilson2013FGK.fits",1,header)
teff=ng.teff
logg=ng.logg
m=ng.m
coefb=ng.KLD_B_
coefv=ng.KLD_V_
coefr=ng.KLD_R_
coefi=ng.KLD_I_
coefh=ng.KLD_H_
coefk=ng.KLD_K_

; find range and sampling of 3d function on each axes: teff=x, logg=y, m=z
axis=teff
t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
xinc=min(t3[uniq(t3,sort(t3))]) & xval=min(axis) & nx=CEIL((max(axis)-xval)/xinc)  ; xinc/=2 Nyquist ?
xref=0.0 ; first "pixel"
print,nx,xref,xval,xinc

axis=logg
t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
yinc=min(t3[uniq(t3,sort(t3))]) & yval=min(axis) & ny=CEIL((max(axis)-yval)/yinc) ; yinc/=2 Nyquist ?
yref=0.0 ; first "pixel"
print,ny,yref,yval,yinc

axis=m
t1=axis[uniq(axis,sort(axis))] & t2=shift(t1,1) & t3=(t1-t2)[1:*] 
zinc=min(t3[uniq(t3,sort(t3))]) & zval=min(axis) & nz=CEIL((max(axis)-zval)/zinc) ; zinc/=2 Nyquist ?
zref=0.0 ; first "pixel"
print,nz,zref,zval,zinc

QHULL, teff, logg, m, tet, /DELAUNAY

result=fltarr(n_tags(ng)-3,n_elements(data)); skip the 3 first columns.
for j=0,n_tags(ng)-1-3 do begin
   volume= QGRID3(teff, logg, m, ng.(j+3), tet, START=[xval,yval,zval], DIMENSION=[nx,ny,nz], DELTA=[xinc,yinc,zinc], missing=-1.0)
;a=where( abs(teff-myt) lt 51  and abs(logg-myp) lt 0.2 and  abs(m-mym) lt 0.5, count) & if count gt 0 then print,i,myt,myp,mym,ng[a]
;transform real coordinates to fractional position in cube pixels.
   x=(datax-xval)/xinc+xref                            ;
   y=(datay-yval)/yinc+yref                            ;
   z=(dataz-zval)/zinc+zref                            ;
   result[j,*]=interpolate(volume,x,y,z,missing=-1)    ; interpolate does the job best.
end
stop
PRINT,format='(%"# TC\tcu\tcv\tcr\tci\tch\tck")'
FOR II=0, N_ELEMENTS(DATA)-1 DO PRINT,format='(%"%s\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f")',(DATA.(0))[II],RESULT[*,II]

end

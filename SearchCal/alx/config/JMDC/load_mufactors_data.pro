pro load_MuFactors_data
common loaded,mu,max_color,code_neilson,code_tradi
mu=mrdfits("MuFactors.fits",1,header)
max_color=n_elements(mu)
;12 bands   U,   B,   V,    R,    I,    J,    H,    K,    L,    M,   N,    Q
code_neilson=reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
code_tradi  =reform(replicate(-1,12*5),12,5); I,II,III,IV,V 
; for Neilson & Leister, bands are B V R I H K and start at 10 and 16
code_neilson[*,4]=[10,10,11,12,13,14,14,15,-1,-1,-1,-1]
code_neilson[*,2]=[16,16,17,18,19,20,20,21,-1,-1,-1,-1]
; for JMMC1 version, all bands are defined and start at cols 22 and 33
code_tradi[*,4]=[22,23,24,25,26,27,28,29,30,31,32,-1] ; class V
code_tradi[*,3]=[22,23,24,25,26,27,28,29,30,31,32,-1] ; class IV = class V
code_tradi[*,2]=[33,34,35,36,37,38,39,40,41,42,43,-1] ; class III
code_tradi[*,1]=[33,34,35,36,37,38,39,40,41,42,43,-1] ; class II => III 
code_tradi[*,0]=[33,34,35,36,37,38,39,40,41,42,43,-1] ; class I  => III (see discussion in JMMC memo)
end

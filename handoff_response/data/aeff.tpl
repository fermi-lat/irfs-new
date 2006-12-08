SIMPLE  =                    T /Dummy Created by MWRFITS v1.4a                  
BITPIX  =                    8 /Dummy primary header created by MWRFITS         
NAXIS   =                    0 /No data is associated with this header          
EXTEND  =                    T /Extensions may (will!) be present               
END

XTENSION= 'BINTABLE'           / binary table extension                         
BITPIX  =                    8 / 8-bit bytes                                    
NAXIS   =                    2 / 2-dimensional binary table                     
NAXIS1  =                14552 / width of table in bytes                        
NAXIS2  =                    1 / number of rows in table                        
PCOUNT  =                    0 / size of special data area                      
GCOUNT  =                    1 / one data group (required keyword)              
TFIELDS =                    6 / number of fields in each row                   
EXTNAME = 'EFFECTIVE AREA'     / name of this binary table extension            
TFORM1  = '34E     '           /                                                
TFORM2  = '34E     '           /                                                
TFORM3  = '51E     '           /                                                
TFORM4  = '51E     '           /                                                
TFORM5  = '1734E   '           /                                                
TFORM6  = '1734E   '           /                                                
TDIM6   = '( 34, 51)'          /                                                
TTYPE1  = 'ENERG_LO '          /                                                
TTYPE2  = 'ENERG_HI '          /                                                
TTYPE3  = 'CTHETA_LO '         /                                                
TTYPE4  = 'CTHETA_HI '         /                                                
TTYPE5  = 'EFFAREA '           /                                                
TTYPE6  = 'STAT_MIN '          /                                                
HISTORY Input merit file: allGamma-rep-GR-v7r3p4_concat_CTnew.root              
HISTORY Filter string:                                                          
HISTORY CFITSIO used the following filtering expression to create this table:   
HISTORY allGamma-rep-GR-v7r3p4_concat_CTnew.fits[EVENTS][col McEnergy; McLogEner
HISTORY gy; McXDir; McYDir; McZDir; McDirErr; Tkr1FirstLayer; CTBBestXDir; CTBBe
HISTORY stYDir; CTBBestZDir; CTBBestEnergy; CTBBestEnergyProb; CTBCORE; CTBGAM][
HISTORY CTBGAM > 0.35 && CTBBestEnergyProb > 0.3]                               
EMIN    =                17.78 / [MeV] Minimum energy                           
EMAX    =             177800.0 / [MeV] Maximum energy                           
GAMMA   =                  1.0 / Spectral slope                                 
CTMIN   =                  0.0 / Minimum cos(THETA)                             
CTMAX   =                  1.0 / Maximum cos(THETA)                             
AREA    =                  6.0 / [m**2] Beam area                               
NPHOT   =             19985000 / Number of photons                              
LATCLASS= 'FRONTA  '                                                            
HISTORY TASK: FSELECT on FILENAME: allGamma.fits                                
HISTORY fselect4.4 at 2006-03-03T10:10:52                                       
HISTORY Expression: Tkr1FirstLayer >= 6                                         
HISTORY TASK: FSELECT on FILENAME: allGamma-FRONT.fits                          
HISTORY fselect4.4 at 2006-03-03T10:10:59                                       
HISTORY Expression: CTBGAM > 0.50 && CTBCORE > 0.35 && CTBBestEnergyProb > 0.35 
COMMENT The effective area is assumed not to depend on azimuth                  
COMMENT Errors are assumed to be symmetric                                      
TELESCOP= 'GLAST   '           /                                                
INSTRUME= 'LAT     '           /                                                
HDUCLASS= 'OGIP    '           /                                                
HDUDOC  = 'CAL/GEN/92-019'     /                                                
HDUCLAS1= 'RESPONSE'           /                                                
HDUCLAS2= 'EFF_AREA'           /                                                
HDUVERS = '1.0.0   '           /                                                
EARVERSN= '1992a   '           /                                                
TDIM5   = '( 34, 51)'          /                                                
1CTYP5  = 'ENERGY  '           /Always use log(ENERGY) for interpolation        
2CTYP5  = 'COSTHETA'           /Off-axis angle cosine                           
CREF5   = '(ENERG_LO:ENERG_HI,CTHETA_LO:CTHETA_HI)' /                           
CSYSNAME= 'XMA_POL '           /                                                
CCLS0001= 'BCF     '           /                                                
CDTP0001= 'DATA    '           /                                                
CCNM0001= 'EFF_AREA'           /                                                
CBD10001= 'ENERG(18-177800)MeV' /                                               
CBD20001= 'THETA(0-90)deg'     /                                                
CBD30001= 'PHI(0-360)deg'      /                                                
CVSD0001= '01/03/2006'         /                                                
CVST0001= '00:00:00'           /                                                
CDES0001= 'GLAST LAT effective area for Data Challenge 2' /                     
TUNIT1  = 'MeV     '           /                                                
TUNIT2  = 'MeV     '           /                                                
TUNIT5  = 'm2      '           /                                                
TUNIT6  = 'm2      '           /                                                
END

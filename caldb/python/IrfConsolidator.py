"""
@file IrfConsolidator.py

@brief Consolidate irf files for event types into single files,
modifying header extension names and CALDB boundary keywords so that
they can be correctly indexed in caldb.indx.

@author J. Chiang <jchiang@slac.stanford.edu
"""
# $Header$

import os
import glob
from collections import OrderedDict
import pyfits

class IrfConsolidator(object):
    _instrument = 'LAT'
    _irf_components = ('psf', 'aeff', 'edisp')
    _evtypes = OrderedDict([('FB', ('FRONT', 'BACK')),
                            ('PSF', ('PSF0', 'PSF1', 'PSF2', 'PSF3')),
                            ('EDISP', ('EDISP0', 'EDISP1', 'EDISP2',
                                       'EDISP3'))])
    _cnams = dict([('EFFECTIVE AREA', 'EFF_AREA'),
                   ('PHI_DEPENDENCE', 'PHI_DEP'),
                   ('EFFICIENCY_PARAMS', 'EFFICIENCY_PARS'),
                   ('ENERGY DISPERSION', 'EDISP'),
                   ('EDISP_SCALING_PARAMS', 'EDISP_SCALING'),
                   ('RPSF', 'RPSF'),
                   ('PSF_SCALING_PARAMS', 'PSF_SCALING'),
                   ('FISHEYE_CORRECTION', 'FISHEYE_CORR')])
    def __init__(self, irf_name, irf_dir, out_dir='.'):
        self.irf_name = irf_name
        self.irf_dir = irf_dir
        self.out_dir = out_dir
    def _fix_headers(self, hdulist, evtype):
        for hdu in hdulist[1:]:
            hdu.header['CCNM0001'] = self._cnams[hdu.name]
            hdu.name += ('_' + evtype)
            hdu.header['DETNAM'] = evtype
            hdu.header['INSTRUME'] = self._instrument
            hdu.header['CBD10001'] = 'VERSION(%s)' % self.irf_name
            if 'PSFVER' in hdu.header: hdu.header['PSFVER'] = 3

            
            
    def get_input_files(self, component, evtypes):

        irf_files = []
        for evtype in evtypes:

            irf_dir = self.irf_dir + '_' + evtype
            file_pattern = '%s_%s.fits'%(component,irf_dir)

            print os.path.join(irf_dir,file_pattern)
            
            files = glob.glob(os.path.join(irf_dir,file_pattern))

            if len(files) > 1:
                raise Exception('Multiple IRF files found in subdirectory.')
            elif len(files) == 0:
                print 'No IRF files found for type: ', evtype
            
            irf_files += files
        
        return irf_files

    def process_files(self, dryrun=False):

        outfiles = []
        for component in self._irf_components:
            
            for evtype_base, evtypes in self._evtypes.items():
                
                infiles = self.get_input_files(component, evtypes)
                outfile = os.path.join(self.out_dir,
                                       '_'.join((component, self.irf_name, 
                                                 evtype_base + '.fits')))
                outfile = os.path.abspath(outfile)
                
                if dryrun:
                    print infiles
                    print "writing", outfile
                    print
                    continue

                if len(infiles) != len(evtypes):
                    print 'Wrong number of IRFs found for event type: ', component, evtype_base
                    continue
                
                output = pyfits.open(infiles[0])
                evtype = self._evtypes[evtype_base][0]
                self._fix_headers(output, evtype)
                for evtype, infile in zip(self._evtypes[evtype_base][1:],
                                          infiles[1:]):
                    input = pyfits.open(infile)
                    self._fix_headers(input, evtype)
                    for hdu in input[1:]:
                        output.append(hdu)
                output.writeto(outfile, clobber=True)
                outfiles.append(outfile)

        return outfiles
                
if __name__ == '__main__':
    for irf_name in ['P8R2_SOURCE_V6']: #, 'P8_CLEAN_V5', 'P8_ULTRACLEAN_V5'):
        irf_consolidator = IrfConsolidator(irf_name, irf_name)
        irf_consolidator.process_files(dryrun=False)

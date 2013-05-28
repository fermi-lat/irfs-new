import os
import sys
import glob
import pyfits

def update_keywords(files, ccnm):
    for item in files:
        foo = pyfits.open(item)
        for hdu in foo[1:]:
            try:
                hdu.header['CCNM0001'] = ccnm[hdu.header['EXTNAME']]
                print "modifying", item
            except KeyError:
                pass
        try:
            os.remove(item)
        except OSError:
            pass
        foo.writeto(item)

def inspect_keywords(files):
    for item in files:
        foo = pyfits.open(item)
        for hdu in foo[1:]:
            sys.stdout.write("%s  %s  " % (item, hdu.header['EXTNAME']))
            try:
                print "%s" % hdu.header["CCNM0001"]
            except KeyError:
                print "..."

if __name__ == '__main__':
    ccnm = {"EFFECTIVE AREA" : "EFF_AREA",
            "PHI_DEPENDENCE" : "PHI_DEP",
            "EFFICIENCY_PARAMS" : "EFFICIENCY_PARS",
            "RPSF" : "RPSF",
            "PSF_SCALING_PARAMS" : "PSF_SCALING",
            "EDISP_SCALING_PARAMS" : "EDISP_SCALING"}

    for component in ('ea/aeff', 'psf/psf', 'edisp/edisp'):
        files = glob.glob('%s_P7REP_*_V15_*.fits' % component)
        #inspect_keywords(files)
        update_keywords(files, ccnm)

            

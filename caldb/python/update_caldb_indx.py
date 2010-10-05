#
# Script to add standard set of IRF files to caldb.indx using the udcif FTOOL
#
# $Header$
#
import os
from GtApp import GtApp

udcif = GtApp('udcif')

#suffixes = ("P6_v4_trans", "P6_v4_source",
#            "P6_v5_trans", "P6_v5_source", "P6_v5_diff")
#suffixes = ("P7_v1_diff", "P7_v1_source", "P7_v1_trans", "P7_v1_S3")
#suffixes = ("P6_v8_diff", )
#suffixes = ("P6_v7_trans", "P6_v7_source", "P6_v7_diff")
suffixes = ("p6_v3_dataclean",)

for item in suffixes:
    for prefix, loc in zip(("aeff", "psf", "edisp"), ("ea", "psf", "edisp")):
        os.chdir(loc)
        for section in ("front", "back"):
            os.system("printenv CALDB")
            udcif.run(infile="%s_%s_%s.fits" % (prefix, item, section),
                      cif='../../caldb.indx', editc="no",
                      quality=0, newentries=1)
        os.chdir('..')

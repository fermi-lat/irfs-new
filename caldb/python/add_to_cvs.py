#
# Script to add IRF files to cvs repository based on a standard file suffix
#
# $Header$
#
import os
#suffixes = ("P6_v4_trans", "P6_v4_source", "P6_v4_diff",
#            "P6_v5_trans", "P6_v5_source", "P6_v5_diff")
#suffixes = ("P6_v7_trans", "P6_v7_source", "P6_v7_diff")
suffixes = ("p6_v3_dataclean",)

for item in suffixes:
    for prefix, loc in zip(("aeff", "psf", "edisp"), ("ea", "psf", "edisp")):
        for section in ("front", "back"):
            infile = "%s_%s_%s.fits" % (prefix, item, section)
            os.system("cvs add %s" % os.path.join(loc, infile))

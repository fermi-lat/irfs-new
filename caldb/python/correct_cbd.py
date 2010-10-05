#
# Script to set the VERSION field to the conform to the IRF naming convention.
# This is set by hand by setting the version variable below.
#
# $Header$
#
import os
from GtApp import GtApp

fparkey = GtApp('fparkey')

for subdir, component in zip(("ea", "psf", "edisp"), ("aeff", "psf", "edisp")):
    os.chdir(subdir)
    for section in ("back", "front"):
        for nickname, name in zip(("diff", "source", "trans"),
                                  ("DIFFUSE", "SOURCE", "TRANSIENT")):
            version = "VERSION(P6_V7_%s)" % name
            infile = "%s_P6_v7_%s_%s.fits[1]" % (component, nickname, section)
            fparkey.run(value=version, fitsfile=infile, keyword="CBD10001")
    os.chdir("..")

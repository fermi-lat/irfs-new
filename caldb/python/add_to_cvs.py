#
# Script to add IRF files to cvs repository based on a standard file suffix
#
# $Header$
#
import os
import glob
import subprocess

pattern = "P8R2"

for subdir in "ea edisp psf".split():
    irf_files = sorted(glob.glob(os.path.join(subdir, '*%s*.fits' % pattern)))
    for item in irf_files:
        command = "cvs add -kb %s" % item
        print command
        subprocess.call(command, shell=True)

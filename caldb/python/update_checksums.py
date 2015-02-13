#
# Run from the CALDB/data/glast/lat/bcf directory, this script runs
# fverify to check for warnings and errors, and then runs fchecksum to
# correct the CHECKSUM and DATASUM keywords.
#
# $Header$
#
import subprocess
from GtApp import GtApp

fverify = GtApp('fverify')
fchecksum = GtApp('fchecksum')

popen = subprocess.Popen('/usr/bin/find . -name \*.fits -print', shell=True,
                         stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         close_fds=True)
infiles, outfiles = popen.stdin, popen.stdout
#infiles, outfiles = os.popen4('/usr/bin/find . -name \*.fits -print' )

warnings = []
errors = []

for i, item in enumerate(outfiles):
    infile = item.strip()
    fverify.run(infile=infile, outfile='fverify_report', clobber='yes',
                chatter=0)
    report = open('fverify_report').readlines()[-1].strip()
    if report.find("0 warning") == -1:
        warnings.append(infile)
        fchecksum.run(infile=infile, update='yes')
    if report.find("0 error") == -1:
        errors.append(infile)

print "warnings: ", len(warnings)
for item in warnings:
    print item

print "errors: ", len(errors)
for item in errors:
    print item

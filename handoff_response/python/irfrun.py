"""
@file irfrun.py

@brief Run IRF generation (makeirf,makefits,livetime_corr) in the
target directory.
"""

import sys
import os
import glob
import re
import argparse
import yaml

if not 'INST_DIR' in os.environ:
    raise Exception('ST installation directory not defined.')
else:
    inst_dir = os.environ['INST_DIR']

sys.path.append(os.path.join(inst_dir,'irfs/handoff_response/python'))

from run_livetime_corr import corr_fit

def makeirf(irfname,irftype,ext='',overwrite=False):

    setup_file = 'setup%s.py'%ext
    skim_file = 'skim_%s%s.root'%(irfname,ext)
    param_file = 'par_%s%s.root'%(irfname,ext)

    if not os.path.isfile(setup_file):
        raise Exception('No setup.py in target directory.')
    
    if not os.path.isfile(skim_file) or overwrite:
        cmd = 'prune %s'%setup_file
        print cmd
        os.system(cmd)

    if not os.path.isfile(param_file) or overwrite:
        cmd = 'makeirf %s'%setup_file
        print cmd
        os.system(cmd)

    cmd = 'makefits %s %s'%(param_file,irfname)
    print cmd
    os.system(cmd)

usage = "%(prog)s [options] [target_dir]"
description = """This utility runs IRF generation in the target directory."""
parser = argparse.ArgumentParser(usage=usage, description=description)

parser.add_argument('target_dir',
                    help='Peform setup for livetime correction.')

parser.add_argument('--overwrite',default=False,action='store_true')

parser.add_argument('--no_livetime',default=False,action='store_true',
                    help='Skip generation of livetime correction.')

parser.add_argument('--config',
                    default = None,required=True,
                    help = 'Set the configuration file for IRF generation.  '
                    'This file defines scaling and fit parameters as well as '
                    'the energy range and number of thrown events in each '
                    'MC sample.')

args = parser.parse_args()

target_dir = args.target_dir

if not os.path.isfile(os.path.join(target_dir,'setup.py')):
    raise Exception('No setup.py in target directory.')

config = yaml.load(open(args.config,'r'))

m = re.search('(.+?)\_(.+)\_V\d\_(.+)',target_dir)

classver, classname, irftype = m.groups()

# If IRFdefault
if os.path.isfile('IRFdefault.py'):
    irf_default = 'IRFdefault.py'
else:
    irf_default = os.path.join(os.environ['INST_DIR'],
                               'irfs/handoff_response/python/IRFdefault.py')

os.system('cp %s %s'%(irf_default,target_dir))
os.chdir(target_dir)

# Generate IRFs     
makeirf(target_dir,irftype,overwrite=args.overwrite)

if args.no_livetime: sys.exit(0)

# Generate IRFs for each livetime bin
livetime_bins = config['Livetime']['ngen'].keys()
for ltb in livetime_bins:
    makeirf(target_dir,irftype,'_%s'%(ltb),
            overwrite=args.overwrite)

# Generate the livetime correction

lts = config['Livetime']['lts']
avltf=config['Livetime']['avltf']
irfnick=target_dir
path='./'
reffile=path+'aeff_'+irfnick+'.fits'

cf=corr_fit(reffile,avltf,irfnick,
            min_ebin=8,max_ebin=68)

lts=[.814,.833,.850,.871,.891,.913,.926]

for lt, ltb in zip(lts,livetime_bins):
    fn=("aeff_%s_%s.fits") % (irfnick,ltb)
    cf.add_aeff_file([fn,lt],[25.e6,1.25,5.75])

old_p0=[ -2., 5., -0.4, 2.4, -0.1, 3.5]

#do the fit
cf.fit(old_p0,fits_output="")
cf.writein(reffile)



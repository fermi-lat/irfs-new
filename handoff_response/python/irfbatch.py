import sys
import os
import argparse

usage = "%(prog)s [options] [target_dir] ..."
description = """Dispatches IRF generation jobs to batch system."""
parser = argparse.ArgumentParser(usage=usage, description=description)

parser.add_argument('target_dirs',nargs='*',
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

for d in args.target_dirs:

    if not os.path.isfile(os.path.join(d,'setup.py')):
        print 'Skipping ', d
        continue
    
    cmd = 'bsub -oo %s -q xlong python irfrun.py %s'%(d + '.log',d)
    cmd += ' --config=%s '%args.config
    if args.overwrite: cmd += ' --overwrite '
    if args.no_livetime: cmd += ' --no_livetime '

    print cmd
    os.system(cmd)



import os
import re
import sys
import argparse
from xml.dom import minidom
from GtApp import GtApp

if not 'INST_DIR' in os.environ:
    raise Exception('ST installation directory not defined.')
else:
    inst_dir = os.environ['INST_DIR']

sys.path.append(os.path.join(inst_dir,'irfs/caldb/python'))
from IrfConsolidator import *
from make_irf_index import make_irf_index

def get_class_types_from_xml(xmlfile):
    xmldoc = minidom.parse(xmlfile)
    class_version = str(xmldoc.getElementsByTagName('EventClass')[0].attributes['version'].value)
    itemlist = xmldoc.getElementsByTagName('EventMap')
    classnames=[str(l.attributes['name'].value) for l in itemlist[0].getElementsByTagName("EventCategory")
                if l.attributes['name'].value!="LLE"]
    classbits=[int(l.attributes['bit'].value) for l in itemlist[0].getElementsByTagName("EventCategory")
                if l.attributes['name'].value!="LLE"]    
    typenames=[str(l.attributes['name'].value) for l in itemlist[1].getElementsByTagName("EventCategory")]
    typebits=[int(l.attributes['bit'].value) for l in itemlist[1].getElementsByTagName("EventCategory")]
    return class_version,classnames,classbits,typenames,typebits

def mkdir(dir):
    if not os.path.exists(dir):  os.makedirs(dir)
    return dir

usage = "%(prog)s [options] "
description = """This script builds a complete CALDB starting from a
set of IRF FITS files produced with makeirf.  IRF files are first
merged into a set of aggregated IRF files for each class/type group.
The aggregated IRF files are then filled into a CALDB structure."""
parser = argparse.ArgumentParser(usage=usage, description=description)

parser.add_argument('--output', default = 'caldb', 
                    help = 'Output name of CALDB tar file.')

parser.add_argument('--irf_version', default = None, required=True, 
                    help = 'Set the IRF version number (V1, V2, etc.).')

parser.add_argument('--class_names', default = None, 
                    help = 'Provide a comma-separated list of class names.  The classes in the list will be '
                    'added to the list of classes loaded from the evtClassDefs file.')

parser.add_argument('--class_types', default = None, 
                    help = 'Provide a comma-separated list of type selection names '
                    '(e.g. FRONT,BACK,PSF0, etc.).  The types in the list will be '
                    'added to the list of types loaded from the evtClassDefs file.')

parser.add_argument('--classdefs', default = None, required=True,
                    help = 'Set the evtclassdefs file.  This will be used to populate the list '
                    'of valid class/type combinations.')

#parser.add_argument('files', nargs='*')

args = parser.parse_args()

skip_irfs = ['TRANSIENT_ALLTKR_R100']

irf_name_mapping = {'TRANSIENT_TKRONLY_R010' : 'TRANSIENT010E',
                    'TRANSIENT_TKRONLY_R020' : 'TRANSIENT020E',
                    'TRANSIENT_TKRONLY_R100' : 'TRANSIENT100E',
                    'TRANSIENT_R010'         : 'TRANSIENT010',
                    'TRANSIENT_R020'         : 'TRANSIENT020',
                    'TRANSIENT_R100'         : 'TRANSIENT100',
                    'TRANSIENT_SFR_R015'     : 'TRANSIENT015S',
                    'TRANSIENT_SFR_R100'     : 'TRANSIENT100S',
                    'ULTRACLEAN_TKRVETO'     : 'ULTRACLEANVETO'
                    }

(class_version,classnames,
 classbits,typenames,typebits) = get_class_types_from_xml(args.classdefs)

# Create the index mapping (this will be used later to fill
# irf_index.fits)

valid_types = 0
for t in typebits: valid_types += 2**t

index_mapping = { 'classes' : [], 'types' : [] }
for c,b in zip(classnames,classbits):

    if c in skip_irfs: continue
    
    if c in irf_name_mapping:
        irf_name = class_version + '_' + irf_name_mapping[c] + '_' + args.irf_version
    else:
        irf_name = class_version + '_' + c + '_' + args.irf_version
    
    index_mapping['classes'] += [(irf_name,b,valid_types)]

for t,b in zip(typenames,typebits):
    
    if t == 'FRONT' or t == 'BACK':    
        index_mapping['types'] += [(t,b,'none')]
    elif 'PSF' in t:
        index_mapping['types'] += [(t,b,'PSF')]
    elif 'EDISP' in t:
        index_mapping['types'] += [(t,b,'EDISP')]
    else:
        raise Exception('Unrecognized type.')
        
# Aggregate the IRF files
irf_files = []
irf_names = []



for c in classnames:

    if c in skip_irfs: continue
    
    irf_name = class_version + '_' + c + '_' + args.irf_version

    if c in irf_name_mapping:
        new_irf_name = class_version + '_' + irf_name_mapping[c] + '_' + args.irf_version
    else:
        new_irf_name = irf_name
        
    irf_names.append(new_irf_name)
    print 'Processing ', irf_name, new_irf_name

    mkdir(new_irf_name)    
    irf_consolidator = IrfConsolidator(new_irf_name, irf_name, new_irf_name)
    irf_files += irf_consolidator.process_files()
    
if len(irf_files) == 0:
    raise Exception("No IRF files found.")
    
# Setup CALDB directory structure
cwd = os.getcwd()
caldb_dir = os.path.join(cwd,'CALDB')
if os.path.isdir(caldb_dir):
    os.system('rm -r %s'%caldb_dir)
mkdir(caldb_dir)
    
bcf_dir = os.path.join(caldb_dir,'data/glast/lat/bcf')
mkdir(bcf_dir)
bcf_dir = os.path.abspath(bcf_dir)

ea_dir = os.path.join(bcf_dir,'ea')
psf_dir = os.path.join(bcf_dir,'psf')
edisp_dir = os.path.join(bcf_dir,'edisp')
mkdir(os.path.join(bcf_dir,'ea'))
mkdir(os.path.join(bcf_dir,'psf'))
mkdir(os.path.join(bcf_dir,'edisp'))

inst_caldb = os.path.join(inst_dir,'irfs/caldb/CALDB')

os.environ['CALDB']=os.path.join(cwd,'CALDB')
os.environ['CALDBCONFIG']=os.path.join(inst_caldb,'software/tools/caldb.config')
os.environ['CALDBALIAS']=os.path.join(inst_caldb,'software/tools/alias_config.fits')

# Create the IRF index file
make_irf_index(index_mapping,os.path.join(bcf_dir,'irf_index.fits'))

# Create the CALDB index file
os.chdir(os.path.join(bcf_dir,'..'))
os.system('crcif')

# Copy IRF files into CALDB
for f in irf_files:
    if re.search('aeff(.+).fits',f) is not None:
        cmd = 'cp %s %s'%(f,ea_dir); print cmd
        os.system(cmd)
    elif re.search('psf(.+).fits',f) is not None:
        cmd = 'cp %s %s'%(f,psf_dir); print cmd
        os.system(cmd)
    elif re.search('edisp(.+).fits',f) is not None:
        cmd = 'cp %s %s'%(f,edisp_dir); print cmd
        os.system(cmd)

# Create entries in calibration index file
udcif = GtApp('udcif')
for item in irf_names:

    print 'Filling CALDB index for ', item
    
    for prefix, loc in zip(['aeff', 'psf', 'edisp'],
                           (ea_dir, psf_dir, edisp_dir)):
        os.chdir(loc)
        for section in ("FB", "PSF", "EDISP"):
            infile = '%s_%s_%s.fits' % (prefix, item, section)
            if os.path.isfile(infile):            
                udcif.run(infile=infile,cif='../../caldb.indx', editc="no",
                          quality=0, newentries=1)
            else:
                print 'IRF file not found: %s'%infile

import tarfile
os.chdir(cwd)
tar = tarfile.open(args.output + '.tar.gz','w:gz')
tar.add('CALDB')
tar.close()

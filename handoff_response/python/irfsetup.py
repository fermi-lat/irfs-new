import yaml
import os
from math import *
from xml.dom import minidom


###### CONFIGURATION, EDIT HERE #############

#common parameters
#yaml file with class definitions
yamfile='pass8_event_selections_noalias_ssd.yaml'
#IRF definitions
xmlfile="EvtClassDefs_P8R2.xml"
#location of root files: END WITH A '/'
meritdir='/afs/slac/g/glast/groups/canda/irfs/p8_merit/P8V6/allGamma/'
#selection of files to use
globfile='*.root'
#where to put the path structure
target_dir='../test_irfs'
#update existing setup? if not fails if directory exists
update=True

#generated: #events,logemin,logemax
generated=((19873*1e4,1.25,5.75), #allE
           (9953*1e5,1.25,2.75), #lowE
           (49524*1e6,log10(3),1.25), #vlowE
           (136605*300.,5,7)) #highE

#items: yaml file class identifier, irfs class name, variant
#variant is either None or a yaml file identifier (e.g. "P8R1_EDISP0")
def get_class_types_from_xml(xmlfile):
    xmldoc = minidom.parse('EvtClassDefs_P8R2.xml')
    class_version =xmldoc.getElementsByTagName('EventClass')[0].attributes['version'].value
    itemlist = xmldoc.getElementsByTagName('EventMap')
    classnames=[l.attributes['name'].value for l in itemlist[0].getElementsByTagName("EventCategory") if l.attributes['name'].value!="LLE"]
    typenames=[l.attributes['name'].value for l in itemlist[1].getElementsByTagName("EventCategory")]
    return class_version,classnames,typenames

class_version,classnames,typenames = get_class_types_from_xml(xmlfile)

params=yaml.load(open("params_P8R2_V6.yaml",'r'))
generated=params['MC']
irf_version=params['irf_version']
################ END CONFIG ########################################
####################################################################

def makesetup(p8class,classname,variant):
    c = yaml.load(open(yamfile,'r'))

    if variant == "FRONT":
        varcut="Tkr1FirstLayer>5.5"
    elif variant == "BACK":
        varcut="Tkr1FirstLayer<5.5"    
    else:
        varcut=""

    setup_string = """
from IRFdefault import *
import glob
from math import *\n
"""

    setup_string+="Prune.fileName = 'skim_%s.root'\n" % (variant)

    setup_string+="Prune.cuts = '(%s)&&(%s)'\n" % (varcut,c[p8class])

    setup_string+="""
Prune.branchNames = '''McEnergy  McLogEnergy
McXDir  McYDir  McZDir
Tkr1FirstLayer WP8Best* WP8CT* Evt*
FswGamState TkrNumTracks CalTrackAngle'''.split()

meritDir = '%s'
meritFiles = '%s'
Data.files = sorted(glob.glob(meritDir + meritFiles))\n
""" % (meritdir,globfile)

    setup_string+="Data.generated = %s\n" % str([eval(generated[key][0]) for key in ['allE','lowE','highE','vlowE']])
    setup_string+="Data.logemin = [%s]\n" % str([generated[key][1] for key in ['allE','lowE','highE','vlowE']])
    setup_string+="Data.logemax = [%s]\n" % str([generated[key][2] for key in ['allE','lowE','highE','vlowE']])

    setup_string+="""
Data.friends = {}
Data.tree_name = 'MeritTuple'\n
"""

    setup_string+="className='%s'\n" % (classname) 
    setup_string+="root_dir='.'\n"
#    setup_string+="selectionName='%s'" % (frontstr)

    setup_string+="""
Data.var_xdir = 'WP8BestXDir'
Data.var_ydir = 'WP8BestYDir'
Data.var_zdir = 'WP8BestZDir'
Data.var_energy = 'WP8BestEnergy'

Bins.logemin= 0.75
Bins.logemax = 6.5

Bins.edisp_energy_overlap = 0
Bins.edisp_angle_overlap = 0
    
Bins.psf_energy_overlap = 0
Bins.psf_angle_overlap = 0

"""
    setup_string+="PSF.fit_pars={'ncore':[%g,1e-6,1.],'ntail':[%g,1e-6,1.0], 'score':[%g,0.1,5.0], 'stail':[%g,0.1,5], 'gcore':[%g,1.001,20.], 'gtail':[%g,1.001,20.]}\n\n" % tuple(params['PSF']['fit_pars'][variant])

    setup_string+="parameterFile = 'par_%s.root'\n\n" % (classname)

    setup_string+=('Edisp.scaling_pars=%s\n'%str(params['Edisp']['scaling_pars'][variant]))
    setup_string+=('PSF.scaling_pars=%s\n'%str(params['PSF']['scaling_pars'][variant]))

    return setup_string

for classname in ["CLEAN"]:
    for typename in typenames:
        irfname='%s_%s_%s_%s'%(class_version,classname,irf_version,typename)
        print 'Doing',irfname
        dirname=target_dir+'/'+irfname+'/'
        try:
            os.mkdir(dirname)
        except OSError as err:
            if update:
                pass #do not care if direcotry exists
            else:
                raise err
        f = open(dirname+'setup.py','w')
        f.write(makesetup('%s_%s'%(class_version,classname),irfname,typename))
        f.close()
    # f = open(dirname+'setup_b.py','w')
    # f.write(makesetup(p8id,classname,False,variant))
    # f.close()

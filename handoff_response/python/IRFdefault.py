# @file IRFdefault.py
# @brief define default setup options
#
# $Header$
import os
p = os.getcwd().split(os.sep)
print 'loading setup from %s ' % os.getcwd()

#extract class name from file path
className = p[len(p)-1]
print 'eventClass is %s' % className

    
class Prune(object):
    """
    information for the prune step
    """
    fileName = 'goodEvent.root' # file to create
    branchNames ="""
        EvtRun    EvtEnergyCorr 
        McEnergy  McXDir  McYDir  McZDir   
        McXDirErr   McYDirErr  McZDirErr   
        McTkr1DirErr  McDirErr  
        GltWord   FilterStatus_HI 
        Tkr1FirstLayer  
        CTBCORE  CTBSummedCTBGAM  CTBBest*
        """.split()  # specify branch names to include
    cuts='(GltWord&10)>0 && (GltWord!=35) && (FilterStatus_HI==0) && CTBBestEnergyProb>0.1 && CTBCORE>0.1'

class Data(object):
    files=['../all/'+Prune.fileName] # use pruned file in event class all by default
    # these correspond to the three runs at SLAC and UW
    generate_area = 6.0
    generated=[60e6,150e6, 97e6]
    logemin = [1.25, 1.25, 1.0]
    logemax = [5.75, 4.25, 2.75]
    

# define additional cuts based on event class: these are exclusive, add up to class 'all'
additionalCuts = {
    'all': '',
    'classA': '&&CTBSummedCTBGAM>=0.5 && CTBCORE>=0.8',
    'classB': '&&CTBSummedCTBGAM>=0.5 && CTBCORE>=0.5 && CTBCORE<0.8',
    'classC': '&&CTBSummedCTBGAM>=0.5 && CTBCORE<0.5',
    'classD': '&&CTBSummedCTBGAM>=0.1 && CTBSummedCTBGAM<0.5',
    'classF': '&&CTBSummedCTBGAM<0.1',
    'standard': '&&CTBSummedCTBGAM>0.5'
    }
if className in additionalCuts.keys():
    Prune.cuts += additionalCuts[className]
else: print 'Event class "%s" not recognized: using cuts for class all' %className
   
  
import numarray as num

#define default binning as attributes of object Bins
class Bins(object):
    logemin = 1.25
    logemax = 5.75
    logedelta = 0.25 #4 per decade
    energy_bins = int((logemax-logemin)/logedelta)
    energy_bin_edges = (num.arange(energy_bins+1)*logedelta+logemin).tolist()
    
    deltaCostheta = 0.1
    cthmin = 0.2;
    angle_bins = int((1-cthmin)/deltaCostheta)
    
    angle_bin_edges = num.arange(angle_bins+1)*deltaCostheta+cthmin
    
    
class EffectiveAreaBins(Bins):
    """
    subclass of Bins for finer binning of effective area
    """
    logemin = Bins.logemin
    logemax = Bins.logemax
    ebreak = 4.25
    ebinfactor = 4
    ebinhigh = 2
    logedelta = Bins.logedelta 
    # generate list with different 
    x = logemin
    factor = ebinfactor
    energy_bin_edges = []
    while x<logemax+0.01:
        if x>= ebreak: factor = ebinhigh
        energy_bin_edges.append(x)
        x += logedelta/factor
    anglebinfactor=4 # bins multiplier
    angle_bin_edges = num.arange(Bins.angle_bins*anglebinfactor+1)*Bins.deltaCostheta/anglebinfactor+Bins.cthmin



# where to write the output parameter tables - disable if null
parameterFile = '../parameters.root'

# the log file - to cout if null
logFile = 'log.txt'
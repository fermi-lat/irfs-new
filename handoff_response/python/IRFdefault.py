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
        GltWord   OBFGamStatus
        Tkr1FirstLayer VtxAngle 
        CTBVTX CTBCORE  CTBSummedCTBGAM  CTBBest*
        """.split()  # specify branch names to include
    cuts='(GltWord&10)>0 && (GltWord!=35) && (OBFGamStatus>0) && CTBBestEnergyProb>0.1 && CTBCORE>0.1'

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
else:
    pass
    #print 'Event class "%s" not recognized: using cuts for class all' %className
  
try:
    import numarray as num
except ImportError:
    import numpy as num

#define default binning as attributes of object Bins
class Bins(object):

    @classmethod
    def set_energy_bins(cls,logemin=None,logemax=None,logedelta=None):
        """Convenience method for initializing energy bin edge vector."""
        if logemin is not None: cls.logemin = logemin
        if logemax is not None: cls.logemax = logemax
        if logedelta is not None: cls.logedelta = logedelta
        
        cls.energy_bins = int((cls.logemax-cls.logemin)/cls.logedelta)
        cls.energy_bin_edges = (num.arange(cls.energy_bins+1)*
                                cls.logedelta+cls.logemin).tolist()

    @classmethod
    def set_angle_bins(cls,cthmin=None,cthdelta=None):
        """Convenience method for initializing cosTheta bin edge vector."""
        if cthmin is not None: cls.cthmin = cthmin
        if cthdelta is not None: cls.cthdelta = cthdelta

        cls.angle_bins = int((1.0-cls.cthmin)/cls.cthdelta)    
        cls.angle_bin_edges = num.arange(cls.angle_bins+1)*cls.cthdelta+cls.cthmin

    logemin = 1.25
    logemax = 5.75
    logedelta = 0.25 #4 per decade

    cthdelta = 0.1
    cthmin = 0.2

    # no overlap with adjacent bins for energy dispersion fits
    edisp_energy_overlap = 0  
    edisp_angle_overlap = 0

    # no overlap with adjacent bins for psf fits
    psf_energy_overlap = 0  
    psf_angle_overlap = 0

Bins.set_energy_bins()
Bins.set_angle_bins()
    
class FisheyeBins(Bins):
    """
    subclass of Bins for finer binning of fisheye correction
    """
    
    logemin = Bins.logemin
    logemax = Bins.logemax
    logedelta = 0.125

    cthmin = 0.2
    cthdelta = 0.05

FisheyeBins.set_energy_bins()
FisheyeBins.set_angle_bins()
    
class EffectiveAreaBins(Bins):
    """
    subclass of Bins for finer binning of effective area
    """

    @classmethod
    def set_energy_bins(cls,logemin=None,logemax=None):

        if logemin is not None: cls.logemin = logemin
        if logemax is not None: cls.logemax = logemax
        
        cls.energy_bin_edges = []
        x = cls.logemin
        factor = cls.ebinfactor
        while x<cls.logemax+0.01:
            if x>= cls.ebreak: factor = cls.ebinhigh
            cls.energy_bin_edges.append(x)
            x += cls.logedelta/factor

    
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
    angle_bin_edges = num.arange(Bins.angle_bins*anglebinfactor+1)*Bins.cthdelta/anglebinfactor+Bins.cthmin

class PSF(object):
    pass

class Edisp(object):
    pass

# the log file - to cout if null
logFile = 'log.txt'

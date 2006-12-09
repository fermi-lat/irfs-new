# @file default.py
# @brief define default setup options
#
# $Header$
import os
p = os.getcwd().split(os.sep)
print 'loading setup from %s ' % os.getcwd()
className = p[len(p)-1]
print 'eventClass is %s'%className

import numarray as num

def log_array(npts, xmin, xmax):
    xstep = num.log(xmax/xmin)/(npts - 1)
    return xmin*num.exp(num.arange(npts, type=num.Float)*xstep)


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
    logemin = Bins.logemin
    logemax = Bins.logemax
    ebreak = 4.25
    ebinfactor = 4
    anglebinfactor=4
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
    
    
class Data(object):
    files=['../all/goodEvent.root'] # use pruned file by default
    generated=[60e6,150e6,97e6]
    logemin = [1.25,1.25,1.0]
    logemax = [5.75,4.25,2.75]

      
# a small list just for determining the IRF functions
names="""
 EvtRun    EvtEnergyCorr 
 McEnergy  McXDir  McYDir  McZDir   
 McXDirErr   McYDirErr  McZDirErr   
 McTkr1DirErr  McDirErr  
 GltWord   FilterStatus_HI 
 Tkr1FirstLayer  
 CTBCORE  CTBSummedCTBGAM  CTBBest*
 """

class Prune(object):
    fileName = 'goodEvent.root'
    branchNames =names.split()  
    cuts='(GltWord&10)>0 && (GltWord!=35) && (FilterStatus_HI==0) && CTBBestEnergyProb>0.1 && CTBCORE>0.1'

# define cuts based on event class: these are exclusive, add up to class 'all'
if className=='classA': Prune.cuts+='&&CTBSummedCTBGAM>=0.5 && CTBCORE>=0.8'
if className=='classB': Prune.cuts+='&&CTBSummedCTBGAM>=0.5 && CTBCORE>=0.5 && CTBCORE<0.8'
if className=='classC': Prune.cuts+='&&CTBSummedCTBGAM>=0.5 && CTBCORE>=0.1 && CTBCORE<0.5'
if className=='classD': Prune.cuts+='&&CTBSummedCTBGAM>=0.1 && CTBSummedCTBGAM<0.5 && CTBCORE>0.1'
if className=='classF': Prune.cuts+='&&CTBSummedCTBGAM<0.1'

   

#print 'default energy bins edges: ', Bins.energy_bin_edges    
#print 'effective area energy bins edges: ', EffectiveAreaBins.energy_bin_edges    

import os, sys
import numarray as num
#import hippoplotter as plot

import irfLoader
irfLoader.Loader_go()

class Psf(object):
    _factory = irfLoader.IrfsFactory_instance()
    def __init__(self, irfsName, energy=100., inc=0, phi=0):
        self._irfs = self._factory.create(irfsName)
        self._psf = self._irfs.psf()
        self._energy = energy
        self._inc = inc
        self._phi = phi
    def __call__(self, separation):
        psf, energy, inc, phi = self._psf, self._energy, self._inc, self._phi
        try:
            y = []
            for x in separation:
                y.append(psf.value(x, energy, inc, phi))
            return num.array(y)
        except TypeError:
            return psf.value(separation, energy, inc, phi)
    def app_dir(self):
        x = self._psf.app_dir(self._energy, self._inc)
        return x.first, x.second
    def __getattr__(self, attrname):
        return getattr(self._psf, attrname)

#psf = Psf("Glast25::Front", 1000.)
#psf = Psf("testIrfs::Front", 1000.)
psf = Psf("DC1::Front", 1000.)

#nsamp = 1000
#separations = []
#wts = []
#for i in range(nsamp):
#    sep = psf.app_dir()[0]
#    separations.append(sep)
#    wts.append(1./num.sin(sep*num.pi/180.))
#
#nt = plot.newNTuple((separations, wts), ('separation', 'wt'))
#
#hist = plot.Histogram(nt, 'separation', xlog=1, ylog=1)
#rep = hist.getDataRep()
#rep.setWeight('wt')
#
#seps = 0.01*num.exp(num.arange(100, type=num.Float)*num.log(20./0.01)/99.)
#psf_vals = psf(seps)*100.
#plot.scatter(seps, psf_vals, pointRep='Line', lineStyle='Dash', oplot=1,
#             autoscale=1)

SkyDir = irfLoader.SkyDir

energy = 300.
theta = 40.
phi = 0.
scZAxis = SkyDir(180., theta)
scXAxis = SkyDir(90., 0)

cones = irfLoader.ConeVector()
roiCenter = SkyDir(180., 0.)
roi = irfLoader.AcceptanceCone(roiCenter, 15.)
cones.append(roi)

def sample_dist(sep, ntrials=100, nsamp=1000):
    srcDir = SkyDir(180., sep)
    nobs = []
    for j in range(ntrials):
        naccept = 0
        for i in range(nsamp):
            appDir = psf.appDir(energy, srcDir, scZAxis, scXAxis)
            if roi.inside(appDir):
                naccept += 1
        nobs.append(naccept)
    nobs = num.array(nobs)
    navg = sum(nobs)/float(ntrials)
    return (navg/float(nsamp),
            num.sqrt(sum(nobs**2)/float(ntrials) - navg**2)/float(nsamp))

def Npred(sep):
    srcDir = SkyDir(180., sep)
    return psf.angularIntegral(energy, srcDir, scZAxis, scXAxis, cones)

seps = []
nobs = []
nobserr = []
npreds = []
for sep in num.arange(5, 20):
    seps.append(sep)
    nn, nerr = sample_dist(sep, ntrials=20)
    nobs.append(nn)
    nobserr.append(nerr)
    npreds.append(Npred(sep))
    print sep, nobs[-1], nobserr[-1], npreds[-1]

from hippoplot import *
createApp(1)
plot1 = LinePlot(seps, npreds, xname='separation', yname='Npred')
plot2 = XYPlot(seps, nobs, yerr=nobserr, currentDisplay=plot1)
plot2.saveAsImage('foo.png')



"""
@brief
Test code for Psf angular integrals over the ROI acceptance cone.  The
plots are the enclosed Psf as a function of separation between the
true source direction and the center of the ROI.  The black curves are
the values returned from the IPsf::angularIntegral method, and the red
data points are the results of simulations using the IPsf::appDir
method, which is used by observationSim for drawing apparent photon
directions.

@author J. Chiang <jchiang@slac.stanford.edu>

"""
# @file psf_tests.py
# $Header$
#
import os, sys, bisect
import numarray as num
import hippoplotter as plot
import irf_loader

irf_loader.Loader_go()

SkyDir = irf_loader.SkyDir

class Psf(object):
    _factory = irf_loader.IrfsFactory_instance()
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

class PsfTests(object):
    def __init__(self, irfs, separations, roiRadius=20):
        self.irfs = irfs
        self.seps = separations
    def _Npred(self, sep):
        srcDir = SkyDir(180., sep)
        return self.psf.angularIntegral(self.energy, srcDir, self.scZAxis,
                                        self.scXAxis, self.cones)
    def _SampleDist(self, sep, ntrials=20, nsamp=100):
        srcDir = SkyDir(180., sep)
        nobs = []
        for j in range(ntrials):
            naccept = 0
            for i in range(nsamp):
                appDir = self.psf.appDir(self.energy, srcDir, self.scZAxis,
                                         self.scXAxis)
                if self.cones[0].inside(appDir):
                    naccept += 1
            nobs.append(naccept)
        nobs = num.array(nobs)
        navg = sum(nobs)/float(ntrials)
        return (navg/float(nsamp),
                num.sqrt(sum(nobs**2)/float(ntrials) - navg**2)/float(nsamp))
    def _setPsf(self, energy, inc, phi=0):
        self.energy = energy
        self.inc = inc
        self.phi = phi
        self.psf = Psf(self.irfs, energy, inc, phi)
        self.scZAxis = SkyDir(180, inc)
        self.scXAxis = SkyDir(90, 0)
    def _setRoi(self, roiRadius=20):
        roiCenter = SkyDir(180., 0)
        roi = irf_loader.AcceptanceCone(roiCenter, roiRadius)
        self.cones = [roi]
    def plotResults(self, energy, inclination, ntrials=20, nsamp=100,
                    plot_residuals=False):
        self._setPsf(energy, inclination)
        self._setRoi(roiRadius=20)
        nobs = []
        nobserr = []
        npreds = []
        colnames = ('separation', 'Npred', 'Nobs', 'Nobserr', 'Nobs - Npred')
        nt = plot.newNTuple( [[]]*len(colnames), colnames)
        display = plot.Scatter(nt, 'separation', 'Npred', pointRep='Line')
        display.setRange('y', 0, 1.1)
        plot.XYPlot(nt, 'separation', 'Nobs', yerr='Nobserr',
                    color='red', oplot=1)
        display.setRange('x', min(seps), max(seps))
        display.setTitle("%s: %i MeV, %.1f deg" %
                         (self.irfs, self.energy, self.inc))
        if plot_residuals:
            resid_display = plot.XYPlot(nt, 'separation', 'Nobs - Npred',
                                  yerr='Nobserr')
            resid_display.setRange('x', min(seps), max(seps))
            resid_display.setTitle("%s: %i MeV, %.1f deg" %
                                   (self.irfs, self.energy, self.inc))
            plot.hline(0)
            resid_display.setAutoRanging('y', True)
        for sep in self.seps:
            nobs, nerr = self._SampleDist(sep)
            npred = self._Npred(sep)
            nt.addRow( (sep, npred, nobs, nerr, nobs-npred) )
        return nt, display
    def plot_rspgenIntegral(self, energy, inclination, phi=0, nsamp=2000):
        rmin = 1e-2
        rmax = 30.
        npts = 20
        rstep = num.log(rmax/rmin)/(npts-1)
        radii = rmin*num.exp(rstep*num.arange(npts))
        self._setPsf(energy, inclination, phi)
        seps = []
        srcDir = SkyDir(180, 0)
        for i in range(nsamp):
            appDir = self.psf.appDir(energy, srcDir, self.scZAxis,
                                     self.scXAxis)
            seps.append(appDir.difference(srcDir)*180./num.pi)
        seps.sort()
        fraction = num.arange(nsamp, type=num.Float)/nsamp
        disp = plot.scatter(seps, fraction, xlog=1,
                            xname='ROI radius', yname='enclosed Psf fraction',
                            pointRep='Line', color='red')
        disp.setTitle("%s: %i MeV, %.1f deg" %
                      (self.irfs, energy, inclination))
        npred = []
        resids = []
        for radius in radii:
            npred.append(self.psf.angularIntegral(energy, inclination, phi,
                                                  radius))
            resids.append(num.abs((self._interpolate(seps, fraction, radius)
                                   - npred[-1])/npred[-1]))
        plot.scatter(radii, npred, pointRep='Line', oplot=1)
        residplot = plot.scatter(radii, resids, 'ROI radius',
                                 yname='abs(sim - npred)/npred',
                                 xlog=1, ylog=1)
        plot.hline(0)
        residplot.setTitle("%s: %i MeV, %.1f deg" %
                           (self.irfs, energy, inclination))
        
    def _interpolate(self, x, y, xval):
        if xval > x[-1]:
            return 1
        indx = bisect.bisect(x, xval) - 1
        yval = ( (xval - x[indx])/(x[indx+1] - x[indx])
                 *(y[indx+1] - y[indx]) + y[indx] )
        return yval
    
if __name__ == '__main__':
    seps = num.concatenate((num.arange(12, 19), num.arange(19, 21, 0.3),
                            num.arange(21, 25)))

#    energies = (100, 300, 1000, 3e3, 1e4)
#    incs = (0, 5, 10, 20)
    energies = (100,)
    incs = (0,)

    def createPlots(irfs, seps, energies, inclinations):
        my_tests = PsfTests(irfs, seps)
        for energy in energies:
            for inclination in inclinations:
                my_tests.plotResults(energy, inclination, plot_residuals=True)
                my_tests.plot_rspgenIntegral(energy, inclination)

    irfs = ('testIrfs::Front', 'testIrfs::Back',
            'Glast25::Front', 'Glast25::Back',
            'DC1::Front', 'DC1::Back')

    for irf in irfs:
        createPlots(irf, seps, energies, incs)

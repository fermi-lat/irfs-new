import os, sys
import numarray as num
import hippoplotter as plot

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
    def appDir(self):
        x = self._psf.app_dir(self._energy, self._inc)
        return x.first, x.second

psf = Psf("Glast25::Front", 1000.)

nsamp = 1000
separations = []
wts = []
for i in range(nsamp):
    sep = psf.appDir()[0]
    separations.append(sep)
    wts.append(1./num.sin(sep*num.pi/180.))

nt = plot.newNTuple((separations, wts), ('separation', 'wt'))

hist = plot.Histogram(nt, 'separation', xlog=1, ylog=1)
rep = hist.getDataRep()
rep.setWeight('wt')

seps = 0.01*num.exp(num.arange(100, type=num.Float)*num.log(20./0.01)/99.)
psf_vals = psf(seps)*100.
plot.scatter(seps, psf_vals, pointRep='Line', lineStyle='Dash', oplot=1,
             autoscale=1)

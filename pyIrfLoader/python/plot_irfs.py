"""
@brief Provide some convenient functor interfaces to the IRF value methods.
Plot them using HippoDraw.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

import pyIrfLoader

import numarray as num

pyIrfLoader.Loader_go()

class irf(object):
    _factory = pyIrfLoader.IrfsFactory_instance()
    def __init__(self, irfsName, inc=0, phi=0):
        self._irfs = self._factory.create(irfsName)
        self._inc = inc
        self._phi = phi

class Psf(irf):
    def __init__(self, irfsName, energy=100., inc=0, phi=0):
        irf.__init__(self, irfsName, inc, phi)
        self._psf = self._irfs.psf()
        self._energy = energy
    def __call__(self, separation):
        psf, energy, inc, phi = self._psf, self._energy, self._inc, self._phi
        try:
            y = []
            for x in separation:
                y.append(psf.value(x, energy, inc, phi))
            return num.array(y)
        except TypeError:
            return psf.value(separation, energy, inc, phi)

class Aeff(irf):        
    def __init__(self, irfsName, inc=0, phi=0):
        irf.__init__(self, irfsName, inc, phi)
        self._aeff = self._irfs.aeff()
    def __call__(self, energy):
        aeff, inc, phi = self._aeff, self._inc, self._phi
        try:
            y = []
            for x in energy:
                y.append(aeff.value(x, inc, phi))
            return num.array(y)
        except TypeError:
            return aeff.value(energy, inc, phi)

def log_array(xmin, xmax, npts):
    return xmin*num.exp(num.arange(npts, type=num.Float)/(npts-1)
                        *num.log(xmax/xmin))

sep = log_array(0.01, 20., 100)
energies = log_array(30, 2e5, 100)

if __name__ == '__main__':
    import hippoplotter as plot
    section = "Back"
    irfNames = ["DC1::", "DC1A::",
                "Glast25::", "testIrfs::"]
    for i in range(len(irfNames)):
        irfNames[i] += section
    lineStyles = ('Solid', 'DashDot', 'Dot', 'Dash')
    colors = ('black', 'red', 'green', 'blue')

    psf_plot = None
    aeff_plot = None
    for irfName, style, color in zip(irfNames, lineStyles, colors):
        psf = Psf(irfName, energy=5e2)

        if psf_plot is None:
            psf_plot = plot.scatter(sep, psf(sep), 'separation', 'Psf',
                                    xlog=1, ylog=1, xrange=(0.01, 30),
                                    color=color, pointRep='Line',
                                    lineStyle=style)
        else:
            plot.canvas.selectDisplay(psf_plot)
            plot.scatter(sep, psf(sep), pointRep='Line', oplot=1,
                         lineStyle=style, color=color)
    
        aeff = Aeff(irfName)
        if aeff_plot is None:
            aeff_plot = plot.scatter(energies, aeff(energies), 'energy',
                                     'Aeff', xlog=1, pointRep='Line',
                                     lineStyle=style, color=color)
        else:
            plot.canvas.selectDisplay(aeff_plot)
            plot.scatter(energies, aeff(energies), oplot=1, pointRep='Line',
                         lineStyle=style, color=color)
                     



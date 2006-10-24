import bisect
from FunctionWrapper import FunctionWrapper
from log_array import log_array
import numarray as num
import hippoplotter as plot
import pyIrfLoader as irf_loader

irf_loader.Loader_go()

factory = irf_loader.IrfsFactory_instance()

fronta = factory.create("DC2::FrontA")
backa = factory.create("DC2::BackA")
frontb = factory.create("DC2::FrontB")
backb = factory.create("DC2::BackB")

dc1a_front = factory.create("DC1A::Front")
dc1a_back = factory.create("DC1A::Back")

psf_fa = fronta.psf()
psf_ba = backa.psf()
psf_fb = frontb.psf()
psf_bb = backb.psf()

radii = log_array(30, 1e-2, 30.)

@FunctionWrapper
def theta_68(energy, psf=None, inc=0, phi=0, frac=0.68):
    f = FunctionWrapper(lambda x : psf.angularIntegral(energy, inc, phi, x))
    y = f(radii)
    indx = bisect.bisect(y, frac) - 1
    return ((frac - y[indx])/(y[indx+1] - y[indx])
            *(radii[indx+1] - radii[indx]) + radii[indx])

energies = log_array(40, 20, 2e5)
plot1 = plot.scatter(energies, theta_68(energies, psf=psf_fa), pointRep='Line',
                     xlog=1, ylog=1, xname='Energy (MeV)',
                     yname='theta_68 (deg)')
plot1.setTitle('normal incidence')
plot.scatter(energies, theta_68(energies, psf=psf_ba), pointRep='Line',
             oplot=1, lineStyle='Dot')
plot.scatter(energies, theta_68(energies, psf=psf_fb), pointRep='Line',
             xlog=1, ylog=1, color='red', oplot=1)
plot.scatter(energies, theta_68(energies, psf=psf_bb), pointRep='Line',
             oplot=1, color='red', lineStyle='Dot', autoscale=1)

plot.scatter(energies, theta_68(energies, psf=dc1a_front.psf()),
             pointRep='Line', oplot=1, color='blue')
plot.scatter(energies, theta_68(energies, psf=dc1a_back.psf()),
             pointRep='Line', oplot=1, color='blue', lineStyle='Dot')

aeff_fa = fronta.aeff()
aeff_ba = backa.aeff()
aeff_fb = frontb.aeff()
aeff_bb = backb.aeff()

@FunctionWrapper
def aeff(energy, aeffObj=None, inc=0, phi=0):
    return aeffObj.value(energy, inc, phi)

plot2 = plot.scatter(energies, aeff(energies, aeffObj=aeff_fa), xlog=1,
                     pointRep='Line', xname='Energy (MeV)',
                     yname='eff. area (cm^2)')
plot2.setTitle('normal incidence')
plot.scatter(energies, aeff(energies, aeffObj=aeff_ba), oplot=1,
             pointRep='Line', lineStyle='Dot')
plot.scatter(energies, aeff(energies, aeffObj=aeff_fb), oplot=1,
             pointRep='Line', color='red')
plot.scatter(energies, aeff(energies, aeffObj=aeff_bb), oplot=1,
             pointRep='Line', color='red', lineStyle='Dot')

plot.scatter(energies, aeff(energies, aeffObj=dc1a_front.aeff()), oplot=1,
             pointRep='Line', color='blue', autoscale=1)
plot.scatter(energies, aeff(energies, aeffObj=dc1a_back.aeff()), oplot=1,
             pointRep='Line', color='blue', autoscale=1, lineStyle='Dot')

@FunctionWrapper
def aeff_profile(inc, aeffObj=None, energy=1e3, phi=0):
    return aeffObj.value(energy, inc, phi)

thetas = num.arange(70)

plot3 = plot.scatter(thetas, aeff_profile(thetas, aeffObj=aeff_fa),
                     pointRep='Line', xname='inclination (deg)',
                     yname='eff. area (cm^2)')
plot3.setTitle('E = 1 GeV')
plot.scatter(thetas, aeff_profile(thetas, aeffObj=aeff_ba), oplot=1,
             pointRep='Line', lineStyle='Dot', autoscale=1)
plot.scatter(thetas, aeff_profile(thetas, aeffObj=aeff_fb), oplot=1,
             pointRep='Line', color='red', autoscale=1)
plot.scatter(thetas, aeff_profile(thetas, aeffObj=aeff_bb), oplot=1,
             pointRep='Line', color='red', lineStyle='Dot', autoscale=1)

plot.scatter(thetas, aeff_profile(thetas, aeffObj=dc1a_front.aeff()),
             oplot=1, pointRep='Line', color='blue', autoscale=1)
plot.scatter(thetas, aeff_profile(thetas, aeffObj=dc1a_back.aeff()),
             oplot=1, pointRep='Line', color='blue', autoscale=1,
             lineStyle='Dot')

@FunctionWrapper
def th68_profile(inc, psf=None, energy=1e3, phi=0, frac=0.68):
    f = FunctionWrapper(lambda x : psf.angularIntegral(energy, inc, phi, x))
    y = f(radii)
    indx = bisect.bisect(y, frac) - 1
    return ((frac - y[indx])/(y[indx+1] - y[indx])
            *(radii[indx+1] - radii[indx]) + radii[indx])

plot4 = plot.scatter(thetas, th68_profile(thetas, psf=psf_fa),
                     pointRep='Line', xname='inclination (deg)',
                     yname='theta_68 (deg)')
plot4.setTitle('E = 1 GeV')
plot.scatter(thetas, th68_profile(thetas, psf=psf_ba), oplot=1,
             pointRep='Line', lineStyle='Dot', autoscale=1)
plot.scatter(thetas, th68_profile(thetas, psf=psf_fb), oplot=1,
             pointRep='Line', color='red', autoscale=1)
plot.scatter(thetas, th68_profile(thetas, psf=psf_bb), oplot=1,
             pointRep='Line', color='red', lineStyle='Dot', autoscale=1)
plot.scatter(thetas, th68_profile(thetas, psf=dc1a_front.psf()), oplot=1,
             pointRep='Line', color='blue', autoscale=1)
plot.scatter(thetas, th68_profile(thetas, psf=dc1a_back.psf()), oplot=1,
             pointRep='Line', color='blue', lineStyle='Dot', autoscale=1)

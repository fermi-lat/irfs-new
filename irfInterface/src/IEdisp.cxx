/**
 * @file IEdisp.cxx
 * @brief Provide default implementations of factorable member functions.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>
#include <algorithm>

#include "CLHEP/Random/RandFlat.h"

#include "astro/SkyDir.h"

#include "st_facilities/GaussianQuadrature.h"

#include "irfInterface/IEdisp.h"

namespace {
   void fill_energies(double emin, double emax, size_t nee,
                      std::vector<double> & energies) {
      energies.clear();
      double estep(std::log(emax/emin)/(nee-1));
      for (size_t i = 0; i < nee; i++) {
         energies.push_back(emin*std::exp(i*estep));
      }
   }
   typedef double (*D_fp)(double*);    // "from" f2c.h
   double adhocIntegrator(D_fp fun, double emin, double emax, 
                          double err, long ierr) {
      double integral 
         = st_facilities::GaussianQuadrature::integrate(fun, emin, emax, 
                                                        err, ierr);
      if (integral == 0) {
         emin = std::max(emin, 1e-5);
         size_t npts(4);
         double efactor(std::exp(std::log(emax/emin)/(npts-1)));
         double e0(emin);
         double e1(emin*efactor);
         for (size_t i(0); i < npts; i++) {
            integral += 
               st_facilities::GaussianQuadrature::integrate(fun, e0, e1, 
                                                            err, ierr);
            e0 *= efactor;
            e1 *= efactor;
         }
      }
      return integral;
   }
}

namespace irfInterface {

double IEdisp::s_energy(1000);
double IEdisp::s_theta(0);
double IEdisp::s_phi(0);
double IEdisp::s_time(0);
const IEdisp * IEdisp::s_self(0);

double IEdisp::appEnergy(double energy,
                         const astro::SkyDir & srcDir,
                         const astro::SkyDir & scZAxis,
                         const astro::SkyDir & scXAxis,
                         double time) const {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   double emin(energy/10.);
   double emax(energy*10.);
   size_t nee(200);
   std::vector<double> energies;
   ::fill_energies(emin, emax, nee, energies);
   
   std::vector<double> integrand;
   for (std::vector<double>::iterator appE(energies.begin());
        appE != energies.end(); ++appE) {
      integrand.push_back(value(*appE, energy, theta, phi, time));
   }
   
   std::vector<double> integralDist;
   integralDist.push_back(0);
   for (size_t i = 1; i < energies.size(); i++) {
      integralDist.push_back(integralDist.at(i-1) + 
                             (integrand.at(i) + integrand.at(i-1))/2.
                             *(energies.at(i) - energies.at(i-1)));
   }
   
   double xi(CLHEP::RandFlat::shoot()*integralDist.back());
   size_t indx = std::upper_bound(integralDist.begin(), integralDist.end(), xi)
      - integralDist.begin() - 1;
   double appEnergy((xi - integralDist.at(indx))
                    /(integralDist.at(indx+1) - integralDist.at(indx))
                    *(energies.at(indx+1) - energies.at(indx))
                    + energies.at(indx));
   return appEnergy;
}


double IEdisp::integral(double emin, double emax, double energy,
                        const astro::SkyDir & srcDir,
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis, double time) const {
   return edispIntegral(this, emin, emax, energy, srcDir, scZAxis, 
                        scXAxis, time);
}

double IEdisp::integral(double emin, double emax, double energy,
                        double theta, double phi, double time) const {
   return edispIntegral(this, emin, emax, energy, theta, phi, time);
}

double IEdisp::edispIntegral(const IEdisp * self,
                             double emin, double emax, double energy,
                             const astro::SkyDir & srcDir,
                             const astro::SkyDir & scZAxis,
                             const astro::SkyDir & scXAxis, double time) {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return edispIntegral(self, emin, emax, energy, theta, phi, time);
}

double IEdisp::edispIntegral(const IEdisp * self, double emin, double emax,
                             double energy, double theta, double phi, 
                             double time) {
   setStaticVariables(energy, theta, phi, time, self);
   double integral;
   double err(0);
   long ierr(0);
   integral = ::adhocIntegrator(&edispIntegrand, emin, emax, err, ierr);
   return integral;
}

double IEdisp::meanAppEnergy(double energy,
                             const astro::SkyDir & srcDir, 
                             const astro::SkyDir & scZAxis,
                             const astro::SkyDir & scXAxis,
                             double time) const {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   static double phi(0);
   return meanAppEnergy(energy, theta, phi, time);
}

double IEdisp::meanAppEnergy(double energy, double theta, double phi,
                             double time) const {
   setStaticVariables(energy, theta, phi, time, this);
   double integral;
   double emin(0);
   double emax(energy*10.);
   double err(1e-5);
   long ierr(0);
   integral = 
      st_facilities::GaussianQuadrature::integrate(&meanEnergyIntegrand, emin, 
                                                   emax, err, ierr);

// The energy dispersion is not guarranteed to be properly normalized.
   double normalization;
   normalization = 
      st_facilities::GaussianQuadrature::integrate(&edispIntegrand, emin, 
                                                   emax, err, ierr);
   
   return integral/normalization;
}

double IEdisp::meanTrueEnergy(double appEnergy,
                              const astro::SkyDir & srcDir, 
                              const astro::SkyDir & scZAxis,
                              const astro::SkyDir & scXAxis,
                              double time) const {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   static double phi(0);
   return meanTrueEnergy(appEnergy, theta, phi, time);
}

double IEdisp::meanTrueEnergy(double appEnergy, double theta, double phi,
                              double time) const {
   setStaticVariables(appEnergy, theta, phi, time, this);
   double integral;
//   double emin(0);
   double emin(30);
   double emax(std::min(1.76e5, appEnergy*10.));
   double err(1e-5);
   long ierr(0);
   integral = 
      st_facilities::GaussianQuadrature::integrate(&meanTrueEnergyIntegrand, 
                                                   emin, emax, err, ierr);

// The energy dispersion is not guarranteed to be properly normalized.
   double normalization;
   normalization = 
      st_facilities::GaussianQuadrature::integrate(&trueEnergyIntegrand, 
                                                   emin, emax, err, ierr);
   
   return integral/normalization;
}

double IEdisp::edispIntegrand(double * appEnergy) {
   return s_self->value(*appEnergy, s_energy, s_theta, s_phi, s_time);
}

double IEdisp::meanEnergyIntegrand(double * appEnergy) {
   return *appEnergy*edispIntegrand(appEnergy);
}

double IEdisp::trueEnergyIntegrand(double * energy) {
   return s_self->value(s_energy, *energy, s_theta, s_phi, s_time);
}

double IEdisp::meanTrueEnergyIntegrand(double * energy) {
   return *energy*trueEnergyIntegrand(energy);
}

void IEdisp::setStaticVariables(double energy, double theta, double phi,
                                double time, const IEdisp * self) {
   s_energy = energy;
   s_theta = theta;
   s_phi = phi;
   s_time = time;
   s_self = self;
}

} // namespace irfInterface

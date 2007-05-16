/**
 * @file IEdisp.cxx
 * @brief Provide default implementations of factorable member functions.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <iostream>

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
}

namespace irfInterface {

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
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   EdispIntegrand func(*this, energy, theta, phi, time);
   return adhocIntegrator(func, emin, emax);
}

double IEdisp::integral(double emin, double emax, double energy,
                        double theta, double phi, double time) const {
   EdispIntegrand func(*this, energy, theta, phi, time);
   return adhocIntegrator(func, emin, emax);
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
   double integral;
   double emin(0);
   double emax(energy*10.);
   double err(1e-5);
   int ierr(0);

   MeanEnergyIntegrand func(*this, energy, theta, phi, time);

   integral = 
      st_facilities::GaussianQuadrature::dgaus8(func, emin, emax, err, ierr);
                                                
// The energy dispersion is not guarranteed to be properly normalized.
   EdispIntegrand edisp(*this, energy, theta, phi, time);
   double normalization;
   normalization = 
      st_facilities::GaussianQuadrature::dgaus8(edisp, emin, emax, err, ierr);

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
   double integral;
//   double emin(0);
   double emin(30);
   double emax(std::min(1.76e5, appEnergy*10.));
   double err(1e-5);
   int ierr(0);

   MeanTrueEnergyIntegrand func(*this, appEnergy, theta, phi, time);
   integral = 
      st_facilities::GaussianQuadrature::dgaus8(func, emin, emax, err, ierr);

// The energy dispersion is not guarranteed to be properly normalized.
   TrueEnergyIntegrand edisp(*this, appEnergy, theta, phi, time);
   double normalization;
   normalization = 
      st_facilities::GaussianQuadrature::dgaus8(edisp, emin, emax, err, ierr);
   
   return integral/normalization;
}

double IEdisp::adhocIntegrator(const EdispIntegrand & func, 
                               double emin, double emax) const {
   double err(1e-7);
   double integral = accuracyKluge(func, emin, emax, err);
   if (integral == 0) {
      emin = std::max(emin, 1e-5);
      size_t npts(4);
      double efactor(std::exp(std::log(emax/emin)/(npts-1)));
      double e0(emin);
      double e1(emin*efactor);
      for (size_t i(0); i < npts; i++) {
         integral += accuracyKluge(func, emin, emax, err);
         e0 *= efactor;
         e1 *= efactor;
      }
   }
   return integral;
}

double IEdisp::accuracyKluge(const EdispIntegrand & func,
                             double emin, double emax, double err) const {
// kluge to deal with integrations deemed not to be sufficiently accurate
   double integral;
   int ier;
   double factor(1e3);
   try {
      integral = st_facilities::GaussianQuadrature::dgaus8(func, emin, emax, 
                                                           err, ier);
   } catch (st_facilities::GaussianQuadrature::dgaus8Exception & eObj) {
      if (eObj.errCode() != 2 && err > 1./factor) {
         throw;
      }
      err *= factor;
      integral = st_facilities::GaussianQuadrature::dgaus8(func, emin, emax,
                                                           err, ier);
   }
   return integral;
}
   
} // namespace irfInterface


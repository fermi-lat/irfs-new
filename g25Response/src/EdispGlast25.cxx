/** 
 * @file EdispGlast25.cxx
 * @brief Implementation for LAT energy dispersion class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cassert>
#include <cmath>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "CLHEP/Random/RandGauss.h"

using CLHEP::RandGauss;

#include "astro/SkyDir.h"

#include "st_facilities/FitsUtil.h"

#include "EdispGlast25.h"

namespace {
   double erfcc(double x) {
/* (C) Copr. 1986-92 Numerical Recipes Software 0@.1Y.. */
      double t, z, ans;
      
      z=fabs(x);
      t=1.0/(1.0+0.5*z);
      ans = t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
            t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
            t*(-0.82215223+t*0.17087277)))))))));
      return x >= 0.0 ? ans : 2.0-ans;
   }
} // unnamed namespace

namespace g25Response {

EdispGlast25::EdispGlast25(const std::string &filename, int hdu) 
   : m_haveEdisp(true), m_fracSig(0) {

   if (hdu < 2 || hdu > 4) {
      std::ostringstream errorMessage;
      errorMessage << "g25Response::EdispGlast25:\n"
                   << "Invalid HDU: " << hdu << std::endl;
      throw std::invalid_argument(errorMessage.str());
   }

   std::string extName;
   st_facilities::FitsUtil::getFitsHduName(filename, hdu, extName);
   st_facilities::FitsUtil::getRecordVector(filename, extName, "energy", 
                                            m_energies);
   st_facilities::FitsUtil::getRecordVector(filename, extName, "sigma",
                                            m_sigmas);
}

double EdispGlast25::value(double appEnergy,
                           double energy, 
                           const astro::SkyDir &srcDir,
                           const astro::SkyDir &scZAxis,
                           const astro::SkyDir &scXAxis,
                           double time) const {
   (void)(srcDir); (void)(scZAxis); (void)(scXAxis);

   double theta(0);
   double phi(0);
   return value(appEnergy, energy, theta, phi, time);
}

double EdispGlast25::value(double appEnergy, double energy,
                           double theta, double phi, double time) const {
   (void)(phi);
   (void)(time);
   if (theta < 0) {
      std::ostringstream message;
      message << "g25Response::EdispGlast25::value(...):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }

   if (m_fracSig > 0 || m_haveEdisp) {
      if (appEnergy > 0) {
         double my_sigma = sigma(energy);
         return exp(-pow((appEnergy - energy)/my_sigma, 2)/2.)
            /sqrt(2.*M_PI)/my_sigma;
      } else if (appEnergy == 0) {
// This is a mixture model, with a delta-function at zero, so here
// return the integral of the Gaussian over the range (-\infty, 0).
         double scaledValue = energy/sqrt(2.)/sigma(energy);
         return 1. - ::erfcc(scaledValue)/2.;
      }
   } else {
      if (appEnergy == energy) {
// Should really return infinity, since it's a delta function.
         return 1;
      }
   }
   return 0;
}

double EdispGlast25::sigma(double energy) const {
   if (m_fracSig > 0) {
      return energy*m_fracSig;
   } else if (m_haveEdisp) {
      std::vector<double>::const_iterator it;
      if (energy < *(m_energies.begin())) {
         it = m_energies.begin();
      } else if (energy >= *(m_energies.end()-1)) {
         it = m_energies.end() - 2;
      } else {
         it = std::upper_bound(m_energies.begin(), m_energies.end(),
                               energy) - 1;
         assert(*it <= energy && *(it+1) >= energy);
      }
      int j = it - m_energies.begin();
      double fracSig = log(energy/(*it))/log(*(it+1)/(*it))
         *(m_sigmas[j+1] - m_sigmas[j]) + m_sigmas[j];
      return energy*fracSig;
   }
   return 0;
}

double EdispGlast25::appEnergy(double energy,
                               const astro::SkyDir &srcDir,
                               const astro::SkyDir &scZAxis,
                               const astro::SkyDir &scXAxis,
                               double time) const {
   (void)(srcDir); (void)(scZAxis); (void)(scXAxis);
   (void)(time);

   if (m_fracSig > 0 || m_haveEdisp) {
      double my_energy = RandGauss::shoot()*sigma(energy) + energy;
      if (my_energy > 0) {
         return my_energy;
      } else {
         return 0;
      }
   }
   return energy;
}

double EdispGlast25::integral(double emin, double emax, double energy,
                              const astro::SkyDir &srcDir, 
                              const astro::SkyDir &scZAxis,
                              const astro::SkyDir &scXAxis, 
                              double time) const {
   (void)(srcDir); (void)(scZAxis); (void)(scXAxis);

   double theta(0);
   double phi(0);
   return integral(emin, emax, energy, theta, phi, time);
}

double EdispGlast25::integral(double emin, double emax, double energy,
                              double theta, double phi, double time) const {
   (void)(theta); (void)(phi);
   (void)(time);

   if (m_fracSig > 0 || m_haveEdisp) {
      double my_sigma = sigma(energy);
      double scaledEmin = (emin - energy)/sqrt(2.)/my_sigma;
      double scaledEmax = (emax - energy)/sqrt(2.)/my_sigma;
      double my_integral = (::erfcc(scaledEmin) - ::erfcc(scaledEmax))/2.;
      return my_integral;
   } else {
      if (emin <= energy && emax >= energy) {
         return 1;
      }
   }
   return 0;
}

} // namespace g25Response

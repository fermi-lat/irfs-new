/**
 * @file Aeff.cxx
 * @brief Implementation for post-handoff review effective area class.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include "astro/SkyDir.h"

#include "latResponse/FitsTable.h"

#include "Aeff.h"

namespace latResponse {

Aeff::Aeff(const std::string & fitsfile, const std::string & extname)
   : m_aeffTable(fitsfile, extname, "EFFAREA") {}

double Aeff::value(double energy, 
                   const astro::SkyDir & srcDir, 
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis,
                   double time) const {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(energy, theta, phi, time);
}

double Aeff::value(double energy, double theta, double phi,
                   double time) const {
   (void)(phi);
   (void)(time);
   double costheta(std::cos(theta*M_PI/180.));
   double test_value = m_aeffTable.minCosTheta();
   if (costheta < m_aeffTable.minCosTheta()) {
      return 0;
   }
   bool interpolate;
   return m_aeffTable.value(std::log10(energy), costheta, interpolate=true)*1e4;
}

double Aeff::upperLimit() const {
   return m_aeffTable.maximum()*1e4;
}

} // namespace latResponse

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
   (void)(time);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi;
   return value(energy, theta, phi=0);
}

double Aeff::value(double energy, double theta, double phi,
                   double time) const {
   (void)(phi);
   (void)(time);
   bool interpolate;
   return m_aeffTable.value(std::log10(energy), std::cos(theta*M_PI/180.),
                            interpolate=true);
}

double Aeff::upperLimit() const {
   return m_aeffTable.maximum()*1e4;
}

} // namespace latResponse

/** 
 * @file Aeff.cxx
 * @brief Implementation for the effective Area class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "astro/SkyDir.h"

#include "irfUtil/Util.h"

#include "Aeff.h"

namespace testResponse {

Aeff::Aeff(const std::string &filename, int hdu) 
   : m_filename(filename), m_hdu(hdu) {
   read_FITS_table();
}

Aeff::~Aeff() {}

void Aeff::read_FITS_table() {
   std::string extName;
   irfUtil::Util::getFitsHduName(m_filename, m_hdu, extName);

   std::vector<double> lower_bounds;
   irfUtil::Util::getRecordVector(m_filename, extName, "energy_lo", 
                                  lower_bounds);

   std::vector<double> upper_bounds;
   irfUtil::Util::getRecordVector(m_filename, extName, "energy_hi", 
                                  upper_bounds);

   m_energy.clear();
   for (unsigned int i = 0; i < upper_bounds.size() 
           && i < lower_bounds.size(); i++) {
      m_energy.push_back( sqrt(lower_bounds[i]*upper_bounds[i]) );
   }

   irfUtil::Util::getRecordVector(m_filename, extName, "theta_lo",
                                  lower_bounds);
   lower_bounds[0] = 0;  // kludge because of error in aeff_DC1.fits
   irfUtil::Util::getRecordVector(m_filename, extName, "theta_hi", 
                                  upper_bounds);
   m_theta.clear();
   for (unsigned int i = 0; i < upper_bounds.size() 
           && i < lower_bounds.size(); i++) {
      m_theta.push_back( sqrt(lower_bounds[i]*upper_bounds[i]) );
   }

   irfUtil::Util::getRecordVector(m_filename, extName, "effarea", m_aeffTable);
}

double Aeff::value(double energy, 
                   const astro::SkyDir &srcDir, 
                   const astro::SkyDir &scZAxis,
                   const astro::SkyDir &) const {
// Inclination wrt spacecraft z-axis in radians.
   double theta = srcDir.difference(scZAxis);
   theta *= 180./M_PI;
   return value(energy, theta, 0.);
}

double Aeff::value(double energy, double theta, double phi) const {
   (void)(phi);
   if (theta < 0) {
      std::ostringstream message;
      message << "testResponse::Aeff::value(double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   double my_value(0);
   if (theta > m_theta.back()) {
      return 0;
   }
   try {
      my_value = irfUtil::Util::bilinear(m_energy, energy, m_theta, theta, 
                                         m_aeffTable);
   } catch (std::runtime_error & eObj) {
      if (irfUtil::Util::expectedException(eObj, "bilinear")) {
         std::cerr << "Failed attempt to interpolate at\n"
                   << "energy: " << energy << "\n"
                   << "theta: " << theta << std::endl;
      } else {
         throw;
      }
   }
   return my_value;
}

} // namespace testResponse

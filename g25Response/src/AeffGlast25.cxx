/** 
 * @file AeffGlast25.cxx
 * @brief Implementation for LAT effective area class using GLAST25 IRF data.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <sstream>
#include <stdexcept>

#include "astro/SkyDir.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "AeffGlast25.h"

namespace g25Response {

AeffGlast25::AeffGlast25(const std::string &filename, int hdu, double fudge)
   : Glast25(filename, hdu), m_fudge(fudge) {
   readAeffData();
}

double AeffGlast25::value(double energy, 
                          const astro::SkyDir &srcDir,
                          const astro::SkyDir &scZAxis,
                          const astro::SkyDir &,
                          double time) const {
   (void)(time);
// Inclination wrt spacecraft z-axis in degrees.
   double inc = srcDir.difference(scZAxis)*180./M_PI;
   if (m_fudge < 0) {
// Provide a constant effective area for testing source models.
      return 1.2e4;
   } else {
      return m_fudge*value(energy, inc);
   }
}

double AeffGlast25::value(double energy, double theta, double phi,
                          double time) const {
   (void)(phi);
   (void)(time);
   if (theta < 0) {
      std::ostringstream message;
      message << "g25Response::AeffGlast25::value(double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   return value(energy, theta);
}

double AeffGlast25::upperLimit() const {
   return m_aeffMax;
}

double AeffGlast25::value(double energy, double inc) const {
   if (inc <= Glast25::incMax()) {
      double my_value;
      try {
//          my_value = st_facilities::Util::bilinear(m_energy, energy, 
//                                                   m_theta, inc, m_aeff);
         double logEnergy = std::log(energy);
         my_value = st_facilities::Util::bilinear(m_logEnergy, logEnergy, 
                                                  m_theta, inc, m_aeff);
      } catch (std::runtime_error & eObj) {
         if (st_facilities::Util::expectedException(eObj, "Util::bilinear")) {
            my_value = 0;
         } else {
            throw;
         }
      }
      return my_value;
   } else {
      return 0;
   }
}

void AeffGlast25::readAeffData() {
   std::string extName;
   st_facilities::FitsUtil::getFitsHduName(m_filename, m_hdu, extName);
   st_facilities::FitsUtil::getRecordVector(m_filename, extName, "energy", 
                                            m_energy);
   m_logEnergy.clear();
   m_logEnergy.reserve(m_energy.size());
   for (unsigned int i = 0; i < m_energy.size(); i++) {
      m_logEnergy.push_back(std::log(m_energy[i]));
   }
   st_facilities::FitsUtil::getRecordVector(m_filename, extName, "theta", 
                                            m_theta);
   st_facilities::FitsUtil::getRecordVector(m_filename, extName, "aeff", 
                                            m_aeff);

   m_aeffMax = m_aeff.at(0);
   for (size_t i = 1; i < m_aeff.size(); i++) {
      if (m_aeffMax < m_aeff.at(i)) {
         m_aeffMax = m_aeff.at(i);
      }
   }
}

} // namespace g25Response

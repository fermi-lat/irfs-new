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
                          const astro::SkyDir &) const {
// Inclination wrt spacecraft z-axis in degrees.
   double inc = srcDir.difference(scZAxis)*180./M_PI;
   if (m_fudge < 0) {
// Provide a constant effective area for testing source models.
      return 1.2e4;
   } else {
      return m_fudge*value(energy, inc);
   }
}

double AeffGlast25::value(double energy, double theta, double phi) const {
   (void)(phi);
   if (theta < 0) {
      std::ostringstream message;
      message << "g25Response::AeffGlast25::value(double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   return value(energy, theta);
}


double AeffGlast25::value(double energy, double inc) const {
   if (inc <= Glast25::incMax()) {
      double my_value;
      try {
         my_value = st_facilities::Util::bilinear(m_energy, energy, 
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
   st_facilities::FitsUtil::getRecordVector(m_filename, extName, "theta", 
                                            m_theta);
   st_facilities::FitsUtil::getRecordVector(m_filename, extName, "aeff", 
                                            m_aeff);
}

} // namespace g25Response

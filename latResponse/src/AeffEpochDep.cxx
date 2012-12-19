/**
 * @file AeffEpochDep.cxx
 * @brief Class to handle Aeffs that change from epoch to epoch.  This
 * should eventually be refactored to be the "standard"
 * implementation, rather than treated as a special case.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 * 
 * $Header$
 */

#include "AeffEpochDep.h"

namespace latResponse {

AeffEpochDep::AeffEpochDep(const std::vector<std::string> & fitsfiles,
                           const std::string & extname,
                           size_t nrow) : m_upperLimit(0) {
   for (size_t i(0); i < fitsfiles.size(); i++) {
      getEpochStart(fitsfiles[i], extname);
      m_aeffs.push_back(Aeff(fitsfiles[i], extname, nrow));
      if (m_aeffs.back().upperLimit() > m_upperLimit) {
         m_upperLimit = m_aeffs.back().upperLimit();
      }
   }
}

double AeffEpochDep::value(double energy,
                           const astro::SkyDir & srcDir,
                           const astro::SkyDir & scZDir,
                           const astro::SkyDir & scXDir,
                           double time) const {
   size_t indx(index(time));
   return m_aeffs[indx].value(energy, srcDir, scZDir, scXDir, time);
}

double AeffEpochDep::value(double energy, double theta, double phi, 
                           double time) const {
   size_t indx(index(time));
   return m_aeffs[indx].value(energy, theta, phi, time);
}

double AeffEpochDep::upperLimit() const {
   return m_upperLimit;
}

} // namespace latResponse

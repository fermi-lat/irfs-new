/**
 * @file EdispEpochDep.cxx
 * @brief Definition for class to handle epoch-dependent energy dispersion.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 * 
 * $Header$
 */

#include "EdispEpochDep.h"

namespace latResponse {

EdispEpochDep::EdispEpochDep(const std::vector<std::string> & fitsfiles,
                             const std::string & extname,
                             size_t nrow) {
   for (size_t i(0); i < fitsfiles.size(); i++) {
      getEpochTimes(fitsfiles[i]);
      m_edisps.push_back(Edisp2(fitsfiles[i], extname, nrow));
   }
}

double EdispEpochDep::value(double appEnergy, 
                            double energy,
                            const astro::SkyDir & srcDir, 
                            const astro::SkyDir & scZAxis,
                            const astro::SkyDir & scXAxis,
                            double time) const {
   size_t indx(index(time));
   return m_edisps[indx].value(appEnergy, energy, srcDir, scZAxis,
                               scXAxis, time);
}

double EdispEpochDep::value(double appEnergy, double energy,
                            double theta, double phi,
                            double time) const {
   size_t indx(index(time));
   return m_edisps[indx].value(appEnergy, energy, theta, phi, time);
}

} // namespace latResponse

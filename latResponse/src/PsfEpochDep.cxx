/**
 * @file PsfEpochDep.cxx
 * @brief Definition for class that handles Psfs that change from
 * epoch to epoch.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include "PsfEpochDep.h"

namespace latResponse {

PsfEpochDep::PsfEpochDep(const std::vector<std::string> & fitsfiles, 
                         bool isFront, const std::string & extname,
                         size_t nrow) {
   for (size_t i(0); i < fitsfiles.size(); i++) {
      getEpochStart(fitsfiles[i], extname);
      m_psfs.push_back(Psf3(fitsfiles[i], isFront, extname, nrow));
   }
}

double PsfEpochDep::value(const astro::SkyDir & appDir, 
                          double energy, 
                          const astro::SkyDir & srcDir, 
                          const astro::SkyDir & scZAxis,
                          const astro::SkyDir & scXAxis, 
                          double time) const {
   size_t indx(index(time));
   return m_psfs[indx].value(appDir, energy, srcDir, scZAxis, scXAxis, time);
}

double PsfEpochDep::value(double separation, double energy, double theta,
                          double phi, double time) const {
   size_t indx(index(time));
   return m_psfs[indx].value(separation, energy, theta, phi, time);
}

double PsfEpochDep::
angularIntegral(double energy,
                const astro::SkyDir & srcDir,
                double theta, 
                double phi, 
                const AcceptanceConeVector_t & acceptanceCones, 
                double time) {
   size_t indx(index(time));
   return m_psfs[indx].angularIntegral(energy, srcDir, theta, phi, 
                                       acceptanceCones, time);
}

double PsfEpochDep::
angularIntegral(double energy, double theta, double phi,
                double radius, double time) const {
   size_t indx(index(time));
   return m_psfs[indx].angularIntegral(energy, theta, phi, radius, time);
}

} // namespace latResponse

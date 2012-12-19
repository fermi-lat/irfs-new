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

PsfEpochDep::PsfEpochDep() {
}

PsfEpochDep::PsfEpochDep(const PsfEpochDep & other)
   : irfInterface::IPsf(other), EpochDep(other) {
   for (size_t i(0); i < other.m_psfs.size(); i++) {
      m_psfs.push_back(other.m_psfs[i]->clone());
   }
}

PsfEpochDep & PsfEpochDep::operator=(const PsfEpochDep & rhs) {
   if (this != &rhs) {
      irfInterface::IPsf::operator=(rhs);
      EpochDep::operator=(rhs);
      for (size_t i(0); i < m_psfs.size(); i++) {
         delete m_psfs[i];
      }
      m_psfs.clear();
      for (size_t i(0); i < rhs.m_psfs.size(); i++) {
         m_psfs.push_back(rhs.m_psfs[i]->clone());
      }
   }
   return *this;
}

PsfEpochDep::~PsfEpochDep() {
   for (size_t i(0); i < m_psfs.size(); i++) {
      delete m_psfs[i];
   }
}

double PsfEpochDep::value(const astro::SkyDir & appDir, 
                          double energy, 
                          const astro::SkyDir & srcDir, 
                          const astro::SkyDir & scZAxis,
                          const astro::SkyDir & scXAxis, 
                          double time) const {
   size_t indx(index(time));
   return m_psfs[indx]->value(appDir, energy, srcDir, scZAxis, scXAxis, time);
}

double PsfEpochDep::value(double separation, double energy, double theta,
                          double phi, double time) const {
   size_t indx(index(time));
   return m_psfs[indx]->value(separation, energy, theta, phi, time);
}

double PsfEpochDep::
angularIntegral(double energy,
                const astro::SkyDir & srcDir,
                double theta, 
                double phi, 
                const AcceptanceConeVector_t & acceptanceCones, 
                double time) {
   size_t indx(index(time));
   return m_psfs[indx]->angularIntegral(energy, srcDir, theta, phi, 
                                        acceptanceCones, time);
}

double PsfEpochDep::
angularIntegral(double energy, double theta, double phi,
                double radius, double time) const {
   size_t indx(index(time));
   return m_psfs[indx]->angularIntegral(energy, theta, phi, radius, time);
}

void PsfEpochDep::addPsf(const irfInterface::IPsf & psf,
                         double epoch_start) {
   appendEpoch(epoch_start);
   m_psfs.push_back(psf.clone());
}

} // namespace latResponse

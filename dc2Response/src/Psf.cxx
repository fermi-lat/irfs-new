/** 
 * @file Psf.cxx
 * @brief Implementation for the DC2 point-spread function class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cassert>
#include <cmath>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "irfInterface/AcceptanceCone.h"
#include "irfUtil/Util.h"
#include "irfUtil/dgaus8.h"

#include "Psf.h"

namespace dc2Response {

Psf::Psf(const std::string &filename) 
   : DC2(filename), m_haveAngularIntegrals(false), m_acceptanceCone(0) {
   readEnergyScaling();
   computeCumulativeDists();
}

Psf::Psf(const std::string &filename, int hdu, int npars) 
   : DC2(filename, hdu, npars), m_haveAngularIntegrals(false), 
     m_acceptanceCone(0) {
   readEnergyScaling();
   computeCumulativeDists();
}

Psf::~Psf() {
   delete m_acceptanceCone;
}

Psf::Gint Psf::s_gfunc;

Psf::Psf(const Psf &rhs) : IPsf(rhs), DC2(rhs) {
   m_scaledAngles = rhs.m_scaledAngles;
   m_cumulativeDists = rhs.m_cumulativeDists;
   m_scaleEnergy = rhs.m_scaleEnergy;
   m_scaleFactor = rhs.m_scaleFactor;
   m_haveAngularIntegrals = rhs.m_haveAngularIntegrals;
   m_nesteps = rhs.m_nesteps;
   m_logestep = rhs.m_logestep;
   if (!rhs.m_acceptanceCone) {
      m_acceptanceCone = 0;
   } else {
      m_acceptanceCone 
         = new irfInterface::AcceptanceCone(*rhs.m_acceptanceCone);
   }
   m_psi = rhs.m_psi;
   m_angularIntegrals.resize(rhs.m_angularIntegrals.size());
   for (unsigned int i = 0; i < rhs.m_angularIntegrals.size(); i++) {
      m_angularIntegrals[i].resize(rhs.m_angularIntegrals[i].size());
      m_angularIntegrals[i] = rhs.m_angularIntegrals[i];
   }
}

double Psf::value(const astro::SkyDir &appDir, 
                  double energy, 
                  const astro::SkyDir &srcDir, 
                  const astro::SkyDir &scZAxis,
                  const astro::SkyDir &) const {
   
// Angle between photon and source directions in radians.
   double separation = appDir.difference(srcDir);

// Inclination wrt spacecraft z-axis in degrees.
   double inc = srcDir.difference(scZAxis)*180./M_PI;

   if (inc < m_theta[m_theta.size()-1]) {
      return value(separation, energy, inc);
   }
   return 0;
}

astro::SkyDir Psf::appDir(double energy,
                          const astro::SkyDir &srcDir,
                          const astro::SkyDir &scZAxis,
                          const astro::SkyDir &) const {
   
   double inclination = srcDir.difference(scZAxis)*180./M_PI;

   int ipar = getParamsIndex(energy, inclination);

   double xi = RandFlat::shoot();
   std::vector<double>::const_iterator distIt 
      = find_iterator(m_cumulativeDists[ipar], xi);
   int itheta = distIt - m_cumulativeDists[ipar].begin();
   double theta = ( (xi - *distIt)/(*(distIt + 1) - *distIt)
                    *(m_scaledAngles[itheta+1] - m_scaledAngles[itheta])
                    + m_scaledAngles[itheta] )*energyScaling(energy);

   xi = RandFlat::shoot();
   double phi = 2.*M_PI*xi;

// These might as well have been passed by value...
   astro::SkyDir appDir = srcDir;
   astro::SkyDir zAxis = scZAxis;

// Create an arbitrary x-direction about which to perform the theta
// rotation.
   Hep3Vector srcVec(appDir());
   Hep3Vector arbitraryVec(srcVec.x() + 1., srcVec.y() + 1., srcVec.z() + 1.);
   Hep3Vector xVec = srcVec.cross(arbitraryVec.unit());

   appDir().rotate(theta, xVec).rotate(phi, srcVec);

   return appDir;
}

double Psf::value(double separation, double energy, double theta,
                  double phi) const {
   if (theta < 0) {
      std::ostringstream message;
      message << "dc2Response::Psf::value(...):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   (void)(phi);
   return value(separation*M_PI/180., energy, theta);
}

double Psf::value(double separation, double energy, double inc) const {

   std::vector<double> pars = fitParams(energy, inc);

// Apply the energy scaling to the separation between source and event
// directions.
   double eScale = energyScaling(energy);
   double x = separation/eScale;

// Convert to per steradians.
   if (separation != 0) {
      return value(x, pars)/2./M_PI/sin(separation)/eScale;
   }
   return value(0, pars)/2./M_PI;
}

double Psf::value(double scaledAngle, std::vector<double> &pars) const {
   double x(scaledAngle);
// Use lognormal fit.
   assert(pars.size() == 3);
   double my_value;
   if (x != 0) {
      my_value = pars[0]/pars[1]*exp(-(log10(x) - pars[2])*(log10(x) - pars[2])
                                     /pars[1]/pars[1]/2.)/x;
   } else {
      my_value = 0;
   }
   return my_value;
}

void Psf::rescaleParams(std::vector<double> &pars, double scaleFactor) {
   pars[0] /= scaleFactor;
}

void Psf::readEnergyScaling() {
   if (m_have_FITS_data) {
      std::string extName;
      irfUtil::Util::getFitsHduName(m_filename, m_hdu, extName);
      irfUtil::Util::getRecordVector(m_filename, extName, "McEnergy",
                                     m_scaleEnergy);
      irfUtil::Util::getRecordVector(m_filename, extName, "ScaleFactor",
                                     m_scaleFactor);
   } else {
      irfUtil::Util::getTableVector(m_filename, "PsfScale", "McEnergy",
                                    m_scaleEnergy);
      irfUtil::Util::getTableVector(m_filename, "PsfScale", "ScaleFactor",
                                    m_scaleFactor);
   }
}

double Psf::energyScaling(double energy) const {
   std::vector<double>::const_iterator enIt 
      = find_iterator(m_scaleEnergy, energy);
   int indx = enIt - m_scaleEnergy.begin();
   double logScale = log10(energy/(*enIt))/log10(*(enIt+1)/(*enIt))
      *log10(m_scaleFactor[indx+1]/m_scaleFactor[indx]);
   double scale = m_scaleFactor[indx]*pow(10., logScale);
   return scale;
}

void Psf::computeCumulativeDists() {
// Prepare a logrithmically-spaced array of scaled angles.
   int npts(1000);            // Is this enough points?
   double angle_min = 1e-2;
   double angle_max = 1e2;
   double angle_step = log(angle_max/angle_min)/(npts-1);
   m_scaledAngles.reserve(npts);
   for (int i = 0; i < npts; i++) {
      m_scaledAngles.push_back(angle_min*exp(i*angle_step));
   }

// Apply trapezoidal rule on this grid of points for each set of
// parameters.
   for (unsigned int i = 0; i < m_pars.size(); i++) {
      std::vector<double> myDist;
      myDist.reserve(npts);
      myDist.push_back(0);
      for (int j = 1; j < npts; j++) {
         double delta = (value(m_scaledAngles[j], m_pars[i]) 
                         + value(m_scaledAngles[j-1], m_pars[i]))/2.
            *(m_scaledAngles[j] - m_scaledAngles[j-1]);
         myDist.push_back(delta + myDist[j-1]);
      }

// Rescale the appropriate parameter values (this is implementation
// dependent so pass to a separate method)
      rescaleParams(m_pars[i], myDist[npts-1]);

// Rescale the distributions.
      for (int j = 0; j < npts; j++) {
         myDist[j] /= myDist[npts-1];
      }
      m_cumulativeDists.push_back(myDist);
   }
}   

double Psf::angularIntegral(double energy,
                            const astro::SkyDir &srcDir,
                            const astro::SkyDir &scZAxis,
                            const astro::SkyDir &,
                            const std::vector<irfInterface::AcceptanceCone *> 
                            &acceptanceCones) {
   double theta = srcDir.difference(scZAxis)*180./M_PI;
   static double phi;
   return angularIntegral(energy, srcDir, theta, phi, acceptanceCones);
}

double Psf::angularIntegral(double energy, const astro::SkyDir & srcDir,
                            double theta, double phi, 
                            const std::vector<irfInterface::AcceptanceCone *> 
                            &acceptanceCones) {
   (void)(phi);
   if (!m_acceptanceCone || *m_acceptanceCone != *(acceptanceCones[0])) {
      computeAngularIntegrals(acceptanceCones);
      m_haveAngularIntegrals = true;
   }
   if (theta > *(m_theta.end() - 1) ) {
      return 0;
   }
   double psi = srcDir.difference(m_acceptanceCone->center());
   int ipar = getParamsIndex(energy, theta);
   int ie = ipar % (m_energy.size() - 1);
   int ie_sub = static_cast<int>(log10(energy/m_energy[ie])/m_logestep);
   int ipsi = find_iterator(m_psi, psi) - m_psi.begin();
   int indx = ipsi*m_nesteps + ie_sub;
   if (m_needIntegral[ipar][indx]) {
      performIntegral(ipar, ipsi, ie_sub);
      m_needIntegral[ipar][indx] = false;
   }
   return m_angularIntegrals[ipar][indx];
}

double Psf::angularIntegral(double energy, double theta, 
                            double phi, double radius) const {
   (void)(phi);
   int ipar = getParamsIndex(energy, theta);
   radius /= energyScaling(energy);

   if (radius >= *(m_scaledAngles.end()-1)) {
      return 1.;
   }
   std::vector<double>::const_iterator it 
      = find_iterator(m_scaledAngles, radius);
   int i = it - m_scaledAngles.begin();
   double integral = (radius - *it)/(*(it+1) - *it)
      *(m_cumulativeDists[ipar][i+1] - m_cumulativeDists[ipar][i])
      + m_cumulativeDists[ipar][i];

   return integral;
}

void Psf::computeAngularIntegrals
(const std::vector<irfInterface::AcceptanceCone *> &cones) {
   
// Assume the first acceptance cone is the ROI; ignore the rest.
   if (!m_acceptanceCone) {
      m_acceptanceCone = new irfInterface::AcceptanceCone();
   }
   *m_acceptanceCone = *cones[0];

   unsigned int npsi = 100;

   if (!m_haveAngularIntegrals) {
// Set up the array describing the separation between the center of
// the ROI and the source, m_psi
      m_psi.clear();
      double psiMin = 0.;
      double psiMax = 90.*M_PI/180.;    // Is this a large enough angle?
      double psiStep = (psiMax - psiMin)/(npsi - 1.);
      for (unsigned int i = 0; i < npsi; i++) {
         m_psi.push_back(psiStep*i + psiMin);
      }

// Determine the sub-dividing of each energy range, assuming the same
// logrithmic interval is used for each.
      m_nesteps = 5;   // Is this enough points per energy range?
      m_logestep = log10(m_energy[1]/m_energy[0])/(m_nesteps - 1);
   }

// Fill m_angularIntegrals.
   m_angularIntegrals.resize(m_pars.size());
   m_needIntegral.resize(m_pars.size());
   for (unsigned int ipar = 0; ipar < m_pars.size(); ipar++) {
      m_angularIntegrals[ipar].resize(m_psi.size()*m_nesteps);
      m_needIntegral[ipar].resize(m_psi.size()*m_nesteps);
      for (unsigned int i = 0; i < npsi; i++) {
         for (int j = 0; j < m_nesteps; j++) {
            int indx = i*m_nesteps + j;
            m_needIntegral[ipar][indx] = true;
//            performIntegral(ipar, i, j);
         } // j
      } // i
   } // ipar
}

void Psf::performIntegral(int ipar, int ipsi, int jen) {
   double roi_radius = m_acceptanceCone->radius()*M_PI/180.;
   double one = 1.;
   double mup = cos(roi_radius + m_psi[ipsi]);
   double mum = cos(roi_radius - m_psi[ipsi]);
   
   double cp = cos(m_psi[ipsi]);
   double sp = sin(m_psi[ipsi]);
   double cr = cos(roi_radius);

   int indx = ipsi*m_nesteps + jen;

// Compute the energy index and value.
   int ie = ipar % (m_energy.size()-1);
   double energy = m_energy[ie]*pow(10., m_logestep*jen);
   double err = 1e-5;
   long ierr;
   double firstIntegral = 0;

// Check if point is inside or outside the cone            
   if (m_psi[ipsi] < roi_radius) {
// Integrate the first term...
      s_gfunc = Gint(this, ipar, energy);
      dgaus8_(&Psf::gfuncIntegrand, &mum, &one, 
              &err, &firstIntegral, &ierr);
   }

// and the second.
   s_gfunc = Gint(this, ipar, energy, cp, sp, cr);
   double secondIntegral;
   dgaus8_(&Psf::gfuncIntegrand, &mup, &mum, 
           &err, &secondIntegral, &ierr);
   
   if (m_psi[ipsi] < roi_radius) {
      m_angularIntegrals[ipar][indx] = firstIntegral + secondIntegral;
   } else {
      m_angularIntegrals[ipar][indx] = secondIntegral;
   }
}

double Psf::Gint::value(double mu) const {
   double eScale = m_psfObj->energyScaling(m_energy);
   double x = acos(mu)/eScale;

// This bit parallels Psf::value(double, double, double).
   double my_value = m_psfObj->value(x, m_psfObj->m_pars[m_ipar]);
   if (x != 0) {
      my_value /= (2.*M_PI*sqrt(1. - mu*mu)*eScale);
   } else {
      my_value /= (2.*M_PI);
   }

   if (!m_doFirstTerm) {
      double phimin;
      double arg = (m_cr - mu*m_cp)/sqrt(1. - mu*mu)/m_sp;
      if (arg >= 1.) {
         phimin = 0;
      } else if (arg <= -1.) {
         phimin = M_PI;
      } else {
         phimin = acos(arg);
      }
      return 2.*phimin*my_value;
   }
   return 2.*M_PI*my_value;
}

} // namespace dc2Response

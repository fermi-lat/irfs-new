/** 
 * @file PsfGlast25.cxx
 * @brief Implementation for the GLAST25 point-spread function class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "irfInterface/AcceptanceCone.h"

#include "irfUtil/Util.h"
#include "irfUtil/dgaus8.h"

#include "PsfGlast25.h"

namespace {
   double my_acos(double mu) {
      if (mu > 1) {
         return 0;
      } else if (mu < -1) {
         return M_PI;
      } else {
         return acos(mu);
      }
   }
} // unnamed namespace

namespace g25Response {

PsfGlast25::Gint PsfGlast25::s_gfunc;

PsfGlast25::PsfGlast25(const PsfGlast25 &rhs) : IPsf(rhs), Glast25(rhs) {
   m_energy = rhs.m_energy;
   m_theta = rhs.m_theta;
   m_wt = rhs.m_wt;
   m_sig1.resize(rhs.m_sig1.size());
   m_sig1 = rhs.m_sig1;
   m_sig2.resize(rhs.m_sig2.size());
   m_sig2 = rhs.m_sig2;
   m_haveAngularIntegrals = rhs.m_haveAngularIntegrals;
   if (!rhs.m_acceptanceCone) {
      m_acceptanceCone = 0;
   } else {
      m_acceptanceCone = new irfInterface::AcceptanceCone(*rhs.m_acceptanceCone);
   }
   m_psi = rhs.m_psi;
   m_sigma = rhs.m_sigma;
   m_angularIntegrals.resize(rhs.m_angularIntegrals.size());
   m_angularIntegrals = rhs.m_angularIntegrals;
}

double PsfGlast25::value(const astro::SkyDir &appDir, 
                         double energy, 
                         const astro::SkyDir &srcDir, 
                         const astro::SkyDir &scZAxis,
                         const astro::SkyDir &) const {

// Angle between photon and source directions in radians.
   double separation = appDir.difference(srcDir);

// Inclination wrt spacecraft z-axis in degrees.
   double inc = srcDir.difference(scZAxis)*180./M_PI;

   return value(separation, energy, inc);
}
double PsfGlast25::value(double separation, double energy,
                         double theta, double phi) const {
   if (theta < 0) {
      std::ostringstream message;
      message << "g25Response::PsfGlast25::value(...):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   (void)(phi);
   return value(separation*M_PI/180., energy, theta);
}

astro::SkyDir PsfGlast25::appDir(double energy,
                                 const astro::SkyDir &srcDir,
                                 const astro::SkyDir &scZAxis,
                                 const astro::SkyDir &) const {
   
   double inclination = srcDir.difference(scZAxis)*180./M_PI;

   std::vector<double> psfParams;
   fetchPsfParams(energy, inclination, psfParams);
   double sig1 = psfParams[0];
   double sig2 = psfParams[1];
   double wt = psfParams[2];

// Draw the apparent direction in photon coordinates.
   double xi = RandFlat::shoot();
   double sig;
   if (xi <= wt) {
      sig = sig1*M_PI/180.;
   } else {
      sig = sig2*M_PI/180.;
   }
   xi = RandFlat::shoot();
   double mu = 1. + sig*sig*log(1. - xi*(1. - exp(-2./sig/sig)));
   double theta = my_acos(mu);

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

double PsfGlast25::value(double separation, double energy, 
                         double inc) const {

   if (inc > Glast25::incMax()) {
      return 0;
   }
   std::vector<double> psfParams;
   fetchPsfParams(energy, inc, psfParams);

   double sig1 = psfParams[0];
   double sig2 = psfParams[1];
   double wt = psfParams[2];

// Compute the psf as the weighted sum of two Gaussians projected onto
// the Celestial sphere (so cannot use the Euclidean space Gaussian
// function)
   sig1 *= M_PI/180.;
   sig2 *= M_PI/180.;

   double mu = cos(separation);
   double part1 = wt*exp(-(1. - mu)/sig1/sig1)
      /sig1/sig1/(1. - exp(-2./sig1/sig1));
   double part2 = (1. - wt)*exp(-(1. - mu)/sig2/sig2)
      /sig2/sig2/(1. - exp(-2./sig2/sig2));

   return (part1 + part2)/2./M_PI;
}

void PsfGlast25::readPsfData() {
   std::string extName;
   irfUtil::Util::getFitsHduName(m_filename, m_hdu, extName);
   irfUtil::Util::getRecordVector(m_filename, extName, "energy", m_energy);
   irfUtil::Util::getRecordVector(m_filename, extName, "theta", m_theta);
   irfUtil::Util::getRecordVector(m_filename, extName, "sig1", m_sig1);
   irfUtil::Util::getRecordVector(m_filename, extName, "sig2", m_sig2);
   irfUtil::Util::getRecordVector(m_filename, extName, "w", m_wt);
}

void PsfGlast25::fetchPsfParams(double energy, double inc, 
                                std::vector<double> &psfParams) const {

// Do a bilinear interpolation on the point-spread function data
   double sig1val, sig2val;
   try {
      sig1val 
         = irfUtil::Util::bilinear(m_energy, energy, m_theta, inc, m_sig1);
   } catch(...) {
      std::cerr << "Attempting to interpolate sig1." << std::endl;
      throw;
   }
   try {
      sig2val 
         = irfUtil::Util::bilinear(m_energy, energy, m_theta, inc, m_sig2);
   } catch(...) {
      std::cerr << "Attempting to interpolate sig2." << std::endl;
      throw;
   }

// Simply set the weight using the upper bound energy
   std::vector<double>::const_iterator ie;
   if (energy < *(m_energy.begin())) {
      ie = m_energy.begin();
   } else if (energy >= *(m_energy.end() - 1)) {
      ie = m_energy.end() - 1;
   } else {
      ie = std::upper_bound(m_energy.begin(), m_energy.end(), energy);
   }
   double wt = m_wt[ie - m_energy.begin()];

   psfParams.push_back(sig1val);
   psfParams.push_back(sig2val);
   psfParams.push_back(wt);
}

double 
PsfGlast25::angularIntegral(double energy,
                            const astro::SkyDir &srcDir,
                            const astro::SkyDir &scZAxis,
                            const astro::SkyDir &scXAxis,
                            const std::vector<irfInterface::AcceptanceCone *> 
                            &acceptanceCones) {
   (void)(scXAxis);
   static double phi;
   double theta = srcDir.difference(scZAxis)*180./M_PI;
   return angularIntegral(energy, srcDir, theta, phi, acceptanceCones);
}

double 
PsfGlast25::angularIntegral(double energy, const astro::SkyDir &srcDir,
                            double theta, double phi,
                            const std::vector<irfInterface::AcceptanceCone *> 
                            &acceptanceCones) {
   (void)(phi);
   if (!m_acceptanceCone || *m_acceptanceCone != *(acceptanceCones[0])) {
      computeAngularIntegrals(acceptanceCones);
      m_haveAngularIntegrals = true;
   }

   double psi = srcDir.difference(m_acceptanceCone->center());
   if (theta > Glast25::incMax()) {
      return 0;
   }
   std::vector<double> psfParams;
   fetchPsfParams(energy, theta, psfParams);

   double sig1 = psfParams[0]*M_PI/180.;
   double sig2 = psfParams[1]*M_PI/180.;
   double wt = psfParams[2];

   if (psi > *(m_psi.end()-1)) {
      return 0;
   } else {
      double frac1;
      double frac2;
      if (psi == 0) {
         double mu = cos(m_acceptanceCone->radius()*M_PI/180.);
         frac1 = (1. - exp((mu-1.)/sig1/sig1))/(1. - exp(-2./sig1/sig1));
         frac2 = (1. - exp((mu-1.)/sig2/sig2))/(1. - exp(-2./sig2/sig2));
      } else {
         frac1 = irfUtil::Util::bilinear(m_psi, psi, m_sigma, sig1,
                                         m_angularIntegrals);
         frac2 = irfUtil::Util::bilinear(m_psi, psi, m_sigma, sig2, 
                                         m_angularIntegrals);
      }
      return wt*frac1 + (1. - wt)*frac2;
   }
}

double PsfGlast25::angularIntegral(double energy, double theta, double phi,
                                   double radius) const {
   (void)(phi);
   std::vector<double> psfParams;
   fetchPsfParams(energy, theta, psfParams);

   double sig1 = psfParams[0]*M_PI/180.;
   double sig2 = psfParams[1]*M_PI/180.;
   double wt = psfParams[2];

   double mu = cos(radius*M_PI/180.);
   double frac1 = (1. - exp((mu-1.)/sig1/sig1))/(1. - exp(-2./sig1/sig1));
   double frac2 = (1. - exp((mu-1.)/sig2/sig2))/(1. - exp(-2./sig2/sig2));
   return wt*frac1 + (1. - wt)*frac2;
}

void PsfGlast25::computeAngularIntegrals
(const std::vector<irfInterface::AcceptanceCone *> &cones) {
   
// Assume the first acceptance cone is the ROI; ignore the rest.
   if (!m_acceptanceCone) {
      m_acceptanceCone = new irfInterface::AcceptanceCone();
   }
   *m_acceptanceCone = *cones[0];
   double roi_radius = m_acceptanceCone->radius()*M_PI/180.;

   unsigned int npsi = 100;
   unsigned int nsigma = 100;

   if (!m_haveAngularIntegrals) {
// Set up the arrays describing the separation between the center of
// the ROI and the source, m_psi, and the width of the Gaussian,
// m_sigma.
      m_psi.clear();
      double psiMin = 0.;
      double psiMax = 70.*M_PI/180.;
      double psiStep = (psiMax - psiMin)/(npsi - 1.);
      for (unsigned int i = 0; i < npsi; i++) {
         m_psi.push_back(psiStep*i + psiMin);
      }

      m_sigma.clear();
      double sigmaMin = 0.;
// The maximum sigma value in psf_lat.fits is actually about 27
// degrees.
      double sigmaMax = 30.*M_PI/180.; 
      double sigmaStep = (sigmaMax - sigmaMin)/(nsigma - 1.);
      for (unsigned int i = 0; i < nsigma; i++) {
         m_sigma.push_back(sigmaStep*i + sigmaMin);
      }
   }
// Fill m_angularIntegrals.
   m_angularIntegrals.resize(npsi*nsigma);
   for (unsigned int i = 0; i < npsi; i++) {
      double mup = cos(roi_radius + m_psi[i]);
      double mum = cos(roi_radius - m_psi[i]);

      double cr = cos(roi_radius);
      double cp = cos(m_psi[i]);
      double sp = sin(m_psi[i]);

      for (unsigned int j = 0; j < nsigma; j++) {
         int indx = i*nsigma + j;
         if (m_sigma[j] == 0) {
            m_angularIntegrals[indx] = 1.;
         } else {
            double denom = 1. - exp(-2/m_sigma[j]/m_sigma[j]);
            double gauss_int;
            if (m_psi[i] == 0) {
               gauss_int = 0;
            } else {
// Set the object providing the integrand.
               s_gfunc = Gint(m_sigma[j], cr, cp, sp);
// Use DGAUS8 to perform the integral.
               double err = 1e-5;
               long ierr;
               dgaus8_(&PsfGlast25::gfuncIntegrand, &mup, &mum, 
                       &err, &gauss_int, &ierr);
            }
            if (m_psi[i] <= roi_radius) {
               m_angularIntegrals[indx] 
                  = ((1. - exp((mum-1.)/m_sigma[j]/m_sigma[j]))
                     + gauss_int/M_PI/m_sigma[j]/m_sigma[j])/denom;
            } else {
               m_angularIntegrals[indx] 
                  =  gauss_int/M_PI/m_sigma[j]/m_sigma[j]/denom;
            }
         }
      } // j
   } // i
}

double PsfGlast25::Gint::value(double mu) const {
   double phi;
   if (mu == 1) {
      phi = M_PI;
   } else {
      double arg = (m_cr - mu*m_cp)/sqrt(1. - mu*mu)/m_sp;
      if (arg >= 1.) {
         phi = 0;
      } else if (arg <= -1.) {
         phi = M_PI;
      } else {
         phi = acos(arg);
      }
   }
   double value = phi*exp((mu - 1.)/m_sig/m_sig);
   return value;
}

} // namespace g25Response

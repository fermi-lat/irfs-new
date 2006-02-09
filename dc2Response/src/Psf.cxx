/** 
 * @file Psf.cxx
 * @brief Implementation for the DC2 point-spread function class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/dgaus8.h"
#include "st_facilities/Util.h"

#include "irfInterface/AcceptanceCone.h"

#include "Psf.h"
#include "PsfScaling.h"

namespace {
   double psfFunc(double x, double gamma) {
      return x*std::pow(1. + x*x/2./gamma, -gamma);
   }
}

namespace dc2Response {

double Psf::s_energy;
double Psf::s_theta;
double Psf::s_phi;
const Psf * Psf::s_self(0);
std::vector<double> Psf::s_gammas;
std::vector<double> Psf::s_psfNorms;
std::vector<double> Psf::s_lowerFractions;

Psf::Psf(const std::string & fitsfile, const std::string & extname)
   : DC2(fitsfile, extname), m_psfScaling(0) {
   readData();
   if (s_gammas.empty()) {
      computePsfNorms();
      computeLowerFractions();
   }
}

Psf::~Psf() {
   delete m_psfScaling;
}

Psf::Psf(const Psf & rhs) 
   : IPsf(rhs), DC2(rhs), m_sigma(rhs.m_sigma), m_gamma(rhs.m_gamma),
     m_logElo(rhs.m_logElo), m_logEhi(rhs.m_logEhi), m_logE(rhs.m_logE), 
     m_cosinc(rhs.m_cosinc) {
   delete m_psfScaling;
   m_psfScaling = new PsfScaling(*rhs.m_psfScaling);
}

double Psf::value(const astro::SkyDir & appDir, 
                  double energy, 
                  const astro::SkyDir & srcDir, 
                  const astro::SkyDir & scZAxis,
                  const astro::SkyDir & ) const {
// Angle between photon and source directions in radians.
   double separation = appDir.difference(srcDir);
   
// Inclination wrt spacecraft z-axis in radians
   double inc = srcDir.difference(scZAxis);

   return value(separation*180./M_PI, energy, inc*180./M_PI, 0.);
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
   double logE(std::log(energy));
   double mu(std::cos(theta*M_PI/180.));
   double gam(gamma(logE, mu));
   double psfNorm(st_facilities::Util::interpolate(s_gammas, s_psfNorms, gam));
   double meanSep(angularScale(energy, mu));
   double x(separation/meanSep);
   return ::psfFunc(x, gam)/2./M_PI/std::sin(separation*M_PI/180.)
      /(meanSep*M_PI/180.)/psfNorm;
}

double Psf::gamma(double logE, double mu) const {
   return st_facilities::Util::bilinear(m_cosinc, mu, m_logE, logE, m_gamma);
}

double Psf::sigma(double logE, double mu) const {
   return st_facilities::Util::bilinear(m_cosinc, mu, m_logE, logE, m_sigma);
}

double Psf::angularScale(double energy, double mu) const {
   double logE(std::log(energy));
   double scale((*m_psfScaling)(energy, mu));
   return scale*sigma(logE, mu);
}

astro::SkyDir Psf::appDir(double energy,
                          const astro::SkyDir & srcDir,
                          const astro::SkyDir & scZAxis,
                          const astro::SkyDir & ) const {
   double mu(std::cos(srcDir.difference(scZAxis)));
   double theta(drawOffset(energy, mu));
             
   double xi = RandFlat::shoot();
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

double Psf::drawOffset(double energy, double mu) const {
   double logE(std::log(energy));
   return angularScale(energy, mu)*drawScaledDev(gamma(logE, mu));
}

double Psf::angularIntegral(double energy,
                            const astro::SkyDir & srcDir,
                            const astro::SkyDir & scZAxis,
                            const astro::SkyDir &,
                            const std::vector<irfInterface::AcceptanceCone *> 
                            & acceptanceCones) {
   double theta = srcDir.difference(scZAxis)*180./M_PI;
   static double phi(0);
   return angularIntegral(energy, srcDir, theta, phi, acceptanceCones);
}

double Psf::angularIntegral(double energy, 
                            const astro::SkyDir & srcDir,
                            double theta, 
                            double phi, 
                            const std::vector<irfInterface::AcceptanceCone *> 
                            & acceptanceCones) {
   (void)(energy);
   (void)(srcDir);
   (void)(theta);
   (void)(phi);
   (void)(acceptanceCones);
//    if (!m_acceptanceCone || *m_acceptanceCone != *(acceptanceCones[0])) {
//       computeAngularIntegrals(acceptanceCones);
//       m_haveAngularIntegrals = true;
//    }
//    double psi = srcDir.difference(m_acceptanceCone->center());
//    if (psi >= m_psi.back()) {
//       return 0;
//    }
//    unsigned int ipsi = std::upper_bound(m_psi.begin(), m_psi.end(), psi) 
//       - m_psi.begin();
      
//    double sep_mean = sepMean(energy, theta*M_PI/180.);
//    if (sep_mean < m_sepMean.front() || sep_mean >= m_sepMean.back()) {
//       return 0;
//    }

//    unsigned int isepMean = 
//       std::upper_bound(m_sepMean.begin(), m_sepMean.end(), sep_mean) 
//       - m_sepMean.begin() - 1;

//    unsigned int indx = ipsi*m_sepMean.size() + isepMean;

//    if (isepMean == m_sepMean.size() - 1) {
//       return psfIntegral(psi, sep_mean);
//    }

// // Interpolate in sepMean dimension.
//    if (m_needIntegral[indx]) {
//       m_angularIntegral[indx] = psfIntegral(psi, m_sepMean[isepMean]);
//       m_needIntegral[indx] = false;
//    }
//    if (m_needIntegral[indx+1]) {
//       m_angularIntegral[indx+1] = psfIntegral(psi, m_sepMean[isepMean+1]);
//       m_needIntegral[indx+1] = false;
//    }
//    double my_value = (sep_mean - m_sepMean[isepMean])
//       /(m_sepMean[isepMean+1] - m_sepMean[isepMean])
//       *(m_angularIntegral[indx+1] - m_angularIntegral[indx])
//       + m_angularIntegral[indx];
//    return my_value;
   return 0;
}

double Psf::angularIntegral(double energy, double theta, 
                            double phi, double radius) const {
   s_energy = energy;
   s_theta = theta;
   s_phi = phi;
   s_self = this;
   double integral;
   double err(1e-5);
   long ierr(0);
   double zero(0);
   dgaus8_(&coneIntegrand, &zero, &radius, &err, &integral, &ierr);
   return integral;
}

double Psf::coneIntegrand(double * offset) {
   return s_self->value(*offset, s_energy, s_theta, s_phi)
      *std::sin(*offset*M_PI/180.)*2.*M_PI*M_PI/180.;
}

void Psf::readData() {
   m_psfScaling = new PsfScaling(m_filename);

// Read in the parameters from the FITS file.
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

   const tip::Table * psf = fileSvc.readTable(m_filename, m_extname);

   tip::Table::ConstIterator it(psf->begin());
   tip::ConstTableRecord & row(*it);

   row["sigma"].get(m_sigma);
   row["gamma"].get(m_gamma);

   std::vector<double> elo, ehi;
   row["energ_lo"].get(elo);
   row["energ_hi"].get(ehi);

   for (size_t k = 0; k < elo.size(); k++) {
      m_logElo.push_back(std::log(elo.at(k)));
      m_logEhi.push_back(std::log(ehi.at(k)));
      m_logE.push_back((m_logElo.back() + m_logEhi.back())/2.);
   }

   std::vector<double> theta;
   row["theta"].get(theta);

   for (size_t i = 0; i < theta.size(); i++) {
      m_cosinc.push_back(std::cos(theta.at(i)));
   }
   delete psf;
}

void Psf::computePsfNorms() {
   size_t ngam(100);
   double gmin(1.001);
   double gmax(5);
   double gstep(std::log(gmax/gmin)/(ngam - 1));
   double lowerLim(0);
   double upperLim(20);
   double err(1e-5);
   long ierr(0);
   double psfNorm(0);
   for (size_t i = 0; i < ngam; i++) {
      s_gammas.push_back(gmin*std::exp(i*gstep));
      dgaus8_(&psfIntegrand, &lowerLim, &upperLim, &err, &psfNorm, &ierr);
      s_psfNorms.push_back(psfNorm);
   }
}

double Psf::drawScaledDev(double gamma) const {
   double xbreak(std::sqrt(2.*gamma));
   double alpha(std::pow(2.*gamma, gamma));
   double beta(2.*gamma - 1.);

   double lowerFrac(st_facilities::Util::interpolate(s_gammas, 
                                                     s_lowerFractions,
                                                     gamma));
   double x(0);
   while (true) {
      double xi(RandFlat::shoot());
      if (xi < lowerFrac) {
         x = xbreak*std::sqrt(RandFlat::shoot());
         if (RandFlat::shoot() < ::psfFunc(x, gamma)/x) {
            return x;
         }
      } else {
         x = xbreak*std::pow((1. - RandFlat::shoot()), 1./(1. - beta));
         if (RandFlat::shoot() < 
             ::psfFunc(x, gamma)/(alpha*std::pow(x, -beta))) {
            return x;
         }
      }
   }
   return x;
}

void Psf::computeLowerFractions() {
   std::vector<double>::const_iterator it;
   s_lowerFractions.clear();
   for (it = s_gammas.begin(); it != s_gammas.end(); ++it) {
      double gamma = *it;
      double xbreak(std::sqrt(2.*gamma));
      double alpha(std::pow(2.*gamma, gamma));
      double beta(2.*gamma - 1.);
      double lowerIntegral(gamma);
      double upperIntegral(alpha/(beta - 1.)*std::pow(xbreak, 1. - beta));
      s_lowerFractions.push_back(lowerIntegral/(lowerIntegral+upperIntegral));
   }
}

double Psf::psfIntegrand(double * xx) {
   const double & x(*xx);
   return ::psfFunc(x, s_gammas.back());
}

} // namespace dc2Response

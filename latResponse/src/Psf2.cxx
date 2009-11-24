/**
 * @file Psf2.cxx
 * @brief Implementation for Psf2 class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/GaussianQuadrature.h"
#include "st_facilities/Util.h"

#include "astro/SkyDir.h"

#include "Psf2.h"
#include "PsfIntegralCache.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

namespace latResponse {

Psf2::Psf2(const std::string & fitsfile, bool isFront,
           const std::string & extname, size_t nrow)
   : Psf(fitsfile, isFront, extname, nrow) {
}

Psf2::Psf2(const Psf2 & rhs) : Psf(rhs) {}

Psf2::~Psf2() {
}

double Psf2::value(const astro::SkyDir & appDir, 
                   double energy, 
                   const astro::SkyDir & srcDir, 
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis, 
                   double time) const {
   (void)(scXAxis);
   double sep(appDir.difference(srcDir)*180./M_PI);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(sep, energy, theta, phi, time);
}

double Psf2::value(double separation, double energy, double theta,
                  double phi, double time) const {
   (void)(phi);
   (void)(time);
   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));
   return psf_function(separation*M_PI/180., my_pars);
}

double Psf2::angularIntegral(double energy, double theta, 
                             double phi, double radius, double time) const {
   if (energy < 120.) {
      double integral = IPsf::angularIntegral(energy, theta, phi, radius, time);
      return integral;
   }
   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));

   double ncore(my_pars[0]);
   double ntail(my_pars[1]);
   double score(my_pars[2]);
   double stail(my_pars[3]);
   double gcore(my_pars[4]);
   double gtail(my_pars[5]);

   double sep = radius*M_PI/180.;
   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*psf_base_integral(uc, gcore)*2.*M_PI*::sqr(score) + 
           ntail*ncore*psf_base_integral(ut, gtail)*2.*M_PI*::sqr(stail));
}

double Psf2::angularIntegral(double energy,
                             const astro::SkyDir & srcDir,
                             const astro::SkyDir & scZAxis,
                             const astro::SkyDir & scXAxis,
                             const std::vector<irfInterface::AcceptanceCone *> 
                             & acceptanceCones,
                             double time) {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return angularIntegral(energy, srcDir, theta, phi, acceptanceCones, time);
}

double Psf2::angularIntegral(double energy, const astro::SkyDir & srcDir,
                             double theta, double phi, 
                             const std::vector<irfInterface::AcceptanceCone *> 
                             & acceptanceCones, double time) {
   (void)(phi);
   (void)(time);
   irfInterface::AcceptanceCone & cone(*acceptanceCones.at(0));
   if (!m_integralCache || cone != m_integralCache->acceptanceCone()) {
      delete m_integralCache;
      m_integralCache = new PsfIntegralCache(*this, cone);
   }
   double psi(srcDir.difference(cone.center()));

   const std::vector<double> & psis(m_integralCache->psis());
   if (psi > psis.back()) {
      std::ostringstream message;
      message << "latResponse::Psf2::angularIntegral:\n"
              << "Error evaluating PSF integral.\n"
              << "Requested source location > " 
              << psis.back()*180/M_PI << " degrees from ROI center.";
      throw std::runtime_error(message.str());
   }

   size_t ii(std::upper_bound(psis.begin(), psis.end(), psi) 
             - psis.begin() - 1);

   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));
   double ncore(my_pars[0]);
   double ntail(my_pars[1]);
   double score(my_pars[2]);
   double stail(my_pars[3]);
   double gcore(my_pars[4]);
   double gtail(my_pars[5]);

   /// Remove sigma**2 scaling imposed by pars(...).  This is put back
   /// in angularIntegral below for each grid value of sigmas.  This
   /// preserves the normalization in the bilinear interpolation by
   /// explicitly putting in the important sigma-dependence.

   double norm_core(ncore*score*score);
   double norm_tail(ntail*stail*stail);

   double y1(norm_core*m_integralCache->angularIntegral(score, gcore, ii) + 
             norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii));
   double y2(norm_core*m_integralCache->angularIntegral(score, gcore, ii+1) + 
             norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii+1));

   double y = ((psi - psis.at(ii))/(psis.at(ii+1) - psis.at(ii))
               *(y2 - y1)) + y1;
   
   return y;
}

double Psf2::psf_base_function(double u, double gamma) {
   // ugly kluge because of sloppy programming in handoff_response
   // when setting boundaries of fit parameters for the PSF.
   if (gamma == 1) {
      gamma = 1.001;
   }
   return (1. - 1./gamma)*std::pow(1. + u/gamma, -gamma);
}

double Psf2::psf_base_integral(double u, double gamma) {
   return 1. - std::pow(1. + u/gamma, 1. - gamma);
}

double Psf2::psf_function(double sep, double * pars) {
   double ncore(pars[0]);
   double ntail(pars[1]);
   double score(pars[2]);
   double stail(pars[3]);
   double gcore(pars[4]);
   double gtail(pars[5]);

   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*psf_base_function(uc, gcore) +
           ntail*ncore*psf_base_function(ut, gtail));
}

double Psf2::psf_integral(double sep, double * pars) {
   double ncore(pars[0]);
   double ntail(pars[1]);
   double score(pars[2]);
   double stail(pars[3]);
   double gcore(pars[4]);
   double gtail(pars[5]);

   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*psf_base_integral(uc, gcore) + 
           ntail*ncore*psf_base_integral(ut, gtail));
}

double * Psf2::pars(double energy, double costh) const {
   double loge(std::log10(energy));
   if (costh == 1.0) {  // Why is this necessary?
      costh = 0.9999;
   }
   
   if (loge == m_loge_last && costh == m_costh_last) {
      return m_pars;
   }
   
   m_loge_last = loge;
   m_costh_last = costh;
   
   m_parTables.getPars(loge, costh, m_pars);
   
   // Rescale the sigma values after interpolation
   m_pars[2] *= scaleFactor(energy);
   m_pars[3] *= scaleFactor(energy);
   
   if (m_pars[2] == 0 || m_pars[3] == 0 ||
       m_pars[4] == 0 || m_pars[5] == 0) {
      std::ostringstream message;
      message << "latResponse::Psf2::pars: psf parameters are zero "
              << "when computing solid angle normalization:\n"
              << "\tenergy = " << energy << "\n"
              << "\tm_pars[2] = " << m_pars[2] << "\n"
              << "\tm_pars[3] = " << m_pars[3] << "\n"
              << "\tm_pars[4] = " << m_pars[4] << "\n"
              << "\tm_pars[5] = " << m_pars[5] << std::endl;
      std::cerr << message.str() << std::endl;
      throw std::runtime_error(message.str());
   }
   
// Ensure that the Psf2 integrates to unity.
   double norm;
   static double theta_max(M_PI/2.);
   if (energy < 120.) { // Use the *correct* integral of Psf2 over solid angle.
      Psf2Integrand foo(m_pars);
      double err(1e-5);
      int ierr;
      norm = st_facilities::GaussianQuadrature::dgaus8(foo, 0, theta_max,
                                                       err, ierr);
      m_pars[0] /= norm*2.*M_PI;
   } else { // Use small angle approximation.
      double norm0(psf_base_integral(::sqr(theta_max/m_pars[2])/2., m_pars[4])
                   *2.*M_PI*::sqr(m_pars[2]));
      double norm1(psf_base_integral(::sqr(theta_max/m_pars[3])/2., m_pars[5])
                   *2.*M_PI*::sqr(m_pars[3]));

      norm = m_pars[0]*(norm0 + m_pars[1]*norm1);
      m_pars[0] /= norm;
   }

   return m_pars;
}

} // namespace latResponse


/**
 * @file Psf3.cxx
 * @brief PSF as the sum of two King model functions.
 * See http://confluence.slac.stanford.edu/x/bADIAw.
 * In contrast to Psf2, this class interpolates the distributions 
 * rather than the parameters.
 *
 * $Header$
 */

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "latResponse/Bilinear.h"
#include "Psf3.h"
#include "PsfIntegralCache.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

namespace latResponse {

Psf3::Psf3(const std::string & fitsfile, bool isFront,
           const std::string & extname, size_t nrow) 
   : Psf2(fitsfile, isFront, extname, nrow) {
   normalize_pars();
}

Psf3::Psf3(const Psf3 & rhs) : Psf2(rhs) {}

Psf3::~Psf3() {
}

double Psf3::value(double separation, double energy, double theta,
                   double phi, double time) const {
   (void)(phi);
   (void)(time);

   std::vector<std::vector<double> > parVectors;

   double logE(std::log10(energy));
   double costh(std::cos(theta*M_PI/180.));
   double tt, uu;
   std::vector<double> cornerEnergies;
   m_parTables.getCornerPars(logE, costh, tt, uu, cornerEnergies, parVectors);

   double sep(separation*M_PI/180.);
   std::vector<double> yvals;
   for (size_t i(0); i < parVectors.size(); i++) {
      yvals.push_back(evaluate(cornerEnergies.at(i), sep, parVectors.at(i)));
   }

   double my_value = Bilinear::evaluate(tt, uu, yvals);
   return my_value;
}

double Psf3::angularIntegral(double energy, double theta, 
                             double phi, double radius, double time) const {

   double logE(std::log10(energy));
   double costh(std::cos(theta*M_PI/180.));
   if (costh == 1.0) {  // Why is this necessary?
      costh = 0.9999;
   }
   
   if (logE == m_loge_last && costh == m_costh_last) {
      return m_integral;
   }
   
   m_loge_last = logE;
   m_costh_last = costh;

   if (energy < 120.) {
      m_integral = IPsf::angularIntegral(energy, theta, phi, radius, time);
      return m_integral;
   }
   double tt, uu;
   std::vector<double> cornerEnergies;
   std::vector<std::vector<double> > parVectors;
   m_parTables.getCornerPars(logE, costh, tt, uu, cornerEnergies, parVectors);
   
   std::vector<double> yvals;
   for (size_t i(0); i < 4; i++) {
      yvals.push_back(psf_base_integral(cornerEnergies.at(i), radius,
                                        parVectors.at(i)));
   }
   m_integral = Bilinear::evaluate(tt, uu, yvals);
   return m_integral;
}

double Psf3::psf_base_integral(double energy, double radius, 
                               const std::vector<double> & pars) {
   double ncore(pars.at(0));
   double ntail(pars.at(1));
   double score(pars.at(2)*scaleFactor(energy));
   double stail(pars.at(3)*scaleFactor(energy));
   double gcore(pars.at(4));
   double gtail(pars.at(5));

   double sep = radius*M_PI/180.;
   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*Psf2::psf_base_integral(uc, gcore)*2.*M_PI*::sqr(score) + 
           ntail*ncore*Psf2::psf_base_integral(ut, gtail)*2.*M_PI*::sqr(stail));
}

double Psf3::angularIntegral(double energy,
                             const astro::SkyDir & srcDir,
                             double theta, 
                             double phi, 
                             const AcceptanceConeVector_t & acceptanceCones, 
                             double time) {
   (void)(phi);
   (void)(time);
   irfInterface::AcceptanceCone & cone(*acceptanceCones.at(0));
   if (!m_integralCache || cone != m_integralCache->acceptanceCone()) {
      delete m_integralCache;
      m_integralCache = new PsfIntegralCache(*this, cone);
   }
   double logE(std::log10(energy));
   double costh(std::cos(theta*M_PI/180.));
   double tt, uu;
   std::vector<double> cornerEnergies;
   std::vector<std::vector<double> > parVectors;
   m_parTables.getCornerPars(logE, costh, tt, uu, cornerEnergies, parVectors);

   double psi(srcDir.difference(cone.center()));

   std::vector<double> yvals;
   for (size_t i(0); i < parVectors.size(); i++) {
      yvals.push_back(angularIntegral(cornerEnergies.at(i), psi,
                                      parVectors.at(i)));
   }
   double value(Bilinear::evaluate(tt, uu, yvals));

   return value;
}

double Psf3::angularIntegral(double energy, double psi, 
                             const std::vector<double> & pars) {
   const std::vector<double> & psis(m_integralCache->psis());
   if (psi > psis.back()) {
      std::ostringstream message;
      message << "latResponse::Psf3::angularIntegral:\n"
              << "Error evaluating PSF integral.\n"
              << "Requested source location > " 
              << psis.back()*180/M_PI << " degrees from ROI center.";
      throw std::runtime_error(message.str());
   }
   size_t ii(std::upper_bound(psis.begin(), psis.end(), psi) 
             - psis.begin() - 1);

   double ncore(pars.at(0));
   double ntail(pars.at(1));
   double score(pars.at(2)*scaleFactor(energy));
   double stail(pars.at(3)*scaleFactor(energy));
   double gcore(pars.at(4));
   double gtail(pars.at(5));

   /// Remove sigma**2 scaling imposed by pars(...).  This is put back
   /// in angularIntegral below for each grid value of sigmas.  This
   /// preserves the normalization in the bilinear interpolation by
   /// explicitly putting in the important sigma-dependence.

   double norm_core(ncore*score*score);
   double norm_tail(ntail*stail*stail);

   double y1 = 
      norm_core*m_integralCache->angularIntegral(score, gcore, ii) + 
      norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii);
   double y2 =
      norm_core*m_integralCache->angularIntegral(score, gcore, ii+1) + 
      norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii+1);

   double y = ((psi - psis.at(ii))/(psis.at(ii+1) - psis.at(ii))
               *(y2 - y1)) + y1;
   return y;
}

double Psf3::evaluate(double energy, double sep,
                      const std::vector<double> & pars) const {
   double ncore(pars.at(0));
   double ntail(pars.at(1));
   double score(pars.at(2)*scaleFactor(energy));
   double stail(pars.at(3)*scaleFactor(energy));
   double gcore(pars.at(4));
   double gtail(pars.at(5));

   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*Psf2::psf_base_function(uc, gcore) +
           ntail*ncore*Psf2::psf_base_function(ut, gtail));
}

void Psf3::normalize_pars(double radius) {
   const std::vector<double> & logEnergies(m_parTables.logEnergies());
   const std::vector<double> & costhetas(m_parTables.costhetas());
   
   double phi(0);
   double time(0);
   for (size_t ilogE(0); ilogE < logEnergies.size(); ilogE++) {
      double energy(std::pow(10., logEnergies.at(ilogE)));
      for (size_t icosth(0); icosth < costhetas.size(); icosth++) {
         double theta = std::acos(costhetas.at(icosth))*180./M_PI;
         std::vector<double> pars;
         m_parTables.getPars(ilogE, icosth, pars);
         double norm;
         if (energy < 120) {
            norm = IPsf::angularIntegral(energy, theta, phi, radius, time);
         } else {
            norm = psf_base_integral(energy, radius, pars);
         }
         pars.at(0) /= norm;
         m_parTables.setPars(ilogE, icosth, pars);
      }
   }

}

} // namespace latResponse

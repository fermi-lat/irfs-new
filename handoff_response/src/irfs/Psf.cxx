/** 
* @file Psf.cxx
* @brief Implementation for the  point-spread function class.
* @author 
* 
* $Header$
*/

#include <cassert>
#include <cmath>

#include <stdexcept>
#include <sstream>
#include <algorithm>

#include "st_facilities/dgaus8.h"

#include "irfs/RootEval.h"
#include "gen/PointSpreadFunction.h"

#include "irfInterface/AcceptanceCone.h"

#include "Psf.h"

using namespace handoff_response;

namespace {
   static std::vector<double> psis;
   static std::vector<double> gammas;
   static std::vector<double> sigmas;
   void linearArray(double xmin, double xmax, size_t nx, 
                    std::vector<double> & xx, bool clear=true) {
      if (clear) {
         xx.clear();
      }
      double xstep((xmax - xmin)/(nx-1));
      for (size_t i(0); i < nx; i++) {
         xx.push_back(xstep*i + xmin);
      }
   }
   void logArray(double xmin, double xmax, size_t nx, 
                 std::vector<double> & xx, bool clear=true) {
      if (clear) {
         xx.clear();
      }
      double xstep(std::log(xmax/xmin)/(nx-1));
      for (size_t i(0); i < nx; i++) {
         xx.push_back(xmin*std::exp(xstep*i));
      }
   }
   void fillParamArrays() {
      size_t npsi(500);
      double psimin(0);
      double psimax(60*M_PI/180.);
      linearArray(psimin, psimax, npsi, psis);

      size_t ngam(50);
// These upper and lower values mirror the parameter fit boundaries in
// PointSpreadFunction.cxx, which are also inside anonymous namespace
// and so are inaccessible outside of that file.
      linearArray(1, 1.2, ngam, gammas);
      linearArray(1.2, 5.1, ngam, gammas, false);

      size_t nsig(50);
      double zdir;
      bool front;
// Smallest angular scales expected at highest energies, also use
// hard-wired values for scaled sigma range in PointSpreadFunction.cxx
      double sigmin(PointSpreadFunction::scaleFactor(5.62e6, zdir=1, 
                                                     front=true)*0.15);
      double sigmax(PointSpreadFunction::scaleFactor(30, zdir,
                                                     front=false)*2.0);
      logArray(sigmin, sigmax, nsig, sigmas);
   }

   double psf_function(double u, double gamma) {
      return (1. - 1./gamma)*std::pow(1. + u/gamma, -gamma);
   }

/**
 * @class ParMap
 * @brief Decorator class to provide easy const access to map values.
 */
   class ParMap {
   public:
      ParMap(const std::map<std::string, double> & pars) : m_pars(pars) {}
      double operator[](const std::string & keyname) const {
         std::map<std::string, double>::const_iterator item =
            m_pars.find(keyname);
         if (item == m_pars.end()) {
            std::ostringstream message;
            message << "Psf::angularIntegral:\n"
                    << "cannot find parameter named "
                    << keyname;
            throw std::runtime_error(message.str());
         }
         return item->second;
      }
   private:
      std::map<std::string, double> m_pars;
   };
} // anonymous namespace

double Psf::s_gamma;
double Psf::s_sigma;
double Psf::s_cp;
double Psf::s_sp;
double Psf::s_cr;

Psf::Psf() : m_eval(0), m_acceptanceCone(0) {}

Psf::Psf(handoff_response::IrfEval * eval) 
   : m_eval(eval), m_haveAngularIntegrals(false), m_acceptanceCone(0) {}

Psf::~Psf() {
   delete m_acceptanceCone;
}

double Psf::value(const astro::SkyDir & appDir, 
                  double energy, 
                  const astro::SkyDir & srcDir, 
                  const astro::SkyDir & scZAxis,
                  const astro::SkyDir &, double) const 
{
    // Angle between photon and source directions in radians.
    double separation = appDir.difference(srcDir);

    // Inclination wrt spacecraft z-axis in radians
    double inc = srcDir.difference(scZAxis);

    return value(separation*180./M_PI, energy, inc*180./M_PI, 0.);
}

double Psf::value(double separation, double energy, double theta,
                  double phi, double) const 
{
   if (theta < 0) {
      std::ostringstream message;
      message << "handoff::Psf::value(...):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }
   // (back to radians)
   return m_eval->psf(separation*M_PI/180., energy, theta, phi);
}

double Psf::angularIntegral(double energy, double theta, 
                            double phi, double radius, double /*time*/) const 
{
   double check =  m_eval->psf_integral(radius*M_PI/180., energy, theta, phi);
#if 0 // enable to test
   double time(0);
   double integral = IPsf::angularIntegral(energy, theta, phi, radius, time);
   assert(std::fabs(check - integral) < 1e-2);
#endif
   return check;
}

double Psf::angularIntegral(double energy,
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

double Psf::angularIntegral(double energy, 
                            const astro::SkyDir & srcDir,
                            double theta, 
                            double phi, 
                            const std::vector<irfInterface::AcceptanceCone *> 
                            & acceptanceCones, double time) {
   (void)(phi);
   (void)(time);
   if (!m_haveAngularIntegrals || *m_acceptanceCone != *(acceptanceCones[0])) {
      m_acceptanceCone = acceptanceCones[0]->clone();
      setupAngularIntegrals();
      m_haveAngularIntegrals = true;
   }
   double psi(srcDir.difference(m_acceptanceCone->center()));
   size_t ii(std::upper_bound(psis.begin(), psis.end(), psi) 
             - psis.begin() - 1);

   std::map<std::string, double> my_pars;

   handoff_response::RootEval * rootEval
      = dynamic_cast<handoff_response::RootEval *>(m_eval);
   if (rootEval == 0) {
      throw std::runtime_error("handoff_response::Psf::angularIntegral\n"
                               "Cannot downcast to RootEval.");
   }
   rootEval->getPsfPars(energy, theta, my_pars);

   ParMap pars(my_pars);

   double ncore(pars["ncore"]);
   double ntail(pars["ntail"]);
   double sigma(pars["sigma"]);
   double gcore(pars["gcore"]);
   double gtail(pars["gtail"]);
   
// // Uncached calculation for testing.
//    return (ncore*psfIntegral(psi, sigma, gcore) + 
//            ntail*psfIntegral(psi, sigma, gtail));

   /// Remove sigma**2 scaling imposed by RootEval.  This is put back
   /// in angularIntegral below for each grid value of sigmas.  This
   /// preserves the normalization in the bilinear interpolation by
   /// explicitly putting in the important sigma-dependence.
   ncore *= sigma*sigma;
   ntail *= sigma*sigma;

   double y1(ncore*angularIntegral(sigma, gcore, ii) + 
             ntail*angularIntegral(sigma, gtail, ii));
   double y2(ncore*angularIntegral(sigma, gcore, ii+1) + 
             ntail*angularIntegral(sigma, gtail, ii+1));

   double y = ((psi - psis.at(ii))/(psis.at(ii+1) - psis.at(ii))
               *(y2 - y1)) + y1;

   return y;
}

double Psf::angularIntegral(double sigma, double gamma, size_t ipsi) {
   size_t isig(std::upper_bound(sigmas.begin(), sigmas.end(), sigma)
               - sigmas.begin() - 1);
   size_t igam(std::upper_bound(gammas.begin(), gammas.end(), gamma)
               - gammas.begin() - 1);
   if (isig == sigmas.size() - 1 || igam == gammas.size() - 1) {
      return psfIntegral(psis.at(ipsi), sigma, gamma);
   }

   size_t is[2] = {isig, isig + 1};
   size_t ig[2] = {igam, igam + 1};

   for (size_t i(0); i < 2; i++) {
      for (size_t j(0); j < 2; j++) {
         size_t indx(is[i]*gammas.size() + ig[j]);
         if (m_needIntegral.at(ipsi).at(indx)) {
            m_angularIntegral.at(ipsi).at(indx) =
               psfIntegral(psis.at(ipsi), sigmas.at(is[i]), gammas.at(ig[j]))
               /sigmas.at(is[i])/sigmas.at(is[i]);
            m_needIntegral.at(ipsi).at(indx) = false;
         }
      }
   }

   return bilinear(sigma, gamma, ipsi, isig, igam);
}

double Psf::bilinear(double sigma, double gamma, size_t ipsi,
                     size_t isig, size_t igam) const {
   double tt = (gamma - gammas.at(igam))/(gammas.at(igam+1) - gammas.at(igam));
//   double uu = (sigma - sigmas.at(isig))/(sigmas.at(isig+1) - sigmas.at(isig));
   double uu = (log(sigma) - log(sigmas.at(isig)))
      /(log(sigmas.at(isig+1)) - log(sigmas.at(isig)));
   double y1 = m_angularIntegral.at(ipsi).at(isig*gammas.size() + igam);
   double y2 = m_angularIntegral.at(ipsi).at(isig*gammas.size() + igam + 1);
   double y3 = m_angularIntegral.at(ipsi).at((isig + 1)*gammas.size() + igam);
   double y4 = 
      m_angularIntegral.at(ipsi).at((isig + 1)*gammas.size() + igam + 1);
   double value = (1. - tt)*(1. - uu)*y1 + tt*(1. - uu)*y2 
      + tt*uu*y3 + (1. - tt)*uu*y4;
   return value;
}

void Psf::setupAngularIntegrals() {
   if (::psis.size() == 0) {
      ::fillParamArrays();
   }
   m_angularIntegral.clear();
   m_needIntegral.clear();
   size_t npts(::gammas.size()*::sigmas.size());
   for (size_t i(0); i < psis.size(); i++) {
      std::vector<double> drow(npts, 0);
      std::vector<bool> brow(npts, true);
      m_angularIntegral.push_back(drow);
      m_needIntegral.push_back(brow);
   }
}

double Psf::psfIntegral(double psi, double sigma, double gamma) {
   s_sigma = sigma;
   s_gamma = gamma;

   const irfInterface::AcceptanceCone & roiCone(*m_acceptanceCone);
   double roi_radius(roiCone.radius()*M_PI/180.);
   
   double one(1.);
   double mup(std::cos(roi_radius + psi));
   double mum(std::cos(roi_radius - psi));
   
   s_cp = std::cos(psi);
   s_sp = std::sin(psi);
   s_cr = std::cos(roi_radius);

   double err(1e-5);
   long ierr(0);

   double firstIntegral(0);
   if (psi < roi_radius) {
      dgaus8_(&psfIntegrand1, &mum, &one, &err, &firstIntegral, &ierr);
   }
   
   double secondIntegral(0);
   dgaus8_(&psfIntegrand2, &mup, &mum, &err, &secondIntegral, &ierr);

   return firstIntegral + secondIntegral;
}

double Psf::psfIntegrand1(double * mu) {
   double r(acos(*mu)/s_sigma);
   double u(r*r/2.);
   return 2.*M_PI*psf_function(u, s_gamma);
}

double Psf::psfIntegrand2(double * mu) {
   double r(acos(*mu)/s_sigma);
   double u(r*r/2.);
   double phimin(0);
   double arg((s_cr - *mu*s_cp)/std::sqrt(1. - *mu*(*mu))/s_sp);
   if (arg >= 1.) {
      phimin = 0;
   } else if (arg <= -1.) {
      phimin = M_PI;
   } else {
      phimin = std::acos(arg);
   }
   return 2.*phimin*psf_function(u, s_gamma);
}

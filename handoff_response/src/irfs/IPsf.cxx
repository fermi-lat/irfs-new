/**
 * @file IPsf.cxx
 * @brief Temporary class to provide default implementation of factorable
 * parts of irfInterface::IPsf.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "astro/SkyDir.h"

#include "st_facilities/dgaus8.h"

#include "IPsf.h"

#include <algorithm>
namespace handoff_response {

double IPsf::s_energy(1e3);
double IPsf::s_theta(0);
double IPsf::s_phi(0);
const IPsf * IPsf::s_self(0);

std::vector<double> IPsf::s_psi_values;

IPsf::IPsf() {
   if (s_psi_values.size() == 0) {
      fill_psi_values();
   }
}

astro::SkyDir IPsf::appDir(double energy,
                           const astro::SkyDir & srcDir,
                           const astro::SkyDir & scZAxis,
                           const astro::SkyDir & scXAxis,
                           double time) const {
   (void)(scXAxis);
   (void)(time);

// Compute source inclination
   double theta(srcDir.difference(scZAxis)*180./M_PI);

   setStaticVariables(energy, theta, 0, this);

// Form cumlative distribution in psi (polar angle from source direction)
   std::vector<double> integrand;
   for (std::vector<double>::iterator psi(s_psi_values.begin());
        psi != s_psi_values.end(); ++psi) {
      integrand.push_back(coneIntegrand(&(*psi)));
   }

   std::vector<double> integralDist;
   integralDist.push_back(0);
   for (size_t i = 1; i < s_psi_values.size(); i++) {
      integralDist.push_back(integralDist.at(i-1) + 
                             (integrand.at(i) + integrand.at(i-1))/2.
                             *(s_psi_values.at(i) - s_psi_values.at(i-1)));
   }

   double xi(CLHEP::RandFlat::shoot()*integralDist.back());
   size_t indx = std::upper_bound(integralDist.begin(), integralDist.end(), xi)
      - integralDist.begin() - 1;
   double psi( (xi - integralDist.at(indx))
               /(integralDist.at(indx+1) - integralDist.at(indx))
               *(s_psi_values.at(indx+1) - s_psi_values.at(indx))
               + s_psi_values.at(indx));
   double azimuth(2.*M_PI*RandFlat::shoot());
   
   astro::SkyDir appDir(srcDir);
   astro::SkyDir zAxis(scZAxis);
   
   CLHEP::Hep3Vector srcVec(appDir());
   CLHEP::Hep3Vector arbitraryVec(srcVec.x() + 1., srcVec.y() + 1.,
                                  srcVec.z() + 1.);
   CLHEP::Hep3Vector xVec(srcVec.cross(arbitraryVec.unit()));

   appDir().rotate(psi*M_PI/180., xVec).rotate(azimuth, srcVec);

   return appDir;
}

double IPsf::angularIntegral(double energy, double theta, 
                             double phi, double radius, double time) const {
   (void)(time);
   setStaticVariables(energy, theta, phi, this);
   double integral;
   double err(1e-5);
   long ierr(0);
   double zero(0);
   dgaus8_(&coneIntegrand, &zero, &radius, &err, &integral, &ierr);
   return integral;
}

void IPsf::setStaticVariables(double energy, double theta, double phi,
                              const IPsf * self) {
   s_energy = energy;
   s_theta = theta;
   s_phi = phi;
   s_self = self;
}

double IPsf::coneIntegrand(double * offset) {
   return s_self->value(*offset, s_energy, s_theta, s_phi)
      *std::sin(*offset*M_PI/180.)*2.*M_PI*M_PI/180.;
}

void IPsf::fill_psi_values() {
   double psi_min(1e-4);
   double psi_max(90.);
   size_t npsi(100);
   double dpsi(std::log(psi_max/psi_min)/(npsi-1));
   s_psi_values.push_back(0);
   for (size_t i = 0; i < npsi; i++) {
      s_psi_values.push_back(psi_min*std::exp(dpsi*i));
   }
}

} // namespace handoff_response

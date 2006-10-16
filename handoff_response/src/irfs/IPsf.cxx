/**
 * @file IPsf.cxx
 * @brief Temporary class to provide default implementation of factorable
 * parts of irfInterface::IPsf.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include "st_facilities/dgaus8.h"

#include "IPsf.h"

namespace handoff_response {

double IPsf::s_energy(1e3);
double IPsf::s_theta(0);
double IPsf::s_phi(0);
const IPsf * IPsf::s_self(0);

double IPsf::angularIntegral(double energy, double theta, 
                             double phi, double radius, double time) const {
   (void)(time);
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

double IPsf::coneIntegrand(double * offset) {
   return s_self->value(*offset, s_energy, s_theta, s_phi)
      *std::sin(*offset*M_PI/180.)*2.*M_PI*M_PI/180.;
}

} // namespace handoff_response

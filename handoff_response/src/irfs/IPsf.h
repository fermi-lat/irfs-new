/** 
 * @file IPsf.h
 * @brief IPsf class definition.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef handoff_response_IPsf_h
#define handoff_response_IPsf_h

#include <vector>

#include "irfInterface/IPsf.h"

namespace astro {
   class SkyDir;
}

namespace handoff_response {

/** 
 * @class IPsf
 *
 * @brief Abstract interface for the LAT point spread function classes.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class IPsf : public irfInterface::IPsf {
    
public:

   IPsf();

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir & srcDir,
                                const astro::SkyDir & scZAxis,
                                const astro::SkyDir & scXAxis,
                                double time=0) const;

   /// Angular integral of the PSF in instrument coordinates. 
   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis,
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones,
                   double time=0);

   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones,
                   double time=0);

private:

   static double s_energy;
   static double s_theta;
   static double s_phi;
   static const IPsf * s_self;
   static double coneIntegrand(double * offset);
   static void setStaticVariables(double energy, double theta, double phi,
                                  const IPsf * self);

   static std::vector<double> s_psi_values;

   void fill_psi_values();

   static double s_cp;
   static double s_sp;
   static double s_cr;

   static double psfIntegrand1(double * mu);

   static double psfIntegrand2(double * mu);

};

} // namespace handoff_response

#endif // handoff_response_IPsf_h

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

   /// Angular integral of the PSF in instrument coordinates. 
   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

protected:

   static double s_energy;
   static double s_theta;
   static double s_phi;
   static const IPsf * s_self;
   static double coneIntegrand(double * offset);

};

} // namespace handoff_response

#endif // handoff_response_IPsf_h

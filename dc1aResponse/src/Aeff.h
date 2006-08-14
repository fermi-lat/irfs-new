/**
 * @file Aeff.h
 * @brief Aeff class declaration for dc1aResponse package
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */
  
#ifndef dc1aResponse_Aeff_h
#define dc1aResponse_Aeff_h

#include <cmath>

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

namespace dc1aResponse {

/**
 * @class Aeff
 * @brief Aeff class implementation for test
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */
  
class Aeff : public irfInterface::IAeff {

public:

   Aeff(std::vector<double> & params) : m_params(params) {}

   Aeff(double p0, double p1, double p2) {
      m_params.clear();
      m_params.push_back(p0);
      m_params.push_back(p1);
      m_params.push_back(p2);
   }

   virtual ~Aeff() {}

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis, 
                        double time=0) const;

   virtual double value(double energy, double theta, double, 
                        double time=0) const;

   virtual Aeff * clone() {return new Aeff(*this);}

   virtual double upperLimit() const;

private:

   std::vector<double> m_params;

};

} // namespace dc1aResponse

#endif // dc1aResponse_Aeff_h

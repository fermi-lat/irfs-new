/**
 * @brief Class to handle Aeffs that change from epoch to epoch.  This
 * should eventually be refactored to be the "standard"
 * implementation, rather than treated as a special case.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 * 
 * $Header$
 */

#ifndef _latResponse_AeffEpochDep_h
#define _latResponse_AeffEpochDep_h

#include <string>
#include <vector>

#include "Aeff.h"
#include "EpochDep.h"

namespace latResponse {

class AeffEpochDep : public irfInterface::IAeff, EpochDep {

public:

   AeffEpochDep(const std::vector<std::string> & fitsfiles, 
                const std::string & extname="EFFECTIVE AREA",
                size_t nrow=0);
   
   virtual ~AeffEpochDep() {}
   
   virtual double value(double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time) const;
   
   virtual double value(double energy, double theta, double phi,
                        double time) const;
   
   virtual AeffEpochDep * clone() {
      return new AeffEpochDep(*this);
   }

   virtual double upperLimit() const;

private:

   std::vector<Aeff> m_aeffs;

   double m_upperLimit;

};

} // namespace latResponse

#endif // _latResponse_AeffEpochDep_h

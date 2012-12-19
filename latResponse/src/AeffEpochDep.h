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

#include "irfInterface/IAeff.h"
#include "EpochDep.h"

namespace latResponse {

class AeffEpochDep : public irfInterface::IAeff, EpochDep {

public:

   AeffEpochDep();

   AeffEpochDep(const AeffEpochDep & other);

   AeffEpochDep & operator=(const AeffEpochDep & rhs);
   
   virtual ~AeffEpochDep();
   
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

   void addAeff(const irfInterface::IAeff & aeff, double epoch_start);
   
private:

   std::vector<irfInterface::IAeff *> m_aeffs;

   double m_upperLimit;

};

} // namespace latResponse

#endif // _latResponse_AeffEpochDep_h

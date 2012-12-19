/**
 * @file EdispEpochDep.h
 * @brief Declaration for class to handle Edisp2 that changes from
 * epoch to epoch.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_EdispEpochDep_h
#define latResponse_EdispEpochDep_h

#include <map>
#include <vector>

#include "Edisp2.h"
#include "EpochDep.h"

namespace latResponse {

class EdispEpochDep : public irfInterface::IEdisp, EpochDep {

public:

   EdispEpochDep(const std::vector<std::string> & fitsfiles,
                 const std::string & extname="ENERGY DISPERSION",
                 size_t nrow=0);
   
   virtual ~EdispEpochDep() {}

   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi,
                        double time) const;

   virtual EdispEpochDep * clone() {
      return new EdispEpochDep(*this);
   }

private:

   std::vector<Edisp2> m_edisps;

};

} // namespace latResponse

#endif // latResponse_EdispEpochDep_h

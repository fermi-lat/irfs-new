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

#include <string>
#include <vector>

#include "irfInterface/IEdisp.h"

#include "EpochDep.h"

namespace latResponse {

class EdispEpochDep : public irfInterface::IEdisp, EpochDep {

public:

   EdispEpochDep();

   EdispEpochDep(const EdispEpochDep & other);
   
   EdispEpochDep & operator=(const EdispEpochDep & rhs);
   
   virtual ~EdispEpochDep();

   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi,
                        double time) const;

   virtual irfInterface::IEdisp * clone() {
      return new EdispEpochDep(*this);
   }

   void addEdisp(const irfInterface::IEdisp & edisp,
                 double epoch_start);

private:

   std::vector<irfInterface::IEdisp *> m_edisps;

};

} // namespace latResponse

#endif // latResponse_EdispEpochDep_h


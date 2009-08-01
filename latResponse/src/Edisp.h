/**
 * @file Edisp.h
 * @brief Edisp class definition for post-handoff review IRFs.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Edisp_h
#define latResponse_Edisp_h

#include <map>
#include <vector>

#include "irfInterface/IEdisp.h"

#include "latResponse/ParTables.h"

namespace latResponse {

/**
 * @class Edisp
 * @brief Edisp class for post-handoff review IRFs.
 */

class Edisp : public irfInterface::IEdisp {

public:

   Edisp(const std::string & fitsfile,
         const std::string & extname="ENERGY DISPERSION",
         size_t nrow=0);

   virtual ~Edisp() {}

   /// A member function returning the energy dispersion function.
   /// @param appEnergy measured photon energy in MeV.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time   MET
   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time=0) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi,
                        double time=0) const;

   virtual Edisp * clone() {
      return new Edisp(*this);
   }

   static double old_function(double xx, double * pars);

private:

   ParTables m_parTables;

   mutable double m_loge_last;
   mutable double m_costh_last;

   mutable double m_pars[10];

   double * pars(double energy, double costh) const;

};

} // namespace latResponse

#endif // latResponse_Edisp_h


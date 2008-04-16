/**
 * @file Edisp2.h
 * @brief Class definition for Riccardo's second generation energy
 * dispersion representation
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Edisp2_h
#define latResponse_Edisp2_h

#include <map>
#include <vector>

#include "irfInterface/IEdisp.h"

#include "latResponse/ParTables.h"

namespace latResponse {

/**
 * @class Edisp2
 * @brief Edisp2 class for Riccardo's second generation energy
 * dispersion representation
 */

class Edisp2 : public irfInterface::IEdisp {

public:

   Edisp2(const std::string & fitsfile,
         const std::string & extname="ENERGY DISPERSION",
         size_t nrow=0);

   virtual ~Edisp2() {}

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

   virtual Edisp2 * clone() {
      return new Edisp2(*this);
   }

   double scaleFactor(double energy, double costheta) const;

private:

   ParTables m_parTables;

   mutable double m_loge_last;
   mutable double m_costh_last;

   double * pars(double energy, double costh) const;

   std::vector<double> m_scalePars;
   double m_p1;
   double m_p2;
   double m_t0;

   double old_function(double xx, double * pars) const;

   void readScaling(const std::string & fitsfile,
                    const std::string & extname="EDISP_SCALING_PARAMS");

};

} // namespace latResponse

#endif // latResponse_Edisp2_h


/**
 * @file IEdisp.h
 * @brief IEdisp class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_IEdisp_h
#define handoff_IEdisp_h

#include <utility>
#include <vector>

#include "irfInterface/IEdisp.h"

namespace handoff_response {

/**
 * @class IEdisp
 *
 * @brief declare IEdisp subclass for handoff energy dispersion
 *
 */

class IEdisp : public irfInterface::IEdisp {

public:

   virtual double appEnergy(double energy, 
                            const astro::SkyDir & srcDir,
                            const astro::SkyDir & scZAxis,
                            const astro::SkyDir & scXAxis,
                            double time=0) const;

   /// Return the integral of the energy dispersion function over
   /// the specified interval in apparent energy.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time (MET seconds)
   virtual double integral(double emin, double emax, double energy,
                           const astro::SkyDir & srcDir, 
                           const astro::SkyDir & scZAxis,
                           const astro::SkyDir & scXAxis,
                           double time=0) const;

   /// Return the integral of the energy dispersion function 
   /// using instrument coordinates.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   /// @param time (MET seconds)
   virtual double integral(double emin, double emax, double energy, 
                           double theta, double phi,
                           double time=0) const;

private:

   static double s_energy;
   static double s_theta;
   static double s_phi;
   static const IEdisp * s_self;
   static double edispIntegrand(double * appEnergy);
   static void setStaticVariables(double energy, double theta, double phi,
                                  const IEdisp * self);
};

} // namespace handoff_response

#endif // handoff_IEdisp_h

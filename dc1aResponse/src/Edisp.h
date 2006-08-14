/**
 * @file Edisp.h
 * @brief Edisp class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1aResponse_Edisp_h
#define dc1aResponse_Edisp_h

#include "irfInterface/IEdisp.h"

namespace dc1aResponse {

/**
 * @class Edisp
 *
 * @brief A LAT energy dispersion class using a uniform 10% Gaussian
 * energy resolution.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Edisp : public irfInterface::IEdisp {

public:

   Edisp(double eSigma = 0.1) : m_eSigma(eSigma) {}

   virtual ~Edisp() {}

   /// A member function returning the energy dispersion function.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis, 
                        double time=0) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi, double time=0) const;

   virtual double appEnergy(double energy, 
                            const astro::SkyDir &srcDir,
                            const astro::SkyDir &scZAxis,
                            const astro::SkyDir &scXAxis,
                            double time=0) const;

   /// Return the integral of the energy dispersion function over
   /// the specified interval in apparent energy.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double integral(double emin, double emax, double energy,
                           const astro::SkyDir &srcDir, 
                           const astro::SkyDir &scZAxis,
                           const astro::SkyDir &scXAxis,
                           double time=0) const;

   /// Return the integral of the energy dispersion function 
   /// using instrument coordinates.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   /// @param time Photon arrival time (MET s)
   virtual double integral(double emin, double emax, double energy, 
                           double theta, double phi, double time=0) const;

   virtual Edisp * clone() {return new Edisp(*this);}

private:

   double m_eSigma;

};

} // namespace dc1aResponse

#endif // dc1aResponse_Edisp_h

/**
 * @file Edisp.h
 * @brief Edisp class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_Edisp_h
#define dc2Response_Edisp_h

#include "irfInterface/IEdisp.h"

#include "DC2.h"

namespace dc2Response {

/**
 * @class Edisp
 *
 * @brief A LAT energy dispersion class using the DC2 AllGamma data.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Edisp : public irfInterface::IEdisp, public DC2 {

public:

   Edisp(const std::string &filename);

   Edisp(const std::string &filename, int hdu, int npars);

   virtual ~Edisp() {}

   /// A member function returning the energy dispersion function.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi) const;

   virtual double appEnergy(double energy, 
                            const astro::SkyDir &srcDir,
                            const astro::SkyDir &scZAxis,
                            const astro::SkyDir &scXAxis) const;

   /// Return the integral of the energy dispersion function over
   /// the specified interval in apparent energy.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   virtual double integral(double emin, double emax, double energy,
                           const astro::SkyDir &srcDir, 
                           const astro::SkyDir &scZAxis,
                           const astro::SkyDir &scXAxis) const;

   /// Return the integral of the energy dispersion function 
   /// using instrument coordinates.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   virtual double integral(double emin, double emax, double energy, 
                           double theta, double phi) const;

   virtual Edisp * clone() {return new Edisp(*this);}

private:

   void normalizeDists();

};

} // namespace dc2Response

#endif // dc2Response_Edisp_h

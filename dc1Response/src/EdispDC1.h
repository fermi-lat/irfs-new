/**
 * @file EdispDC1.h
 * @brief EdispDC1 class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1Response_EdispDC1_h
#define dc1Response_EdispDC1_h

#include "irfInterface/IEdisp.h"

#include "DC1.h"

namespace dc1Response {

/**
 * @class EdispDC1
 *
 * @brief A LAT energy dispersion class using the DC1 AllGamma data.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class EdispDC1 : public irfInterface::IEdisp, public DC1 {

public:

   EdispDC1(const std::string &filename);

   EdispDC1(const std::string &filename, int hdu, int npars);

   virtual ~EdispDC1() {}

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

   virtual EdispDC1 * clone() {return new EdispDC1(*this);}

private:

   void normalizeDists();

};

} // namespace dc1Response

#endif // dc1Response_EdispDC1_h

/** 
 * @file IPsf.h
 * @brief IPsf class definition.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef irfInterface_IPsf_h
#define irfInterface_IPsf_h

#include <vector>

namespace astro {
   class SkyDir;
}

namespace irfInterface {

class AcceptanceCone;

/** 
 * @class IPsf
 *
 * @brief Abstract interface for the LAT point spread function classes.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class IPsf {
    
public:

   virtual ~IPsf() {}

   /// Pure virtual method to define the interface for the member
   /// function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double value(const astro::SkyDir &appDir, 
                        double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const = 0;

   /// Return the psf as a function of instrument coordinates.
   /// @param separation Angle between apparent and true photon directions
   ///        (degrees).
   /// @param energy True photon energy (MeV).
   /// @param theta True photon inclination angle (degrees).
   /// @param phi True photon azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   /// @param time Photon arrival time (MET s).
   virtual double value(double separation, double energy, double theta, 
                        double phi, double time=0) const = 0;

   /// This method is also virtual, in case the sub-classes wish to
   /// overload it.
   virtual double operator()(const astro::SkyDir &appDir, 
                             double energy, 
                             const astro::SkyDir &srcDir, 
                             const astro::SkyDir &scZAxis,
                             const astro::SkyDir &scXAxis,
                             double time=0) const {
      return value(appDir, energy, srcDir, scZAxis, scXAxis, time);}

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double angularIntegral(double energy,
                                  const astro::SkyDir &srcDir,
                                  const astro::SkyDir &scZAxis,
                                  const astro::SkyDir &scXAxis,
                                  const std::vector<AcceptanceCone *> 
                                  &acceptanceCones,
                                  double time=0) = 0;

   /// Angular integral of the PSF in instrument coordinates.
   /// @param energy True photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param theta True photon inclination wrt Spacecraft z-axis (degrees)
   /// @param phi True photon azimuth wrt SC x-axis (degrees)
   /// @param acceptanceCones Acceptance cones that define the domain of
   ///        the Psf angular integrals
   /// @param time Photon arrival time (MET s).
   virtual double angularIntegral(double energy, 
                                  const astro::SkyDir &srcDir,
                                  double theta, double phi,
                                  const std::vector<AcceptanceCone *> 
                                  &acceptanceCones,
                                  double time=0) = 0;
   
   /// Angular integral of the PSF in instrument coordinates.  This
   /// method is equivalent to a call to the previous version of
   /// angularIntegral for which a single AcceptanceCone is centered
   /// on the srcDir.
   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const = 0;

   /// Return a random apparent photon direction drawn from the
   /// PSF distribution.
   virtual astro::SkyDir appDir(double energy, 
                                const astro::SkyDir &srcDir,
                                const astro::SkyDir &scZAxis,
                                const astro::SkyDir &scXAxis,
                                double time=0) const = 0;

   virtual IPsf * clone() = 0;

};

} // namespace irfInterface

#endif // irfInterface_IPsf_h

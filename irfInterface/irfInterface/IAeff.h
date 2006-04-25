/** 
 * @file IAeff.h
 * @brief IAeff class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_IAeff_h
#define irfInterface_IAeff_h

namespace astro {
   class SkyDir;
}

namespace irfInterface {

/** 
 * @class IAeff
 *
 * @brief Abstract interface for the LAT effective area classes.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class IAeff {
    
public:

   virtual ~IAeff() {}

   /// Pure virtual method to define the interface for the member
   /// function returning the effective area in cm^2.
   /// @param energy True photon energy (MeV).
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const = 0;

   /// Return the effective area (cm^2) as a function of instrument 
   /// coordinates.
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   virtual double value(double energy, double theta, double phi) const = 0;

   /// This method is also virtual, in case the sub-classes wish to
   /// overload it.
   virtual double operator()(double energy, 
                             const astro::SkyDir &srcDir, 
                             const astro::SkyDir &scZAxis,
                             const astro::SkyDir &scXAxis) const {
      return value(energy, srcDir, scZAxis, scXAxis);}

   virtual IAeff * clone() = 0;

//    /// @return An absolute upper limit on the value of the effective
//    /// area for all energies and directions (cm^2).
//    virtual double upperLimit() const = 0;

};

} // namespace irfInterface

#endif // irfInterface_IAeff_h

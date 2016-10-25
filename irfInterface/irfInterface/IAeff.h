/** 
 * @file IAeff.h
 * @brief IAeff class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_IAeff_h
#define irfInterface_IAeff_h

#include <vector>

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
   /// @param time Photon arrival time (MET s).
   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const = 0;

   /// Return the effective area (cm^2) as a function of instrument 
   /// coordinates.
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   /// @param time Photon arrival time (MET s).
   virtual double value(double energy, double theta, double phi,
                        double time=0) const = 0;

   /// Vectorized version of value()
   virtual std::vector<double> value(const std::vector<double>& energy,
				     const std::vector<double>& theta,
				     const std::vector<double>& phi,
				     double time=0) const {
     if(energy.size() != theta.size() || energy.size() != phi.size())
       throw std::runtime_error("Input arrays must have same dimension.");

     std::vector<double> vals;
     vals.reserve(energy.size());
     std::vector<double>::const_iterator itr0 = energy.begin();
     std::vector<double>::const_iterator itr1 = theta.begin();
     std::vector<double>::const_iterator itr2 = phi.begin();  
     for(; (itr0 != energy.end()) && (itr1 != theta.end()) && 
	   (itr2 != phi.end()); 
	 ++itr0, ++itr1, ++itr2) {
       vals.push_back(value(*itr0,*itr1,*itr2,time));
     }
     return vals;     
   }

   /// This method is also virtual, in case the sub-classes wish to
   /// overload it.
   virtual double operator()(double energy, 
                             const astro::SkyDir &srcDir, 
                             const astro::SkyDir &scZAxis,
                             const astro::SkyDir &scXAxis,
                             double time=0) const {
      return value(energy, srcDir, scZAxis, scXAxis, time);}

   virtual IAeff * clone() = 0;

   /// @return An absolute upper limit on the value of the effective
   /// area for all energies and directions (cm^2).
   virtual double upperLimit() const = 0;

   virtual void setPhiDependence(bool usePhiDependence) {
      m_usePhiDependence = usePhiDependence;
   }

   virtual bool usePhiDependence() const {
      return m_usePhiDependence;
   }

protected:

   bool m_usePhiDependence;

};

} // namespace irfInterface

#endif // irfInterface_IAeff_h

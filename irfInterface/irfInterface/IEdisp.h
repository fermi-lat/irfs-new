/** 
 * @file IEdisp.h
 * @brief IEdisp class definition.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef irfInterface_IEdisp_h
#define irfInterface_IEdisp_h

namespace astro {
   class SkyDir;
}

namespace irfInterface {

/** 
 * @class IEdisp
 *
 * @brief Abstract interface for the LAT energy dispersion classes.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class IEdisp {
    
public:

   virtual ~IEdisp() {}

   /// Pure virtual method to define the interface for the member
   /// function returning the energy dispersion function value.
   /// @param appEnergy Apparent (reconstructed) photon energy in MeV.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s).
   virtual double value(double appEnergy,
                        double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis, 
                        double time=0) const = 0;

   /// Return the energy dispersion as a function of instrument
   /// coordinates.
   /// @param appEnergy Apparent photon energy (MeV).
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   /// @param time Photon arrival time (MET s).
   virtual double value(double appEnergy, double energy, 
                        double theta, double phi, double time=0) const = 0;

   /// This method is also virtual, in case the sub-classes wish to
   /// overload it.
   virtual double operator()(double appEnergy,
                             double energy, 
                             const astro::SkyDir & srcDir, 
                             const astro::SkyDir & scZAxis,
                             const astro::SkyDir & scXAxis,
                             double time=0) const
      {return value(appEnergy, energy, srcDir, scZAxis, scXAxis, time);}

   /// Return a randomly chosen apparent photon energy in MeV drawn
   /// from the energy dispersion function.
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
   /// @param time Photon arrival time (MET s).
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
   ///             X-axis (degrees).
   /// @param time Photon arrival time (MET s).
   virtual double integral(double emin, double emax, double energy, 
                           double theta, double phi, double time=0) const;

   /// @return Mean apparent photon energy (MeV)
   /// @param energy True photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s).
   virtual double meanAppEnergy(double energy,
                                const astro::SkyDir & srcDir, 
                                const astro::SkyDir & scZAxis,
                                const astro::SkyDir & scXAxis,
                                double time=0) const;

   /// @return Mean apparent photon energy (MeV)
   /// @param energy True photon energy (MeV)
   /// @param theta True inclination angle (degrees)
   /// @param phi True azimuthal angle wrt instrument x-axis (degrees)
   /// @param time Photon arrival time (MET s)
   virtual double meanAppEnergy(double energy, double theta, double phi,
                                double time=0) const;

   /// @return Mean true photon energy (MeV)
   /// @param appEnergy Measured photon energy (MeV)
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s).
   virtual double meanTrueEnergy(double appEnergy,
                                 const astro::SkyDir & srcDir, 
                                 const astro::SkyDir & scZAxis,
                                 const astro::SkyDir & scXAxis,
                                 double time=0) const;

   /// @return Mean true photon energy (MeV)
   /// @param appEnergy Measured photon energy (MeV)
   /// @param theta True inclination angle (degrees)
   /// @param phi True azimuthal angle wrt instrument x-axis (degrees)
   /// @param time Photon arrival time (MET s)
   virtual double meanTrueEnergy(double appEnergy, double theta, double phi,
                                 double time=0) const;

   virtual IEdisp * clone() = 0;

private:

   static double s_energy;
   static double s_theta;
   static double s_phi;
   static double s_time;
   static const IEdisp * s_self;
   static double edispIntegrand(double * appEnergy);
   static double meanEnergyIntegrand(double * appEnergy);
   static double trueEnergyIntegrand(double * energy);
   static double meanTrueEnergyIntegrand(double * energy);
   static void setStaticVariables(double energy, double theta, double phi,
                                  double time, const IEdisp * self);
};

} // namespace irfInterface

#endif // irfInterface_IEdisp_h

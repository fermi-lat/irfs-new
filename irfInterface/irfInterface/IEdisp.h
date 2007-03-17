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

   class EdispIntegrand {
   public:
      EdispIntegrand(const IEdisp & edisp, double energy, double theta,
                     double phi, double time) 
         : m_edisp(edisp), m_energy(energy), m_theta(theta), 
           m_phi(phi), m_time(time) {}
      double operator()(double appEnergy) const {
         return m_edisp.value(appEnergy, m_energy, m_theta, m_phi, m_time);
      }
   private:
      const IEdisp & m_edisp;
      double m_energy;
      double m_theta;
      double m_phi;
      double m_time;
   };

   class MeanEnergyIntegrand : public EdispIntegrand {
   public:
      MeanEnergyIntegrand(const IEdisp & edisp, double energy, double theta,
                          double phi, double time) 
         : EdispIntegrand(edisp, energy, theta, phi, time) {}
      double operator()(double appEnergy) const {
         return appEnergy*EdispIntegrand::operator()(appEnergy);
      }
   };

   class TrueEnergyIntegrand {
   public:
      TrueEnergyIntegrand(const IEdisp & edisp, double appEnergy, double theta,
                          double phi, double time)
         : m_edisp(edisp), m_appEnergy(appEnergy), m_theta(theta), m_phi(phi),
           m_time(time) {}
      double operator()(double energy) const {
         return m_edisp.value(m_appEnergy, energy, m_theta, m_phi, m_time);
      }
   private:
      const IEdisp & m_edisp;
      double m_appEnergy;
      double m_theta;
      double m_phi;
      double m_time;
   };

   class MeanTrueEnergyIntegrand : public TrueEnergyIntegrand {
   public:
      MeanTrueEnergyIntegrand(const IEdisp & edisp, double appEnergy, 
                              double theta, double phi, double time)
         : TrueEnergyIntegrand(edisp, appEnergy, theta, phi, time) {}
      double operator()(double energy) const {
         return energy*TrueEnergyIntegrand::operator()(energy);
      }
   };

   double adhocIntegrator(const EdispIntegrand & func, double emin, 
                          double emax) const;
};

} // namespace irfInterface

#endif // irfInterface_IEdisp_h

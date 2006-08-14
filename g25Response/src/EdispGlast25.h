/**
 * @file EdispGlast25.h
 * @brief EdispGlast25 class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_IEdispGlast25_h
#define g25Response_IEdispGlast25_h

#include "irfInterface/IEdisp.h"
#include "Glast25.h"

namespace g25Response {

/**
 * @class EdispGlast25
 *
 * @brief A LAT effective area class using the GLAST25 data.
 *
 * Since there are no GLAST25 data for the energy dispersion, the
 * constructor takes an argument that is the fractional energy
 * dispersion (independent of energy, direction, etc.) assuming a
 * Gaussian form.
 *
 * We can also use energy dispersion files prepared by Yasushi Ikebe.
 * See the constructors.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class EdispGlast25 : public irfInterface::IEdisp, public Glast25 {

public:

   /// Due to the lack of GLAST25 energy dispersion data, a fractional
   /// value can be set here.
   EdispGlast25(double fracSig = 0) : m_haveEdisp(false), m_fracSig(fracSig) {}

   /// This constructor reads in energy dispersion from CALDB 
   /// files prepared by Yasushi Ikebe.
   EdispGlast25(const std::string &filename, int hdu);

   virtual ~EdispGlast25() {}

   /// A member function returning the energy dispersion function.
   /// @param appEnergy Apparent photon energy (MeV).
   /// @param energy True photon energy (MeV).
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir& srcDir,
                        const astro::SkyDir& scZAxis,
                        const astro::SkyDir& scXAxis,
                        double time=0) const;

   /// Return the energy dispersion as a function of instrument
   /// coordinates.
   /// @param appEnergy Apparent photon energy (MeV).
   /// @param energy True photon energy (MeV).
   /// @param theta Inclination of true photon direction wrt the 
   ///        spacecraft z-axis (degrees).
   /// @param phi Azimuthal angle of true photon direction wrt the 
   ///        spacecraft x-axis (degrees).
   /// @param time Photon arrival time (MET s)
   virtual double value(double appEnergy, double energy,
                        double theta, double phi, double time=0) const;

   /// Return photon apparent energy drawn from the energy dispersion 
   /// function.
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

   virtual EdispGlast25 * clone() {return new EdispGlast25(*this);}

private:

   bool m_haveEdisp;
   double m_fracSig;

   std::vector<double> m_energies;
   std::vector<double> m_sigmas;

   /// Return the Gaussian width in MeV.
   double sigma(double energy) const;

};

} // namespace g25Response

#endif // g25Response_IEdispGlast25_h


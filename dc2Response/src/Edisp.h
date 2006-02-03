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
 * @brief LAT energy dispersion for DC2 
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Edisp : public irfInterface::IEdisp, public DC2 {

public:

   Edisp(const std::string & fitsfile, cons std::string & extname);

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

   /// @brief The parameters as a vector over bins in energy and
   /// inclination.  If i is the inclination bin and k is the energy
   /// bin, then the set of parameters for that bin are
   /// m_pars.at(i*m_logE.size() + k).
   std::vector< std::vector<double> > m_pars;

   std::vector<double> m_logElo;
   std::vector<double> m_logEhi;
   std::vector<double> m_logE;
   std::vector<double> m_cosinc;

   void readData();

   const std::vector<double> & fitParams(double McEnergy, double McZDir);

};

} // namespace dc2Response

#endif // dc2Response_Edisp_h

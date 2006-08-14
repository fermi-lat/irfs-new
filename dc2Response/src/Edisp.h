/**
 * @file Edisp.h
 * @brief Edisp class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_Edisp_h
#define dc2Response_Edisp_h

#include <utility>

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

   Edisp(const std::string & fitsfile, const std::string & extname);

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

   std::vector<double> m_eBounds;
   std::vector<double> m_muBounds;

   std::vector<double> m_rwidth;
   std::vector<double> m_ltail;
   std::vector<double> m_norms;

   std::vector< std::vector<float> > m_cumDists;

   void readData();

   double value(double appEnergy, double energy) const;

   size_t parIndex(double energy, double mu) const;

   static std::vector<double> s_xvals;
   void computeCumulativeDists();

   static double s_rwidth;
   static double s_ltail;

   static double edispIntegrand(double * x);

};

} // namespace dc2Response

#endif // dc2Response_Edisp_h

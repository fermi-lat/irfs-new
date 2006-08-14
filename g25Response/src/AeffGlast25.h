/**
 * @file AeffGlast25.h
 * @brief AeffGlast25 class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_IAeffGlast25_h
#define g25Response_IAeffGlast25_h

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

#include "Glast25.h"

namespace g25Response {

/**
 * @class AeffGlast25
 *
 * @brief A LAT effective area class using the GLAST25 data.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class AeffGlast25 : public irfInterface::IAeff, public Glast25 {

public:

   AeffGlast25(const std::string &filename, int hdu, double fudge=1.);

   virtual ~AeffGlast25() {}

   /// A member function returning the effective area in cm.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const;

   virtual double value(double energy, double theta, double,
                        double time=0) const;

   virtual AeffGlast25 * clone() {return new AeffGlast25(*this);}

   virtual double upperLimit() const;

private:

   double value(double energy, double inclination) const;
   void readAeffData();

   /// Multiplicative fudge factor for mocking up multiple responses.
   double m_fudge;

   std::vector<double> m_energy;
   std::vector<double> m_logEnergy;
   std::vector<double> m_theta;
   std::vector<double> m_aeff;

   double m_aeffMax;

};

} // namespace g25Response

#endif // g25Response_IAeffGlast25_h


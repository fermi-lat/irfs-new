/**
 * @file Aeff.h
 * @brief Aeff class declaration for testResponse package
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */
  
#ifndef testResponse_Aeff_h
#define testResponse_Aeff_h

#include <cmath>

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

namespace testResponse {

/**
 * @class Aeff
 * @brief Aeff class implementation for test
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */
  
class Aeff : public irfInterface::IAeff {

public:

   Aeff(const std::string &filename, int hdu);

   virtual ~Aeff();

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double energy, double theta, double) const;

   virtual Aeff * clone() {return new Aeff(*this);}

private:

   std::string m_filename;
   int m_hdu;
   std::vector<double> m_energy;
   std::vector<double> m_theta;
   std::vector<double> m_aeffTable;

   void read_FITS_table();
};

} // namespace testResponse

#endif // testResponse_Aeff_h

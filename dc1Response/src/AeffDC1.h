/**
 * @file AeffDC1.h
 * @brief Aeff class declaration for DC1.
 * @author Johann Cohen-Tanugi
 *
 * $Header$
 */
  
#ifndef dc1Response_AeffDC1_h
#define dc1Response_AeffDC1_h

#include <cmath>

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

#include "DC1.h"

namespace dc1Response {

/**
 * @class AeffDC1
 * @brief Aeff class implementation for DC1.
 * @author Johann Cohen-Tanugi
 *
 * $Header$
 */
  
class AeffDC1 : public irfInterface::IAeff, public DC1 {

public:

   AeffDC1(const std::string &filename, int hdu);
   virtual ~AeffDC1();

   AeffDC1(const AeffDC1 &rhs);

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const;

   virtual double value(double energy, double theta, double,
                        double time=0) const;

   virtual AeffDC1 * clone() {return new AeffDC1(*this);}

   virtual double upperLimit() const;

private:

   std::vector<double> m_aeffTable;

   double m_aeffMax;

   void read_FITS_table();
   int getAeffIndex(double energy, double theta) const;

};

} // namespace dc1Response

#endif // dc1Response_AeffDC1_h

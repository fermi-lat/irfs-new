/**
 * @file Aeff.h
 * @brief Aeff class declaration for DC2.
 * @author Johann Cohen-Tanugi
 *
 * $Header$
 */
  
#ifndef dc2Response_Aeff_h
#define dc2Response_Aeff_h

#include <cmath>

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

#include "DC2.h"

namespace irfUtil {
   class RootTable;
}

namespace dc2Response {

/**
 * @class Aeff
 * @brief Aeff class implementation for DC2.
 * @author Johann Cohen-Tanugi
 *
 * $Header$
 */
  
class Aeff : public irfInterface::IAeff, public DC2 {

public:

   Aeff(const std::string &filename, bool getFront=true);
   Aeff(const std::string &filename, int hdu);

   virtual ~Aeff();

   Aeff(const Aeff &rhs);

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double energy, double theta, double) const;

   virtual Aeff * clone() {return new Aeff(*this);}

   double AeffValueFromTable(const double energy, const double theta) const;

private:

   irfUtil::RootTable * m_aeff;
   std::vector<double> m_aeffTable;
   std::string m_histName;

   void readAeffTable();
   void read_FITS_table();
   int getAeffIndex(double energy, double theta) const;

};

} // namespace dc2Response

#endif // dc2Response_Aeff_h

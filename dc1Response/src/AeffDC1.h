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

#include "RootTable.h"
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

   /// Copy constructor
   AeffDC1(const AeffDC1 &rhs);

   AeffDC1(const std::string &filename, bool getFront=true);
   AeffDC1(const std::string &filename, int hdu);

   virtual ~AeffDC1();
   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double energy, double theta, double) const;

   virtual AeffDC1 * clone() {return new AeffDC1(*this);}

   double AeffValueFromTable(const double energy, const double theta) const;

private:

   RootTable * m_aeff;
   std::vector<double> m_aeffTable;
   std::string m_histName;

   void readAeffTable();
   void read_FITS_table();
   int getAeffIndex(double energy, double theta) const;

};

} // namespace dc1Response

#endif // dc1Response_AeffDC1_h

/**
 * @file Aeff.h
 * @brief Aeff class declaration for DC2.
 * @author J. Chiang
 *
 * $Header$
 */
  
#ifndef dc2Response_Aeff_h
#define dc2Response_Aeff_h

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

#include "DC2.h"

namespace dc2Response {

/**
 * @class Aeff
 * @brief Aeff class implementation for DC2.
 * @author J. Chiang
 *
 * $Header$
 */
  
class Aeff : public irfInterface::IAeff, public DC2 {

public:

   Aeff(const std::string & filename, const std::string & extname);

   Aeff(const Aeff & rhs);

   virtual ~Aeff() {}

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis, 
                        double time=0) const;

   virtual double value(double energy, double theta, double,
                        double time=0) const;

   virtual Aeff * clone() {return new Aeff(*this);}

   virtual double upperLimit() const;

private:

   std::vector< std::vector<double> > m_effArea;
   std::vector<double> m_logElo;
   std::vector<double> m_logEhi;
   std::vector<double> m_logE;
   std::vector<double> m_cosinc;
   double m_aeffMax;

   void readData();

};

} // namespace dc2Response

#endif // dc2Response_Aeff_h

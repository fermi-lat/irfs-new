/**
 * @file Aeff.h
 * @brief Aeff class declaration for post-handoff review IRFs.
 * @author J. Chiang
 *
 * $Header$
 */
  
#ifndef handoff_Aeff_h
#define handoff_Aeff_h

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

#include "latResponse/FitsTable.h"

namespace latResponse {

/**
 * @class Aeff
 * @brief Effective area class declaration.
 */
  
class Aeff : public irfInterface::IAeff {

public:

   Aeff(const std::string & fitsfile, 
        const std::string & extname="EFFECTIVE AREA",
        size_t nrow=0);

   virtual ~Aeff() {}
   
   virtual double value(double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time=0) const;
   
   virtual double value(double energy, double theta, double phi,
                        double time=0) const;
   
   virtual Aeff * clone() {
      return new Aeff(*this);
   }

   virtual double upperLimit() const;

private:

   FitsTable m_aeffTable;

};

} // namespace latResponse

#endif // latResponse_Aeff_h

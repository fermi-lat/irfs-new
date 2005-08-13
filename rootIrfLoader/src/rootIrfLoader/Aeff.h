/** 
 * @file Aeff.h
 * @brief Aeff class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef rootIrfLoader_Aeff_h
#define rootIrfLoader_Aeff_h

#include <string>

#include "IrfBase.h"

namespace irfInterface {
   class IAeff;
}

namespace rootIrfLoader {

/** 
 * @class Aeff
 *
 * @brief Emasculated wrapper interface for the LAT effective area classes
 * for use with ROOT.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Aeff : public IrfBase {
    
public:

   Aeff(const std::string & irfs="DC1A::Front");

   Aeff(const Aeff & rhs);

   Aeff & operator=(const Aeff & rhs);

   ~Aeff();

   bool operator==(const Aeff & rhs) const;

   bool operator!=(const Aeff & rhs) const {
      return !operator==(rhs);
   }

   /// Return the effective area (cm^2) as a function of instrument 
   /// coordinates.
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   double operator()(double energy, double theta, double phi) const;

protected:

   virtual void getRef(irfInterface::Irfs * irfs);

private:

   irfInterface::IAeff * m_aeff;

};

} // namespace rootIrfLoader

#endif // rootIrfLoader_Aeff_h

/** 
 * @file Edisp.h
 * @brief Edisp class definition.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef rootIrfLoader_Edisp_h
#define rootIrfLoader_Edisp_h

#include <string>

#include "IrfBase.h"

namespace irfInterface {
   class IEdisp;
}

namespace rootIrfLoader {

/** 
 * @class Edisp
 *
 * @brief Emasculated wrapper interface for the LAT energy dispersion classes
 * for use with ROOT.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Edisp : public IrfBase {
    
public:

   Edisp(const std::string & irfs="DC1A::Front");

   Edisp(const Edisp & rhs);

   Edisp & operator=(const Edisp & rhs);

   ~Edisp();

   bool operator==(const Edisp & rhs) const;

   bool operator!=(const Edisp & rhs) const {
      return !operator==(rhs);
   }

   /// Return the energy dispersion as a function of instrument
   /// coordinates.
   /// @param appEnergy Apparent photon energy (MeV).
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   double operator()(double appEnergy, double energy, 
                     double theta, double phi) const;

   /// Return the integral of the energy dispersion function 
   /// using instrument coordinates.
   /// @param emin Apparent energy lower bound (MeV)
   /// @param emax Apparent energy upper bound (MeV)
   /// @param energy True photon energy (MeV).
   /// @param theta True inclination angle (degrees).
   /// @param phi True azimuthal angle measured wrt the instrument
   ///             X-axis (degrees).
   double integral(double emin, double emax, double energy, 
                   double theta, double phi) const;

protected:

   void getRef(irfInterface::Irfs * irfs);

private:

   irfInterface::IEdisp * m_edisp;

};

} // namespace rootIrfLoader

#endif // rootIrfLoader_Edisp_h

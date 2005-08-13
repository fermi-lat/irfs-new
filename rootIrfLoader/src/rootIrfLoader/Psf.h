/** 
 * @file Psf.h
 * @brief Psf class definition.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef rootIrfLoader_Psf_h
#define rootIrfLoader_Psf_h

#include <string>

#include "IrfBase.h"

namespace irfInterface {
   class IPsf;
}

namespace rootIrfLoader {

/** 
 * @class Psf
 *
 * @brief Emasculated wrapper interface for the LAT point spread function
 * classes for use with ROOT.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Psf : public IrfBase {
    
public:

   Psf(const std::string & irfs="DC1A::Front");

   Psf(const Psf & rhs);

   Psf & operator=(const Psf & rhs);

   ~Psf();

   bool operator==(const Psf & rhs) const;

   bool operator!=(const Psf & rhs) const {
      return !operator==(rhs);
   }

   /// Return the psf as a function of instrument coordinates.
   /// @param separation Angle between apparent and true photon directions
   ///        (degrees).
   /// @param energy True photon energy (MeV).
   /// @param theta True photon inclination angle (degrees).
   /// @param phi True photon azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   double operator()(double separation, double energy, double theta, 
                     double phi) const;

   /// Angular integral of the PSF in instrument coordinates.  This
   /// method is equivalent to a call to the previous version of
   /// angularIntegral for which a single AcceptanceCone is centered
   /// on the srcDir.
   double angularIntegral(double energy, double theta, double phi,
                          double radius) const;

protected:

   void getRef(irfInterface::Irfs * irfs);

private:

   irfInterface::IPsf * m_psf;

};

} // namespace rootIrfLoader

#endif // rootIrfLoader_Psf_h

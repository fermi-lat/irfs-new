/**
 * @file Psf.h
 * @brief Psf class declaration.
 * @author
 *
 * $Header$
 */

#ifndef handoff_Psf_h
#define handoff_Psf_h

#include <string>
#include <vector>

#include "irfInterface/AcceptanceCone.h"

#include "handoff_response/IrfEval.h"

#include "IPsf.h"

namespace handoff_response{

/**
 * @class Psf
 *
 * @brief 
 *
 * @author 
 *
 */

//class Psf : public irfInterface::IPsf {
class Psf : public IPsf {

public:

   Psf(handoff_response::IrfEval * eval);

   Psf(); 

   virtual ~Psf();


   /// A member function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time MET
   virtual double value(const astro::SkyDir & appDir, 
                        double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time=0) const;

   /// Return the psf as a function of instrument coordinates.
   /// @param separation Angle between apparent and true photon directions
   ///        (degrees).
   /// @param energy True photon energy (MeV).
   /// @param theta True photon inclination angle (degrees).
   /// @param phi True photon azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   /// @param time MET
   virtual double value(double separation, double energy, double theta,
                        double phi,
                        double time=0) const;

   virtual Psf * clone() {return new Psf(*this);}

protected:

   /// Disable this.
   Psf & operator=(const Psf & rhs) {
      return *this;
   }

private:

   handoff_response::IrfEval* m_eval;

};

}

#endif // handoff_Psf_h

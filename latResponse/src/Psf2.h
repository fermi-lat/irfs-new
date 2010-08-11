/**
 * @file Psf2.h
 * @brief Psf2 class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Psf2_h
#define latResponse_Psf2_h

#include <cmath>

#include <map>
#include <string>
#include <vector>

#include "Psf.h"

#include "latResponse/ParTables.h"

namespace latResponse {

class PsfIntegralCache;

/**
 * @class Psf2
 *
 * @brief Revised PSF that is the sum of two King model functions.
 * See http://confluence.slac.stanford.edu/x/bADIAw
 *
 */

class Psf2 : public Psf {

public:

   Psf2(const std::string & fitsfile, bool isFront=true,
        const std::string & extname="RPSF", size_t nrow=0);

   Psf2(const Psf2 & rhs);

   virtual ~Psf2();

   /// Return the psf as a function of instrument coordinates.
   /// @param separation Angle between apparent and true photon directions
   ///        (degrees).
   /// @param energy True photon energy (MeV).
   /// @param theta True photon inclination angle (degrees).
   /// @param phi True photon azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   /// @param time Photon arrival time (MET s)
   virtual double value(double separation, double energy, double theta,
                        double phi, double time=0) const;

   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const AcceptanceConeVector_t & acceptanceCones, 
                   double time=0);

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   virtual Psf2 * clone() {
      return new Psf2(*this);
   }

   /// Functions from handoff_response.
   static double psf_base_function(double u, double gamma);

   static double psf_base_integral(double u, double gamma);

   static double psf_integral(double sep, double * pars);

   static double psf_function(double sep, double * pars);

protected:

   /// Disable this.
   Psf2 & operator=(const Psf2 &) {
      return *this;
   }

private:

   double * pars(double energy, double costh) const;

   /**
    * @class Psf2Integrand
    * @brief Functor used for integrating the PSF to get the proper
    * normalization.
    */
   class Psf2Integrand {
   public:
      Psf2Integrand(double * pars) : m_pars(pars) {}

      /// @param sep angle between true direction and measured (radians)
      double operator()(double sep) const {
         return psf_function(sep, m_pars)*std::sin(sep);
      }
   private:
      double * m_pars;
   };

};

} // namespace latResponse

#endif // latResponse_Psf2_h

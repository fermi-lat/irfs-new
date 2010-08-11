/**
 * @file Psf3.h
 * @brief Psf3 class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Psf3_h
#define latResponse_Psf3_h

#include <cmath>

#include <map>
#include <string>
#include <vector>

#include "Psf2.h"

#include "latResponse/ParTables.h"

namespace latResponse {

class PsfIntegralCache;

/**
 * @class Psf3
 *
 * @brief Revised PSF that is the sum of two King model functions.
 * See http://confluence.slac.stanford.edu/x/bADIAw.
 * In contrast to Psf2, this class interpolates the distributions 
 * rather than the parameters.
 *
 */

class Psf3 : public Psf2 {

public:

   Psf3(const std::string & fitsfile, bool isFront=true,
        const std::string & extname="RPSF", size_t nrow=0);

   Psf3(const Psf3 & rhs);

   virtual ~Psf3();

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

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const AcceptanceConeVector_t & acceptanceCones, 
                   double time=0);
                   

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   virtual Psf3 * clone() {
      return new Psf3(*this);
   }

protected:

   /// Disable this.
   Psf3 & operator=(const Psf3 &) {
      return *this;
   }

private:

   /**
    * @class Psf3Integrand
    * @brief Functor used for integrating the PSF to get the proper
    * normalization.
    */
   class Psf3Integrand {
   public:
      Psf3Integrand(double * pars) : m_pars(pars) {}

      /// @param sep angle between true direction and measured (radians)
      double operator()(double sep) const {
         return psf_function(sep, m_pars)*std::sin(sep);
      }
   private:
      double * m_pars;
   };

   double evaluate(double energy, double sep,
                   const std::vector<double> & pars) const;

   double psf_base_integral(double energy, double radius, 
                            const std::vector<double> & pars);

   double angularIntegral(double energy, double psi, 
                          const std::vector<double> & pars);
};

} // namespace latResponse

#endif // latResponse_Psf3_h

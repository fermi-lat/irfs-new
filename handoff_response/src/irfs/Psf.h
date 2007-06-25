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
#include "irfInterface/IPsf.h"

namespace handoff_response{

   class IrfEval;

/**
 * @class Psf
 *
 * @brief 
 *
 * @author 
 *
 */

class Psf : public irfInterface::IPsf {

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


   virtual double 
   angularIntegral(double energy, double theta, 
                   double phi, double radius, double time=0) const;

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis,
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones,
                   double time=0);

   virtual double
   angularIntegral(double energy, 
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones, double time);
   
   virtual Psf * clone() {return new Psf(*this);}

protected:

   /// Disable this.
   Psf & operator=(const Psf &) {
      return *this;
   }

private:

   handoff_response::IrfEval* m_eval;

   std::vector< std::vector<double> > m_angularIntegral;
   std::vector< std::vector<bool> > m_needIntegral;
   bool m_haveAngularIntegrals;

   irfInterface::AcceptanceCone * m_acceptanceCone;

   double angularIntegral(double sigma, double gamma, size_t ipsi);
   double bilinear(double sigma, double gamma, size_t ipsi,
                   size_t isig, size_t igam) const;
   void setupAngularIntegrals();
   double psfIntegral(double psi, double sigma, double gamma);

   static double s_gamma;
   static double s_sigma;

   static double s_cp;
   static double s_sp;
   static double s_cr;

   static double psfIntegrand1(double * mu);
   static double psfIntegrand2(double * mu);

};

}

#endif // handoff_Psf_h

/**
 * @file Psf.h
 * @brief Psf class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Psf_h
#define latResponse_Psf_h

#include <map>
#include <string>
#include <vector>

#include "irfInterface/IPsf.h"
#include "irfInterface/AcceptanceCone.h"

#include "latResponse/FitsTable.h"

namespace latResponse {

/**
 * @class Psf
 *
 * @brief A LAT point-spread function class for DC2.
 *
 */

class Psf : public irfInterface::IPsf {

public:

   Psf(const std::string & fitsfile, const std::string & extname="RPSF",
       bool isFront=true);

   Psf(const Psf & rhs);

   virtual ~Psf();

   /// A member function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
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
   /// @param time Photon arrival time (MET s)
   virtual double value(double separation, double energy, double theta,
                        double phi, double time=0) const;

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
                   & acceptanceCones, double time=0) {}

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const {}

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir & srcDir,
                                const astro::SkyDir & scZAxis,
                                const astro::SkyDir & scXAxis,
                                double time=0) const {}

   virtual Psf * clone() {
      return new Psf(*this);
   }

protected:

   /// Disable this.
   Psf & operator=(const Psf &) {
      return *this;
   }

private:

   std::vector<std::string> m_parNames;
   std::map<std::string, FitsTable> m_pars;

   // PSF scaling parameters
   double m_thin0;
   double m_thin1;
   double m_thick0;
   double m_thick1;
   double m_index;

   double m_loge_last;
   double m_costh_last;

   // It would be good to get rid of this abomination passed on by
   // handoff_response::IrfEval.
   bool m_isFront;

   bool m_haveAngularIntegrals;

   irfInterface::AcceptanceCone * m_acceptanceCone;

   std::vector< std::vector<bool> > m_needIntegral;

   const FitsTable & parTable(const std::string & name) const;

   double scaleFactor(double energy, bool thin) const;

   void readPars(const std::string & fitsfile,
                 const std::string & extname="RPSF");

   void readScaling(const std::string & fitsfile,
                    const std::string & extname="PSF_SCALING_PARAMS");

   double old_integral(double sep, double * pars) const;

   double old_base_integral(double u, double sigma, double gamma) const;

   double old_function(double sep, double * pars) const;

   double old_base_function(double u, double sigma, double gamma) const;

   double * pars(double energy, double costh) const;

};

} // namespace latResponse

#endif // latResponse_Psf_h

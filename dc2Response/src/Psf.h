/**
 * @file Psf.h
 * @brief Psf class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_Psf_h
#define dc2Response_Psf_h

#include <string>
#include <vector>

#include "irfInterface/IPsf.h"
#include "irfInterface/AcceptanceCone.h"

#include "DC2.h"

namespace dc2Response {

   class PsfScaling;

/**
 * @class Psf
 *
 * @brief A LAT point-spread function class for DC2.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Psf : public irfInterface::IPsf, public DC2 {

public:

   Psf(const std::string & fitsfile, const std::string & extname);

   virtual ~Psf();

   Psf(const Psf & rhs);

   /// A member function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   virtual double value(const astro::SkyDir & appDir, 
                        double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis) const;

   /// Return the psf as a function of instrument coordinates.
   /// @param separation Angle between apparent and true photon directions
   ///        (degrees).
   /// @param energy True photon energy (MeV).
   /// @param theta True photon inclination angle (degrees).
   /// @param phi True photon azimuthal angle measured wrt the instrument
   ///            X-axis (degrees).
   virtual double value(double separation, double energy, double theta,
                        double phi) const;

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis,
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones);

   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const std::vector<irfInterface::AcceptanceCone *> 
                   & acceptanceCones);

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius) const;

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir & srcDir,
                                const astro::SkyDir & scZAxis,
                                const astro::SkyDir & scXAxis) const;

   virtual Psf * clone() {
      return new Psf(*this);
   }

   double drawOffset(double energy, double mu) const;

   double drawScaledDev(double gamma) const;

   const std::vector<double> & lowerFracs() const {
      return s_lowerFractions;
   }

protected:

   /// Disable this.
   Psf & operator=(const Psf & rhs) {
      return *this;
   }

private:

   PsfScaling * m_psfScaling;

   std::vector<double> m_sigma;
   std::vector<double> m_gamma;

   std::vector<double> m_logElo;
   std::vector<double> m_logEhi;
   std::vector<double> m_logE;
   std::vector<double> m_cosinc;

   static std::vector<double> s_psi;
   std::vector<double> m_angScale;
   std::vector<double> m_gamValues;

   std::vector< std::vector<double> > m_angularIntegral;
   std::vector< std::vector<bool> > m_needIntegral;
   bool m_haveAngularIntegrals;

   irfInterface::AcceptanceCone * m_acceptanceCone;

   double psfIntegral(double psi, double angScale, double gamValue,
                      double roi_radius=0);

   double value(double separation, double angScale, double gam) const;

   double bilinear(double angScale, double gamValue, size_t ipsi,
                   size_t iang, size_t igam) const;

   void readData();

   double gamma(double energy, double mu) const;
   double sigma(double energy, double mu) const;

   double angularScale(double energy, double mu) const;

   void computeAngularIntegrals
   (const std::vector<irfInterface::AcceptanceCone *> & cones);

/// @bug This code is not thread-safe...will need to find an
/// integrator that does not require a static function as its
/// argument.
   static double s_energy;
   static double s_theta;
   static double s_phi;
   static const Psf * s_self;
   static double coneIntegrand(double * offset);

   static std::vector<double> s_gammas;
   static std::vector<double> s_psfNorms;
   static std::vector<double> s_lowerFractions;

   static double psfIntegrand(double * xx);
   void computePsfNorms();
   void computeLowerFractions();

   /// Nested class that returns the integrand for the
   /// m_angularIntegrals
   class Gint {
   public:
      Gint() : m_psfObj(0) {}
      Gint(Psf * psfObj, double angScale, double gamma) :
         m_psfObj(psfObj), m_angScale(angScale), m_gamma(gamma),
         m_doFirstTerm(true), m_cp(0), m_sp(0), m_cr(0) {}
      Gint(Psf * psfObj, double angScale, double gamma,
           double cp, double sp, double cr) : 
         m_psfObj(psfObj), m_angScale(angScale), m_gamma(gamma),
         m_doFirstTerm(false), m_cp(cp), m_sp(sp), m_cr(cr) {}
      virtual ~Gint() {}
      double value(double mu) const;
   private:
      Psf * m_psfObj;
      double m_angScale;
      double m_gamma;
      bool m_doFirstTerm;
      double m_cp;
      double m_sp;
      double m_cr;
   };

   /// A static object needed to compute the m_angularIntegrals using
   /// the DGAUS8 integrator
   static Gint s_gfunc;

   /// A static member function to provide the interface to s_gfunc
   /// that is required by DGAUS8
   static double gfuncIntegrand(double *mu) {
      return s_gfunc.value(*mu);
   }

   friend class Psf::Gint;
};

} // namespace dc2Response

#endif // dc2Response_Psf_h

/**
 * @file Psf.h
 * @brief Psf class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef testResponse_Psf_h
#define testResponse_Psf_h

#include <string>
#include <vector>

#include "irfInterface/IPsf.h"
#include "irfInterface/AcceptanceCone.h"

namespace testResponse {

/**
 * @class Psf
 *
 * @brief An "idealized" LAT point-spread function class.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Psf : public irfInterface::IPsf {

public:

   Psf(const std::vector<double> & psfParams);

   virtual ~Psf();

   Psf(const Psf &rhs);

   /// A member function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   virtual double value(const astro::SkyDir &appDir, 
                        double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double separation, double energy, double theta,
                        double phi) const;

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir &srcDir,
                   const astro::SkyDir &scZAxis,
                   const astro::SkyDir &scXAxis,
                   const std::vector<irfInterface::AcceptanceCone *> 
                   &acceptanceCones);

   virtual double 
   angularIntegral(double energy, const astro::SkyDir &srcDir,
                   double theta, double phi, 
                   const std::vector<irfInterface::AcceptanceCone *> 
                   &acceptanceCones);

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius) const;

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir &srcDir,
                                const astro::SkyDir &scZAxis,
                                const astro::SkyDir &scXAxis) const;

   virtual Psf * clone() {return new Psf(*this);}

private:

   /// Prevent compiler-generated version.
   Psf & operator=(const Psf &);

   /// @param separation Angle between true/source and apparent photon 
   ///        directions in *radians*
   /// @param energy True photon energy (MeV)
   /// @param inclination Angle between source direction and spacecraft
   ///        z-axis (radians)
   double value(double separation, double energy, double inclination) const;

   /// @param separation Angle between true/source and apparent photon 
   ///        directions (radians)
   /// @param sepMean Mean separation for the Psf (radians).
   double value(double separation, double sepMean) const;

   /// @return The mean separation for a lognormal distribution of 
   ///         the psf.
   /// @param energy True energy of the photon (MeV)
   /// @param inclination Angle between source direction and spacecraft 
   ///        z-axis (radians)
   double sepMean(double energy, double inclination) const;

   void computeAngularIntegrals
   (const std::vector<irfInterface::AcceptanceCone *> & cones);

   double psfIntegral(double psi, double sepMean, double roi_radius=0);

   std::vector<double> m_psfParams;

   irfInterface::AcceptanceCone * m_acceptanceCone;

   double m_psfNorm;
   std::vector<double> m_scaledDevs;
   std::vector<double> m_cumDist;

   void computeCumulativeDist();
   double scaledDist(double scaledDev) const;
   double drawScaledDev() const;

   std::vector<double> m_psi;
   std::vector<double> m_sepMean;
   std::vector<double> m_angularIntegral;
   std::vector<bool> m_needIntegral;
   bool m_haveAngularIntegrals;

   /// Nested class that returns the integrand for the
   /// m_angularIntegrals
   class Gint {
   public:
      Gint() : m_psfObj(0) {}
      Gint(Psf * psfObj, double sepMean) :
         m_psfObj(psfObj), m_sepMean(sepMean), 
         m_doFirstTerm(true), m_cp(0), m_sp(0), m_cr(0) {}
      Gint(Psf * psfObj, double sepMean, 
           double cp, double sp, double cr) : 
         m_psfObj(psfObj), m_sepMean(sepMean),
         m_doFirstTerm(false), m_cp(cp), m_sp(sp), m_cr(cr) {}
      virtual ~Gint() {}
      double value(double mu) const;
   private:
      Psf * m_psfObj;
      double m_sepMean;
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

} // namespace testResponse

#endif // testResponse_Psf_h

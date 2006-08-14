/**
 * @file PsfDC1.h
 * @brief PsfDC1 class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1Response_PsfDC1_h
#define dc1Response_PsfDC1_h

#include <string>
#include <valarray>
#include <vector>

#include "irfInterface/IPsf.h"
#include "irfInterface/AcceptanceCone.h"

#include "DC1.h"

namespace dc1Response {

/**
 * @class PsfDC1
 *
 * @brief A LAT point-spread function class using the DC1 AllGamma data.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class PsfDC1 : public irfInterface::IPsf, public DC1 {

public:

   PsfDC1(const std::string &filename);

   PsfDC1(const std::string &filename, int hdu, int npars);

   virtual ~PsfDC1();

   PsfDC1(const PsfDC1 &rhs);

   /// A member function returning the point-spread function value.
   /// @param appDir Apparent (reconstructed) photon direction.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time Photon arrival time (MET s)
   virtual double value(const astro::SkyDir &appDir, 
                        double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const;

   virtual double value(double separation, double energy, double theta,
                        double phi, double time=0) const;

   /// Angular integral of the PSF over the intersection of acceptance
   /// cones.
   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir &srcDir,
                   const astro::SkyDir &scZAxis,
                   const astro::SkyDir &scXAxis,
                   const std::vector<irfInterface::AcceptanceCone *> 
                   &acceptanceCones, 
                   double time=0);

   virtual double 
   angularIntegral(double energy, const astro::SkyDir &srcDir,
                   double theta, double phi, 
                   const std::vector<irfInterface::AcceptanceCone *> 
                   &acceptanceCones,
                   double time=0);

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir &srcDir,
                                const astro::SkyDir &scZAxis,
                                const astro::SkyDir &scXAxis,
                                double time=0) const;

   virtual PsfDC1 * clone() {return new PsfDC1(*this);}

private:

   /// Prevent compiler-generated version.
   PsfDC1 & operator=(const PsfDC1 &);

   /// @param separation Angle between true/source and apparent photon 
   ///        directions in *radians*
   /// @param energy True photon energy (MeV)
   /// @param inclination Angle between source direction and spacecraft
   ///        z-axis (degrees)
   double value(double separation, double energy, double inclination) const;

   /// This implements the Perugia parameterization of the PSF
   /// distributions.
   double value(double scaledAngle, std::vector<double> &pars) const;

   void readEnergyScaling();
   double energyScaling(double energy) const;

   /// Compute cumulative distributions for each set of the PSF
   /// parameters and ensure that the normalization is correct for
   /// each.
   void computeCumulativeDists();
   std::vector<double> m_scaledAngles;
   std::vector< std::vector<double> > m_cumulativeDists;

   /// Rescale the fit parameters for Claudia's implementation to
   /// ensure proper normalization.  This is called from
   /// computeCumulativeDists().
   void rescaleParams(std::vector<double> &pars, double scaleFactor);
   
   std::vector<double> m_scaleEnergy;
   std::vector<double> m_scaleFactor;

   bool m_haveAngularIntegrals;

   /// Variables for subdividing the energy intervals used by the
   /// m_pars grid.  These are necessary in order to apply the energy
   /// scaling with sufficient resolution for the angular integrals.
   int m_nesteps;
   double m_logestep;

   irfInterface::AcceptanceCone * m_acceptanceCone;

   std::vector<double> m_psi;

   /// The dimensions of this vector of valarrays will be
   /// m_pars.size() x (m_psi.size() x m_nesteps)
   std::vector< std::valarray<double> > m_angularIntegrals;

   std::vector< std::vector<bool> > m_needIntegral;

   void 
   computeAngularIntegrals(const std::vector<irfInterface::AcceptanceCone*> &);

   void performIntegral(int ipar, int ipsi, int jen);

   /// Nested class that returns the integrand for the
   /// m_angularIntegrals
   class Gint {
   public:
      Gint() {}
      Gint(PsfDC1 *psfObj, int ipar, double energy) :
         m_psfObj(psfObj), m_ipar(ipar), m_energy(energy),
         m_doFirstTerm(true), m_cp(0), m_sp(0), m_cr(0) {}
      Gint(PsfDC1 *psfObj, int ipar, double energy, 
           double cp, double sp, double cr) : 
         m_psfObj(psfObj), m_ipar(ipar), m_energy(energy), 
         m_doFirstTerm(false), m_cp(cp), m_sp(sp), m_cr(cr) {}
      virtual ~Gint() {}
      double value(double mu) const;
   private:
      PsfDC1 *m_psfObj;
      int m_ipar;
      double m_energy;
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

   friend class PsfDC1::Gint;

};

} // namespace dc1Response

#endif // dc1Response_PsfDC1_h

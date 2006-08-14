/**
 * @file PsfGlast25.h
 * @brief PsfGlast25 class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_PsfGlast25_h
#define g25Response_PsfGlast25_h

#include <string>
#include <vector>

#include "irfInterface/IPsf.h"
#include "irfInterface/AcceptanceCone.h"

#include "Glast25.h"

namespace g25Response {

/**
 * @class PsfGlast25
 *
 * @brief A LAT point-spread function class using the GLAST25 data.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class PsfGlast25 : public irfInterface::IPsf, public Glast25 {

public:

   PsfGlast25(const std::string &filename, int hdu)
      : Glast25(filename, hdu), m_haveAngularIntegrals(false), 
        m_acceptanceCone(0) {readPsfData();}

   virtual ~PsfGlast25() {delete m_acceptanceCone;}

   PsfGlast25(const PsfGlast25 &rhs);

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

   virtual double value(double separation, double energy,
                        double theta, double phi, double time=0) const;

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
                   &acceptanceCones, double time=0);

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   virtual astro::SkyDir appDir(double energy,
                                const astro::SkyDir &srcDir,
                                const astro::SkyDir &scZAxis,
                                const astro::SkyDir &scXAxis, 
                                double time=0) const;

   virtual PsfGlast25 * clone() {return new PsfGlast25(*this);}

private:

   /// Prevent compiler-generated version.
   PsfGlast25 & operator=(const PsfGlast25 &);
   
   double value(double separation, double energy, double inclination) const;

   void readPsfData();

   /// Retrieve PSF parameters (sig1, sig2, wt) in instrument
   /// coordinates
   void fetchPsfParams(double energy, double inclination,
                       std::vector<double> &psf_params) const;

   std::vector<double> m_energy;

   std::vector<double> m_theta;

   std::vector<double> m_wt;

   std::vector<double> m_sig1, m_sig2;

   bool m_haveAngularIntegrals;

   irfInterface::AcceptanceCone *m_acceptanceCone;

   std::vector<double> m_psi;
   std::vector<double> m_sigma;
   std::vector<double> m_angularIntegrals;

   void computeAngularIntegrals(const std::vector<irfInterface::AcceptanceCone *> &);

   /// Nested class that returns the integrand for the m_angularIntegrals
   class Gint {
   public:
      Gint() {};
      Gint(double sig, double cr, double cp, double sp) : 
           m_sig(sig), m_cr(cr), m_cp(cp), m_sp(sp) {}
      virtual ~Gint(){};
      double value(double mu) const;
   private:
      double m_sig;
      double m_cr;
      double m_cp;
      double m_sp;
   };

   /// A static object needed to compute the m_angularIntegrals using
   /// the DGAUS8 integrator
   static Gint s_gfunc;

   /// A static member function to provide the interface to s_gfunc
   /// that is required by DGAUS8
   static double gfuncIntegrand(double *mu) {
      return s_gfunc.value(*mu);
   }

};

} // namespace g25Response

#endif // g25Response_PsfGlast25_h


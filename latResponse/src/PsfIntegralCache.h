/**
 * @file PsfIntegralCache.h
 * @brief Class to cache angular integrals of the Psf over regions of
 * interest as a function of source offset angle, and the psf
 * parameters gamma and sigma.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_PsfIntegralCache_h
#define latResponse_PsfIntegralCache_h

#include <ctime>

#include <vector>

namespace irfInterface {
   class AcceptanceCone;
}

namespace latResponse {

class PsfBase;

/**
 * @class PsfIntegralCache
 * @brief Class to cache angular integrals of the Psf over regions of
 * interest as a function of source offset angle, and the psf
 * parameters gamma and sigma.
 *
 */

class PsfIntegralCache {

public:

   PsfIntegralCache(const PsfBase & psf, irfInterface::AcceptanceCone & cone);

   ~PsfIntegralCache();

   double angularIntegral(double sigma, double gamma, size_t psi) const;

   const std::vector<double> & psis() const {
      return m_psis;
   }

   const irfInterface::AcceptanceCone & acceptanceCone() const {
      return * m_acceptanceCone;
   }

private:

   const PsfBase & m_psf;

   irfInterface::AcceptanceCone * m_acceptanceCone;
   
   std::vector<double> m_psis;
   std::vector<double> m_gammas;
   std::vector<double> m_sigmas;

   mutable std::vector< std::vector<bool> > m_needIntegral;
   mutable std::vector< std::vector<double> > m_angularIntegral;

   mutable int m_calls;
   mutable int m_interpolations;

   mutable double m_cpuTotal;

   mutable double m_gamma_avg;
   mutable double m_sigma_avg;
   mutable double m_integralEvals;

   mutable double m_gamma_max;
   mutable double m_gamma_min;

   mutable double m_sigma_max;
   mutable double m_sigma_min;

   void linearArray(double xmin, double xmax, size_t nx,
                    std::vector<double> & xx, bool clear=true) const;

   void logArray(double xmin, double xmax, size_t nx,
                 std::vector<double> & xx, bool clear=true) const;

   void fillParamArrays();
   void setupAngularIntegrals();

   double bilinear(double sigma, double gamma, size_t ipsi,
                   size_t isig, size_t igam) const;

   double psfIntegral(double psi, double sigma, double gamma) const;

   class PsfIntegrand1 {
   public:
      PsfIntegrand1(double sigma, double gamma);
      double operator()(double mu) const;
   private:
      double m_sigma;
      double m_gamma;
   };

   class PsfIntegrand2 {
   public:
      PsfIntegrand2(double sigma, double gamma, double psi, double roi_radius);
      double operator()(double mu) const;
   private:
      double m_sigma;
      double m_gamma;
      double m_cp;
      double m_sp;
      double m_cr;
   };

};

} // namespace latResponse

#endif // latResponse_PsfIntegralCache_h

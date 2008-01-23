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

//namespace latResponse {

class Psf;

/**
 * @class PsfIntegralCache
 * @brief Class to cache angular integrals of the Psf over regions of
 * interest as a function of source offset angle, and the psf
 * parameters gamma and sigma.
 *
 */

class PsfIntegralCache {

public:

   PsfIntegralCache(const Psf & psf);

   double angularIntegral(double sigma, double gamma, double psi);

private:

   const Psf & m_psf;

   std::vector<double> m_psis;
   std::vector<double> m_gammas;
   std::vector<double> m_sigmas;

   mutable std::vector< std::vector<bool> > m_needIntegral;
   mutable std::vector< std::vector<double> > m_angularIntegral;

   void linearArray(double xmin, double xmax, size_t nx,
                    std::vector<double> & xx, bool clear=true) const;

   void logArray(double xmin, double xmax, size_t nx,
                 std::vector<double> & xx, bool clear=true) const;

   void fillParamArrays();

   double psfIntegral(double psi, double sigma, double gamma) const;

   class PsfIntegrand1 {
   public:
      PsfIntegrand(double sigma, double gamma);
      double operator()(double mu) const;
   private:
      double m_sigma;
      double m_gamma;
   };

   class PsfIntegrand2 {
   public:
      PsfIntegrand2(double sigma, double gamma);
      double operator()(double mu) const;
   private:
      double m_sigma;
      double m_gamma;
   };

};

} // namespace latResponse

#end // latResponse_PsfIntegralCache_h

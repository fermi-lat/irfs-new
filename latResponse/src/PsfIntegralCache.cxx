/**
 * @file PsfIntegralCache.cxx
 * @brief Implementation for a class to cache angular integrals of the
 * Psf over regions of interest as a function of source offset angle,
 * and the psf parameters gamma and sigma.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include "Psf.h"
#include "PsfIntegralCache.h"

//namespace latResponse {

PsfIntegralCache::PsfIntegralCache(const Psf & psf) : m_psf(psf) {
   fillParamArrays();
}

void PsfIntegralCache::fillParamArrays() {
   size_t npsi(500);
   double psimin(0);
   double psimax(60*M_PI/180.);
   linearArray(psimin, psimax, npsi, m_psis);

   size_t ngam(50);
// These upper and lower values mirror the parameter fit boundaries in
// PointSpreadFunction.cxx, which are also inside anonymous namespace
// and so are inaccessible outside of that file.
   linearArray(1, 1.2, ngam, m_gammas);
   linearArray(1.2, 5.1, ngam, m_gammas, false);

   size_t nsig(50);
   double zdir;
   bool front;
// Smallest angular scales expected at highest energies, also use
// hard-wired values for scaled sigma range in PointSpreadFunction.cxx
   double sigmin(m_psf.scaleFactor(5.62e6, zdir=1, front=true)*0.15);
   double sigmax(m_psf.scaleFactor(30, zdir, front=false)*2.0);
   logArray(sigmin, sigmax, nsig, m_sigmas);
}

void PsfIntegralCache::linearArray(double xmin, double xmax, size_t nx, 
                                   std::vector<double> & xx, bool clear=true) {
   if (clear) {
      xx.clear();
   }
   double xstep((xmax - xmin)/(nx-1));
   for (size_t i(0); i < nx; i++) {
      xx.push_back(xstep*i + xmin);
   }
}

void PsfIntegralCache::logArray(double xmin, double xmax, size_t nx, 
                                std::vector<double> & xx, bool clear=true) {
   if (clear) {
      xx.clear();
   }
   double xstep(std::log(xmax/xmin)/(nx-1));
   for (size_t i(0); i < nx; i++) {
      xx.push_back(xmin*std::exp(xstep*i));
   }
}

double PsfIntegralCache::
psfIntegral(double psi, double sigma, double gamma) const {

}

} // namespace latResponse

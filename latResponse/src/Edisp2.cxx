/** 
* @file Edisp2.cxx
* @brief Implementation for post-handoff review energy dispersion class.
* @author J. Chiang
*
* $Header$
*/

#include <cmath>

#include <algorithm>

#include "astro/SkyDir.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/GaussianQuadrature.h"

#include "latResponse/FitsTable.h"

#include "Edisp2.h"

namespace latResponse {

Edisp2::Edisp2(const std::string & fitsfile, 
               const std::string & extname, size_t nrow) 
   : m_parTables(fitsfile, extname, nrow), m_loge_last(0), m_costh_last(0) {
   readScaling(fitsfile);
}

double Edisp2::value(double appEnergy,
                     double energy, 
                     const astro::SkyDir & srcDir,
                     const astro::SkyDir & scZAxis,
                     const astro::SkyDir & scXAxis, 
                     double time) const {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(appEnergy, energy, theta, phi, time);
}

double Edisp2::value(double appEnergy, double energy,
                     double theta, double phi, double time) const {
   (void)(phi);
   (void)(time);
   double costh(std::cos(theta*M_PI/180.));
   double xx((appEnergy - energy)/energy);
   xx /= scaleFactor(std::log10(energy), costh);

   double * my_pars(pars(energy, costh));
   
   return old_function(xx, my_pars)/energy;
}

double Edisp2::old_function(double xx, double * pars) const {
// See ::edisp_func in handoff_response/src/gen/Dispersion.cxx
   double tt(std::fabs(xx - pars[3]));
   double s1(pars[1]);
   double s2(pars[4]);
   if (xx > pars[3]) {
      s1 = pars[2];
      s2 = pars[5];
   }
   double g1(std::exp(-0.5*std::pow(tt/s1, m_p1)));
   double g2(std::exp(-0.5*std::pow(tt/s2, m_p2)));
   double nscale(std::exp(0.5*(std::pow(m_t0/s2, m_p2) - 
                               std::pow(m_t0/s1, m_p1))));

   if (tt > m_t0) {
      return pars[0]*nscale*g2;
   }
   return pars[0]*g1;
}

double Edisp2::scaleFactor(double logE, double costh) const {
// See handoff_response::Dispersion::scaleFactor
   costh = std::fabs(costh);
   double my_value(m_scalePars.at(0)*logE*logE +
                   m_scalePars.at(1)*costh*costh + 
                   m_scalePars.at(2)*logE + 
                   m_scalePars.at(3)*costh +
                   m_scalePars.at(4)*logE*costh +
                   m_scalePars.at(5));
   return my_value;
}

double * Edisp2::pars(double energy, double costh) const {
   double loge(std::log10(energy));
   if (costh == 1.0) {
      costh = 0.9999;   // restriction from handoff_response::RootEval
   }

   if (loge == m_loge_last && costh == m_costh_last) {
      return m_pars;
   }
   
   m_loge_last = loge;
   m_costh_last = costh;
   
   bool interpolate;
   m_parTables.getPars(loge, costh, m_pars, interpolate=false);

   // Ensure proper normalization
   EdispIntegrand foo(m_pars, energy, scaleFactor(loge, costh), *this);
   double err(1e-5);
   int ierr;
   double norm = 
      st_facilities::GaussianQuadrature::dgaus8(foo, energy/10.,
                                                energy*10., err, ierr);
   m_pars[0] /= norm;

   return m_pars;
}

void Edisp2::readScaling(const std::string & fitsfile, 
                         const std::string & extname) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));

   std::vector<float> values;

   FitsTable::getVectorData(table, "EDISPSCALE", values);

   size_t npars(values.size() - 3);
   m_scalePars.resize(npars);
   std::copy(values.begin(), values.begin() + npars, m_scalePars.begin());

   m_p1 = values.at(npars);
   m_p2 = values.at(npars + 1);
   m_t0 = values.at(npars + 2);

   delete table;
}

} // namespace latResponse

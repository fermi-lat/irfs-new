/**
 * @file Psf3b.cxx
 * @brief PSF as the sum of two King model functions.
 * See http://confluence.slac.stanford.edu/x/bADIAw.
 * In contrast to Psf2, this class interpolates the distributions 
 * rather than the parameters.
 *
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "latResponse/Bilinear.h"
#include "latResponse/FitsTable.h"

#include "Psf2.h"
#include "Psf3b.h"
#include "PsfIntegralCache.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

namespace latResponse {

Psf3b::Psf3b(const std::string & fitsfile, bool isFront,
             const std::string & extname, size_t nrow) 
   : PsfBase(fitsfile, isFront, extname), m_integralCache(0) {
   readFits(fitsfile, extname, nrow);
   normalize_pars();
}

Psf3b::Psf3b(const Psf3b & rhs) : PsfBase(rhs),
                                  m_logE_bounds(rhs.m_logE_bounds),
                                  m_energies(rhs.m_energies),
                                  m_costh_bounds(rhs.m_costh_bounds),
                                  m_thetas(rhs.m_thetas),
                                  m_parVectors(rhs.m_parVectors),
                                  m_integralCache(0) {}

Psf3b::~Psf3b() {
   delete m_integralCache;
}

double Psf3b::value(const astro::SkyDir & appDir, 
                    double energy, 
                    const astro::SkyDir & srcDir, 
                    const astro::SkyDir & scZAxis,
                    const astro::SkyDir & scXAxis, 
                    double time) const {
   (void)(scXAxis);
   double sep(appDir.difference(srcDir)*180./M_PI);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(sep, energy, theta, phi, time);
}

double Psf3b::value(double separation, double energy, double theta,
                   double phi, double time) const {
   (void)(phi);
   (void)(time);

   double tt, uu;
   std::vector<double> cornerEnergies(4);
   std::vector<size_t> indx(4);
   getCornerPars(energy, theta, tt, uu, cornerEnergies, indx);

   double sep(separation*M_PI/180.);
   std::vector<double> yvals(4);
   for (size_t i(0); i < 4; i++) {
      yvals[i] = evaluate(cornerEnergies[i], sep, &m_parVectors[indx[i]][0]);
   }

   double my_value = Bilinear::evaluate(tt, uu, &yvals[0]);
   return my_value;
}

double Psf3b::angularIntegral(double energy, double theta, 
                              double phi, double radius, double time) const {
   if (energy < 120.) {
      double value = IPsf::angularIntegral(energy, theta, phi, radius, time);
      return value;
   }

   double tt, uu;
   std::vector<double> cornerEnergies(4);
   std::vector<size_t> indx(4);
   getCornerPars(energy, theta, tt, uu, cornerEnergies, indx);
   
   std::vector<double> yvals(4);
   for (size_t i(0); i < 4; i++) {
      yvals[i] = psf_base_integral(cornerEnergies[i], radius,
                                   &m_parVectors[indx[i]][0]);
   }
   double value = Bilinear::evaluate(tt, uu, &yvals[0]);
   return value;
}

double Psf3b::psf_base_integral(double energy, double radius, 
                                const double * pars) const {
   double ncore(pars[0]);
   double ntail(pars[1]);
   double score(pars[2]*scaleFactor(energy));
   double stail(pars[3]*scaleFactor(energy));
   double gcore(pars[4]);
   double gtail(pars[5]);

   double sep = radius*M_PI/180.;
   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*Psf2::psf_base_integral(uc, gcore)*2.*M_PI*::sqr(score) + 
           ntail*ncore*Psf2::psf_base_integral(ut, gtail)*2.*M_PI*::sqr(stail));
}

double Psf3b::angularIntegral(double energy,
                             const astro::SkyDir & srcDir,
                             double theta, 
                             double phi, 
                             const AcceptanceConeVector_t & acceptanceCones, 
                             double time) {
   (void)(phi);
   (void)(time);

   irfInterface::AcceptanceCone & cone(*acceptanceCones.at(0));
   if (!m_integralCache || cone != m_integralCache->acceptanceCone()) {
      delete m_integralCache;
      m_integralCache = new PsfIntegralCache(*this, cone);
   }

   double tt, uu;
   std::vector<double> cornerEnergies(4);
   std::vector<size_t> indx(4);
   getCornerPars(energy, theta, tt, uu, cornerEnergies, indx);

   double psi(srcDir.difference(cone.center()));

   std::vector<double> yvals(4);
   for (size_t i(0); i < 4; i++) {
      yvals[i] = angularIntegral(cornerEnergies[i], psi,
                                 m_parVectors[indx[i]]);
   }
   double value(Bilinear::evaluate(tt, uu, &yvals[0]));

   return value;
}

double Psf3b::angularIntegral(double energy, double psi, 
                              const std::vector<double> & pars) {
   const std::vector<double> & psis(m_integralCache->psis());
   if (psi > psis.back()) {
      std::ostringstream message;
      message << "latResponse::Psf3b::angularIntegral:\n"
              << "Error evaluating PSF integral.\n"
              << "Requested source location > " 
              << psis.back()*180/M_PI 
              << " degrees from ROI center.";
      throw std::runtime_error(message.str());
   }
   size_t ii(std::upper_bound(psis.begin(), psis.end(), psi) 
             - psis.begin() - 1);

   double ncore(pars[0]);
   double ntail(pars[1]);
   double score(pars[2]*scaleFactor(energy));
   double stail(pars[3]*scaleFactor(energy));
   double gcore(pars[4]);
   double gtail(pars[5]);

   /// Remove sigma**2 scaling imposed by pars(...).  This is put back
   /// in angularIntegral below for each grid value of sigmas.  This
   /// preserves the normalization in the bilinear interpolation by
   /// explicitly putting in the important sigma-dependence.
   double norm_core(ncore*score*score);
   double norm_tail(ntail*stail*stail);

   double y1 = 
      norm_core*m_integralCache->angularIntegral(score, gcore, ii) + 
      norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii);
   double y2 =
      norm_core*m_integralCache->angularIntegral(score, gcore, ii+1) + 
      norm_tail*ncore*m_integralCache->angularIntegral(stail, gtail, ii+1);

   double y = ((psi - psis[ii])/(psis[ii+1] - psis[ii])*(y2 - y1)) + y1;
   return y;
}

double Psf3b::evaluate(double energy, double sep, const double * pars) const {
   double ncore(pars[0]);
   double ntail(pars[1]);
   double score(pars[2]*scaleFactor(energy));
   double stail(pars[3]*scaleFactor(energy));
   double gcore(pars[4]);
   double gtail(pars[5]);

   double rc = sep/score;
   double uc = rc*rc/2.;

   double rt = sep/stail;
   double ut = rt*rt/2.;
   return (ncore*Psf2::psf_base_function(uc, gcore) +
           ntail*ncore*Psf2::psf_base_function(ut, gtail));
}

void Psf3b::readFits(const std::string & fitsfile,
                     const std::string & extname, 
                     size_t nrow) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));
   const std::vector<std::string> & validFields(table->getValidFields());

   // The first four columns *must* be "ENERG_LO", "ENERG_HI", "CTHETA_LO",
   // "CTHETA_HI", in that order.
   const char * boundsName[] = {"energ_lo", "energ_hi", 
                                "ctheta_lo", "ctheta_hi"};
   for (size_t i(0); i < 4; i++) {
      if (validFields.at(i) != boundsName[i]) {
         std::ostringstream message;
         message << "latResponse::ParTables::ParTables: "
                 << "invalid header in " << fitsfile << "  "
                 << validFields.at(i) << "  " << i;
         throw std::runtime_error(message.str());
      }
   }

   std::vector<double> elo, ehi;
   FitsTable::getVectorData(table, "ENERG_LO", elo, nrow);
   FitsTable::getVectorData(table, "ENERG_HI", ehi, nrow);
   for (size_t k(0); k < elo.size(); k++) {
      m_logE_bounds.push_back(std::log10(elo.at(k)));
      m_energies.push_back(std::sqrt(elo[k]*ehi[k]));
   }
   m_logE_bounds.push_back(std::log10(ehi.back()));

   std::vector<double> mulo, muhi;
   FitsTable::getVectorData(table, "CTHETA_LO", mulo, nrow);
   FitsTable::getVectorData(table, "CTHETA_HI", muhi, nrow);
   for (size_t i(0); i < muhi.size(); i++) {
      m_costh_bounds.push_back(mulo.at(i));
      m_thetas.push_back(std::acos((mulo[i] + muhi[i])/2.)*180./M_PI);
   }
   m_costh_bounds.push_back(muhi.back());

   size_t par_size(elo.size()*mulo.size());
   m_parVectors.resize(par_size, std::vector<double>());

   std::vector<double> values;
   for (size_t i(4); i < validFields.size(); i++) {
      const std::string & tablename(validFields[i]);
      FitsTable::getVectorData(table, tablename, values, nrow);
      if (values.size() != par_size) {
         std::ostringstream message;
         message << "Parameter array size does not match "
                 << "expected size based on energy and costheta arrays "
                 << "for table " << tablename
                 << " in  " << fitsfile;
         throw std::runtime_error(message.str());
      }
      for (size_t j(0); j < par_size; j++) {
         m_parVectors[j].push_back(values[j]);
      }
   }
   if (m_parVectors[0].size() != 6) {
      std::ostringstream message;
      message << "Number of PSF parameters in "
              << fitsfile
              << " does no match the expected number of 6.";
      throw std::runtime_error(message.str());
   }
   delete table;
}

void Psf3b::normalize_pars(double radius) {
   double phi(0);
   double time(0);
   size_t indx(0);
   for (size_t j(0); j < m_thetas.size(); j++) {
      for (size_t k(0); k < m_energies.size(); k++, indx++) {
         double energy(m_energies[k]);
         double norm;
         if (energy < 120.) {
            norm = IPsf::angularIntegral(energy, m_thetas[j], phi, 
                                         radius, time);
         } else {
            norm = psf_base_integral(energy, radius, &m_parVectors[indx][0]);
         }
         m_parVectors[indx][0] /= norm;
      }
   }
}

void Psf3b::getCornerPars(double energy, double theta,
                          double & tt, double & uu,
                          std::vector<double> & cornerEnergies,
                          std::vector<size_t> & indx) const {
   double logE(std::log10(energy));
   double costh(std::cos(theta*180./M_PI));
   int i(findIndex(m_logEs, energy));
   int j(findIndex(m_cosths, theta));

   tt = (logE - m_logEs[i-1])/(m_logEs[i] - m_logEs[i-1]);
   uu = (costh - m_cosths[j-1])/(m_cosths[j] - m_cosths[j-1]);
   cornerEnergies[0] = m_energies[i-1];
   cornerEnergies[1] = m_energies[i];
   cornerEnergies[2] = m_energies[i];
   cornerEnergies[3] = m_energies[i-1];

   size_t xsize(m_energies.size());
   indx[0] = xsize*(j-1) + (i-1);
   indx[1] = xsize*(j-1) + (i);
   indx[2] = xsize*(j) + (i);
   indx[3] = xsize*(j) + (i-1);
}

int Psf3b::findIndex(const std::vector<double> & xx, double x) {
   typedef std::vector<double>::const_iterator const_iterator_t;

   const_iterator_t ix(std::upper_bound(xx.begin(), xx.end(), x));
   if (ix == xx.end() && x != xx.back()) {
      throw std::invalid_argument("Psf3b::findIndex: x out of range");
   }
   if (x == xx.back()) {
      ix = xx.end() - 1;
   } else if (x <= xx.front()) {
      ix = xx.begin() + 1;
   }
   int i(ix - xx.begin());
   return i;
}

} // namespace latResponse

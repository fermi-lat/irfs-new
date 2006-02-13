/** 
 * @file Edisp.cxx
 * @brief Implementation for LAT energy dispersion class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "CLHEP/Random/RandFlat.h"

#include <tip/IFileSvc.h>
#include <tip/Table.h>

#include "st_facilities/dgaus8.h"

#include "astro/SkyDir.h"

#include "Edisp.h"

namespace {
   bool reverse_cmp(const double & x, const double & y) {
      return x > y;
   }
}

namespace dc2Response {

double Edisp::s_rwidth;
double Edisp::s_ltail;

Edisp::Edisp(const std::string & fitsfile, const std::string & extname) 
   : DC2(fitsfile, extname) {
   readData();
}

double Edisp::value(double appEnergy,
                    double energy, 
                    const astro::SkyDir &srcDir,
                    const astro::SkyDir &scZAxis,
                    const astro::SkyDir &) const {
// Inclination wrt spacecraft z-axis in degrees.
   double theta = srcDir.difference(scZAxis)*180./M_PI;

// The azimuthal angle is not used by the DC2 irfs.
   double phi(0);

   return value(appEnergy, energy, theta, phi);
}

double Edisp::value(double appEnergy, double energy,
                    double theta, double phi) const {
   (void)(phi);

   if (theta < 0) {
      std::ostringstream message;
      message << "dc2Response::Edisp"
              << "::value(double, double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }

   double mu(std::cos(theta*M_PI/180.));

   size_t k = std::upper_bound(m_eBounds.begin(), m_eBounds.end(),
                               energy) - m_eBounds.begin() - 1;
   size_t i = std::upper_bound(m_muBounds.begin(), m_muBounds.end(),
                               mu, ::reverse_cmp) - m_muBounds.begin() - 1;
   size_t indx = i*(m_eBounds.size()-1) + k;
   double p1 = m_ltail.at(indx);
   double p2 = m_rwidth.at(indx);
   double norm = m_norms.at(indx);

   double x((appEnergy - energy)/energy);
   return norm*std::pow(1. + x, p1)/(1. + std::exp(x/p2))/energy;
}

double Edisp::appEnergy(double energy,
                        const astro::SkyDir & srcDir,
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & ) const {
   (void)(srcDir);
   (void)(scZAxis);
   return energy;
}

double Edisp::integral(double emin, double emax, double energy,
                       const astro::SkyDir & srcDir, 
                       const astro::SkyDir & scZAxis,
                       const astro::SkyDir & ) const {
   (void)(emin);
   (void)(emax);
   (void)(energy);
   (void)(srcDir);
   (void)(scZAxis);
//   double phi(0);
//   double theta = srcDir.difference(scZAxis)*180./M_PI;
   return 0;
}
   
double Edisp::integral(double emin, double emax, double energy, 
                       double theta, double phi) const {
   (void)(emin);
   (void)(emax);
   (void)(energy);
   (void)(phi);
   if (theta < 0) {
      std::ostringstream message;
      message << "dc2Response::Edisp"
              << "::integral(double, double, double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }

   return 0;
}

void Edisp::readData() {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

   const tip::Table * edisp = fileSvc.readTable(m_filename, m_extname);

   tip::Table::ConstIterator it(edisp->begin());
   tip::ConstTableRecord & row(*it);

   row["rwidth"].get(m_rwidth);
   row["ltail"].get(m_ltail);

   row["energ_lo"].get(m_eBounds);
   std::vector<double> ehi;
   row["energ_hi"].get(ehi);
   m_eBounds.push_back(ehi.back());

   row["costh_hi"].get(m_muBounds);
   std::vector<double> mulo;
   row["costh_lo"].get(mulo);
   m_muBounds.push_back(mulo.back());

   delete edisp;
   
   computeNorms();
}

void Edisp::computeNorms() {
   m_norms.clear();
   m_norms.reserve(m_rwidth.size());
   for (size_t i = 0; i < m_rwidth.size(); i++) {
      s_rwidth = m_rwidth.at(i);
      s_ltail = m_ltail.at(i);
      double lowerLim(-1);
      double upperLim(1);
      double err(1e-5);
      double edispNorm;
      long ierr;
      dgaus8_(&edispIntegrand, &lowerLim, &upperLim, &err, &edispNorm, &ierr);
      m_norms.push_back(1./edispNorm);
   }
}

double Edisp::edispIntegrand(double * xx) {
   double & x(*xx);
   return std::pow(1. + x, s_ltail)/(1. + std::exp(x/s_rwidth));   
}

} // namespace dc2Response

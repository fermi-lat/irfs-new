/**
 * @file FitsTable.cxx
 * @brief Implementation for FitsTable for accessing IRF data.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "latResponse/Bilinear.h"
#include "latResponse/FitsTable.h"

namespace {
   size_t binIndex(double x, const std::vector<float> & xx) {
      std::vector<float>::const_iterator ix = 
         std::upper_bound(xx.begin(), xx.end(), x);
      return ix - xx.begin();
   }
}

namespace latResponse {

FitsTable::FitsTable(const std::string & filename,
                     const std::string & extname,
                     const std::string & tablename)
   : m_interpolator(0) {

   const tip::Table * table(tip::IFileSvc::instance().readTable(filename, 
                                                                extname));

   std::vector<float> elo, ehi;
   getVectorData(table, "ENERG_LO", elo);
   getVectorData(table, "ENERG_HI", ehi);
   for (size_t k(0); k < elo.size(); k++) {
      m_ebounds.push_back(std::log10(elo.at(k)));
      m_logEnergies.push_back(std::log10(std::sqrt(elo.at(k)*ehi.at(k))));
   }
   m_ebounds.push_back(std::log10(ehi.back()));

   std::vector<float> mulo, muhi;
   getVectorData(table, "CTHETA_LO", mulo);
   getVectorData(table, "CTHETA_HI", muhi);
   for (size_t i(0); i < muhi.size(); i++) {
      m_tbounds.push_back(mulo.at(i));
      m_mus.push_back((m_tbounds.at(i) + muhi.at(i))/2.);
   }
   m_tbounds.push_back(muhi.back());

   m_minCosTheta = mulo.front();

   getVectorData(table, tablename, m_values);
   m_maxValue = m_values.front();
   for (size_t i(1); i < m_values.size(); i++) {
      if (m_values.at(i) > m_maxValue) {
         m_maxValue = m_values.at(i);
      }
   }
//   m_interpolator = new Bilinear(m_logEnergies, m_mus, m_values);

// Replicate nasty TH2 and RootEval::Table behavior from handoff_response,
// by passing xlo, xhi, ylo, yhi values
   float xlo, xhi, ylo, yhi;
   m_interpolator = new Bilinear(m_logEnergies, m_mus, m_values,
                                 xlo=0., xhi=10., ylo=-1., yhi=1.);

   delete table;
}

FitsTable::FitsTable() : m_interpolator(0) {}

FitsTable::FitsTable(const FitsTable & rhs) 
   : m_interpolator(0), m_logEnergies(rhs.m_logEnergies), m_mus(rhs.m_mus),
     m_values(rhs.m_values), m_ebounds(rhs.m_ebounds),
     m_tbounds(rhs.m_tbounds), m_minCosTheta(rhs.m_minCosTheta), 
     m_maxValue(rhs.m_maxValue) {
//    m_interpolator = new Bilinear(m_logEnergies, m_mus, m_values);
   m_interpolator = new Bilinear(m_logEnergies, m_mus, m_values,
                                 0, 10, -1, 1);
}

FitsTable::~FitsTable() { 
   delete m_interpolator;
}

double FitsTable::
value(double logenergy, double costh, bool interpolate) const {
   if (interpolate) {
      if (costh > m_mus.back()) {
         costh = m_mus.back();
      }
      return (*m_interpolator)(logenergy, costh);
   }

   double maxloge(*(m_logEnergies.end() - 2)); 
   if (logenergy >= maxloge) { // use last bin
      logenergy = maxloge;
   }
   if (logenergy <= m_logEnergies.at(1)) { // use first bin
      logenergy = m_logEnergies.at(1);
   }

   size_t ix = binIndex(logenergy, m_ebounds);
   size_t iy = binIndex(costh, m_tbounds);
   if (iy == 0) {
      iy = 1;
   } 
   size_t indx = (iy - 1)*m_logEnergies.size() + ix - 1;

   return m_values.at(indx);
}

void FitsTable::getVectorData(const tip::Table * table,
                              const std::string & fieldName,
                              std::vector<float> & values) {
   values.clear();

   tip::Table::ConstIterator it(table->begin());
   tip::ConstTableRecord & row(*it);

   row[fieldName].get(values);
}

} // namespace latResponse

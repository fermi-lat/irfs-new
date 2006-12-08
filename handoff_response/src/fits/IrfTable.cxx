/**
 * @file IrfTable.cxx
 * @brief  Abstraction for a ROOT TH2F table.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <stdexcept>

#include "TH2F.h"

#include "IrfTable.h"

namespace handoff_response {

IrfTable::IrfTable(TH2F * table) {
   TAxis * xx = table->GetXaxis();
   int nbins(xx->GetNbins() + 2);
   m_xaxis.clear();
   for (int i = 0; i < nbins; i++) {
      m_xaxis.push_back(xx->GetBinLowEdge(i));
   }

   TAxis * yy = table->GetYaxis();
   nbins = yy->GetNbins() + 2;
   m_yaxis.clear();
   for (int i = 0; i < nbins; i++) {
      m_yaxis.push_back(yy->GetBinLowEdge(i));
   }

   m_values.clear();
   for (size_t j = 0; j < m_yaxis.size()-1; j++) {
      for (size_t i = 0; i < m_xaxis.size()-1; i++) {
         m_values.push_back(table->GetBinContent(i, j));
      }
   }
}

size_t IrfTable::index(size_t i , size_t j) const {
   if (i >= m_xaxis.size()) {
      throw std::runtime_error("Request for x index outside of table bounds.");
   }
   if (j >= m_yaxis.size()) {
      throw std::runtime_error("Request for y index outside of table bounds.");
   }
   return j*m_xaxis.size() + i;
}

size_t IrfTable::index(double x, double y) const {
   std::vector<double>::const_iterator ix =
      std::upper_bound(m_xaxis.begin(), m_xaxis.end(), x);
   if (ix == m_xaxis.end()) {
      throw std::runtime_error("Request for x value outside of table bounds.");
   }
   std::vector<double>::const_iterator iy =
      std::upper_bound(m_yaxis.begin(), m_yaxis.end(), y);
   if (iy == m_yaxis.end()) {
      throw std::runtime_error("Request for y value outside of table bounds.");
   }

   size_t i = ix - m_xaxis.begin() - 1;
   size_t j = iy - m_yaxis.begin() - 1;
   return index(i, j);
}

} // namespace handoff_response

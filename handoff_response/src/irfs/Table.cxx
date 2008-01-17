/**
 * @file Table.cxx
 * @brief Implementation for Table abstraction of IRF data.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cassert>

#include <iostream>

#include "TH2F.h"

#include "Bilinear.h"
#include "Table.h"
#include <algorithm>

namespace {
   int bin_index(double x, const std::vector<float> & xx) {
      std::vector<float>::const_iterator ix = 
         std::upper_bound(xx.begin(), xx.end(), x);
      return ix - xx.begin();
   }
}

namespace handoff_response {

Table::Table(TH2F * hist) 
   : m_hist(hist), m_minCosTheta(hist->GetYaxis()->GetBinLowEdge(1)),
     m_maxValue(hist->GetMaximum()), m_interpolator(0) {
   
   binArray( 0, 10, hist->GetXaxis(), m_energy_axis, m_ebounds);
   binArray(-1., 1., hist->GetYaxis(), m_angle_axis, m_tbounds);

   for (Bilinear::const_iterator iy = m_angle_axis.begin(); 
        iy != m_angle_axis.end(); ++iy) {
      float costh(*iy);
      if (costh == 1.0) { // avoid edge in histogram
         costh = 0.999; 
      }
      for (Bilinear::const_iterator ix = m_energy_axis.begin(); 
           ix != m_energy_axis.end(); ++ix) {
         float loge(*ix);
         int bin(hist->FindBin(loge, costh));
         double value(static_cast<double>(hist->GetBinContent(bin)));
         m_data_array.push_back(value);
      }
   }
   m_interpolator = new Bilinear(m_energy_axis, m_angle_axis, m_data_array);
}

Table::~Table() { 
   delete m_interpolator;
   delete m_hist;
   m_hist = 0;
}
    
double Table::maximum() {
   return m_maxValue;
}

void Table::binArray(double low_limit, double high_limit, 
                     TAxis * axis, std::vector<float> & array,
                     std::vector<float> & bounds) {
   array.clear();
   bounds.clear();
   bounds.push_back(low_limit);
   array.push_back(low_limit);
   int nbins(axis->GetNbins());
   for (int i = 1; i < nbins + 1; ++i) {
      array.push_back(axis->GetBinCenter(i));
      bounds.push_back(axis->GetBinLowEdge(i));
   }
   bounds.push_back(axis->GetBinUpEdge(nbins));
   array.push_back(high_limit);
}

double Table::value(double logenergy, double costh, bool interpolate) {
   if (interpolate) {
      return (*m_interpolator)(logenergy, costh);
   }

// non-interpolating: look up value for the bin 
   double maxloge(*(m_energy_axis.end()-2)); // if go beyond this, use last bin
   if (logenergy >= maxloge) {
      logenergy = maxloge;
   }
   if (logenergy <= m_energy_axis.at(1)) {   // use first bin if necessary
      logenergy = m_energy_axis.at(1);
   }

   int bin = m_hist->FindBin(logenergy, costh);
   float root_value = m_hist->GetBinContent(bin);

   int ix = bin_index(logenergy, m_ebounds);
   int iy = bin_index(costh, m_tbounds);
   int indx = (iy - 1)*m_energy_axis.size() + ix - 1;
   if (root_value != m_data_array.at(indx)) {
      std::cout << "bin = " << bin << std::endl
                << "indx = " << indx << std::endl;
      assert(false);
   }
   return m_data_array.at(indx);
}

} // namespace handoff_response
